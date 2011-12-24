/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/Type.h"
#include "mint/graph/String.h"

#include "mint/intrinsic/TypeRegistry.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Project.h"
#include "mint/project/ProjectWriterXml.h"

#include "mint/support/Assert.h"
#include "mint/support/Path.h"

#if HAVE_ALGORITHM
#include <algorithm>
#endif

namespace mint {

struct StringDictComparator {
  inline bool operator()(
      const StringDict<Node>::value_type & lhs, const StringDict<Node>::value_type & rhs) {
    M_ASSERT(lhs.first != NULL);
    M_ASSERT(rhs.first != NULL);
    return lhs.first->value().compare(rhs.first->value()) < 0;
  }
};

ProjectWriterXml & ProjectWriterXml::writeBuildConfiguration(BuildConfiguration * bc) {
  _strm << "<?xml version=\"1.0\"?>\n";
  _strm << "<build-configuration>\n";
  indent();
  if (bc->mainProject() != NULL) {
    writeProject(bc->mainProject(), true);
  }
  unindent();
  _strm << "</build-configuration>\n";
  return *this;
}

ProjectWriterXml & ProjectWriterXml::writeProject(Project * proj, bool isMain) {
  _strm.indent(_indentLevel * 2) << "<project source=\"" << proj->sourceRoot() << "\"";
  if (isMain) {
    _strm << " main=\"true\"";
  }
  _strm << ">\n";
  indent();

  // Options
  SmallVector<StringDict<Object>::value_type, 32> options;
  proj->getProjectOptions(options);

  _strm.indent(_indentLevel * 2) << "<options>\n";
  indent();

  for (SmallVectorImpl<StringDict<Object>::value_type >::const_iterator
      it = options.begin(), itEnd = options.end();
      it != itEnd; ++it) {
    String * optName = it->first;
    Object * option = it->second;
    //String * optHelp = String::dyn_cast(option->getAttributeValue("help"));
    AttributeLookup value;
    if (!option->getAttribute("value", value)) {
      M_ASSERT(false) << "Option " << optName << " value not found!";
    }
    M_ASSERT(value.value != NULL);
    Type * optType = value.value->type();

    _strm.indent(_indentLevel * 2) << "<option name=\"" << optName->value() << "\">\n";
    indent();

    // Convert underscores to dashes.
    SmallString<32> name(optName->value());
    for (SmallVectorImpl<char>::iterator it = name.begin(), itEnd = name.end(); it != itEnd; ++it) {
      if (*it == '_') {
        *it = '-';
      }
    }

    if (optType) {
      _strm.indent(_indentLevel * 2) << "<type>" << optType << "</type>\n";
    }

    if (value.value) {
      _strm.indent(_indentLevel * 2) << "<value>" << value.value << "</value>\n";
    }

    unindent();
    _strm.indent(_indentLevel * 2) << "</option>\n";
  }

  unindent();
  _strm.indent(_indentLevel * 2) << "</options>\n";

  // Configuration variables
  if (proj->mainModule()) {
    _strm.indent(_indentLevel * 2) << "<config-cache>\n";
    writeCachedVars(proj->mainModule());
    _strm.indent(_indentLevel * 2) << "</config-cache>\n";
  }

  if (proj->mainModule()) {
    _strm.indent(_indentLevel * 2) << "<module source-dir=\"" << proj->mainModule()->sourceDir() << "\">\n";
    indent();
    _strm.indent(_indentLevel * 2) << "<targets>\n";
    writeTargets(proj->mainModule());
    _strm.indent(_indentLevel * 2) << "</targets>\n";
    unindent();
    _strm.indent(_indentLevel * 2) << "</module>\n";
  }
  // Config vars
  // Targets
  unindent();
  _strm.indent(_indentLevel * 2) << "</project>\n";
  return *this;
}

ProjectWriterXml & ProjectWriterXml::write(Node * node, bool isDefinition) {
  writeValue(node, isDefinition);
  return *this;
}

ProjectWriterXml & ProjectWriterXml::write(Module * module) {
  _strm << "module(" << module->name() << ") {\n";
  ++_indentLevel;
  Node * savedScope = setActiveScope(module);
  _activeModule = module;
  for (SmallVectorImpl<String *>::const_iterator
      it = module->keyOrder().begin(), itEnd = module->keyOrder().end(); it != itEnd; ++it) {
    Node * n = module->attrs()[*it];
    if (filter(n)) {
      _strm.indent(_indentLevel * 2);
      _strm << *it << " = ";
      write(n, true);
      _strm << "\n";
    }
  }
  setActiveScope(savedScope);
  _activeModule = NULL;
  --_indentLevel;
  _strm.indent(_indentLevel * 2);
  _strm << "}\n";
  return *this;
}

ProjectWriterXml & ProjectWriterXml::write(ArrayRef<Node *> nodes, bool isDefinition) {
  for (ArrayRef<Node *>::iterator it = nodes.begin(), itEnd = nodes.end(); it != itEnd; ++it) {
    _strm.indent(_indentLevel * 2);
    write(*it, isDefinition);
    _strm << "\n";
  }
  return *this;
}

ProjectWriterXml & ProjectWriterXml::writeCachedVars(Module * module) {
  ++_indentLevel;
  Node * savedScope = setActiveScope(module);
  _activeModule = module;
  for (SmallVectorImpl<String *>::const_iterator
      it = module->keyOrder().begin(), itEnd = module->keyOrder().end(); it != itEnd; ++it) {
    Node * n = module->attrs()[*it];
    Object * obj = n->asObject();
    if (obj != NULL) {
      if (obj->inheritsFrom(TypeRegistry::optionType()) || obj->inheritsFrom(TypeRegistry::targetType())) {
        continue;
      }
      if (hasRelativePath(obj) && hasCachedVars(obj)) {
        _strm.indent(_indentLevel * 2) << "<object id=\"";
        writeRelativePath(obj);
        _strm << "\">\n";
        ++_indentLevel;
        for (Attributes::const_iterator
            it = obj->attrs().begin(), itEnd = obj->attrs().end(); it != itEnd; ++it) {
          AttributeLookup lookup;
          if (obj->getAttribute(it->first->value(), lookup) &&
              lookup.definition != NULL &&
              lookup.definition->isCached()) {
            _strm.indent(_indentLevel * 2) << "<attribute name=\"" << it->first->value()
                << "\" type=\"" << lookup.definition->type() << "\">";
            write(lookup.value, false);
            _strm << "</attribute>\n";
          }
        }
        --_indentLevel;
        _strm.indent(_indentLevel * 2) << "</object>\n";
      }
    }
  }
  setActiveScope(savedScope);
  _activeModule = NULL;
  --_indentLevel;
  return *this;
}

bool ProjectWriterXml::hasCachedVars(Object * obj) {
  for (Node * n = obj; n != NULL; n = n->type()) {
    Object * o = n->asObject();
    if (o != NULL) {
      for (Attributes::const_iterator
          it = o->attrs().begin(), itEnd = o->attrs().end(); it != itEnd; ++it) {
        if (it->second->nodeKind() == Node::NK_ATTRDEF) {
          AttributeDefinition * attrDef = static_cast<AttributeDefinition *>(it->second);
          if (attrDef->isCached()) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

void ProjectWriterXml::writeCachedVars(Node * scope, String * name, Node * value) {
  AttributeLookup lookup;
  if (scope->getAttribute(name->value(), lookup) &&
      lookup.definition != NULL &&
      lookup.definition->isCached()) {
    _strm.indent(_indentLevel * 2);
    writeRelativePath(scope);
    _strm << name << " = ";
    write(value, false);
    _strm << "\n";
  } else {
    Object * obj = value->asObject();
    if (obj != NULL && !obj->attrs().empty()) {
      SmallVector<Attributes::value_type, 64 > objectProperties;
      objectProperties.resize(obj->attrs().size());
      std::copy(obj->attrs().begin(), obj->attrs().end(), objectProperties.begin());
      std::sort(objectProperties.begin(), objectProperties.end(), StringDictComparator());
      for (SmallVectorImpl<Attributes::value_type>::const_iterator
          it = objectProperties.begin(), itEnd = objectProperties.end(); it != itEnd; ++it) {
        if (filter(it->second)) {
          writeCachedVars(obj, it->first, it->second);
        }
      }
    }
  }
}

ProjectWriterXml & ProjectWriterXml::writeTargets(Module * module) {
  return *this;
}

bool ProjectWriterXml::writeValue(Node * node, bool isDefinition) {
  switch (node->nodeKind()) {
    case Node::NK_UNDEFINED:
      break;

    case Node::NK_BOOL:
    case Node::NK_INTEGER:
    case Node::NK_FLOAT:
    case Node::NK_STRING:
    case Node::NK_TYPENAME:
    case Node::NK_IDENT:
      _strm << node;
      return true;

    case Node::NK_LIST:
      writeList(static_cast<Oper *>(node));
      return true;

    case Node::NK_DICT:
      writeDict(static_cast<Object *>(node));
      return true;

    case Node::NK_FUNCTION:
      return false;

    case Node::NK_OBJECT:
      return writeObject(static_cast<Object *>(node), isDefinition);

    case Node::NK_ACTION_COMMAND: {
      Oper * op = static_cast<Oper *>(node);
      _strm << "command(";
      writeValue(op->arg(0));
      _strm << ",";
      writeValue(op->arg(1));
      _strm << ")";
      break;
    }

    case Node::NK_ACTION_CLOSURE: {
      Oper * op = static_cast<Oper *>(node);
      _strm << "aclosure(";
      writeValue(op->arg(0));
      _strm << ",";
      writeValue(op->arg(1));
      _strm << ")";
      break;
    }

    default:
      console::err() << "Invalid node type for writing: " << node->nodeKind() << "\n";
      console::err() << "Node value is: " << node << "\n";
      return false;
  }
  return false;
}

void ProjectWriterXml::writeList(Oper * list) {
  if (list->size() == 0) {
    _strm << "[]";
    return;
  }
  _strm << "[\n";
  ++_indentLevel;
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = *it;
    if (filter(n)) {
      if (it != list->begin()) {
        _strm << ",\n";
      }
      _strm.indent(_indentLevel * 2);
      write(n, false);
    }
  }
  --_indentLevel;
  _strm << "\n";
  _strm.indent(_indentLevel * 2);
  _strm << "]";
}

void ProjectWriterXml::writeDict(Object * dict) {
  _strm << "{\n";
  ++_indentLevel;
  SmallVector<Attributes::value_type, 64 > dictProperties;
  dictProperties.resize(dict->attrs().size());
  std::copy(dict->attrs().begin(), dict->attrs().end(), dictProperties.begin());
  std::sort(dictProperties.begin(), dictProperties.end(), StringDictComparator());
  for (SmallVectorImpl<Attributes::value_type>::const_iterator
      it = dictProperties.begin(), itEnd = dictProperties.end(); it != itEnd; ++it) {
    _strm.indent(_indentLevel * 2);
    _strm << it->first << " = ";
    write(it->second, true);
    _strm << "\n";
  }
  --_indentLevel;
  _strm.indent(_indentLevel * 2);
  _strm << "}";
}

bool ProjectWriterXml::writeObject(Object * obj, bool isDefinition) {
  // Write a reference to the object instead of the literal body
  if (!isDefinition && hasRelativePath(obj)) {
    if (obj->parentScope()) {
      _strm << "\"";
      writeRelativePath(obj->parentScope());
      _strm << obj->name();
      _strm << "\"";
    } else {
      _strm << obj->name();
    }
    return true;
  }

  // Write the proto
  if (obj->prototype() != NULL) {
    if (obj->prototype()->name() != NULL) {
      _strm << obj->prototype()->name();
    } else {
      writeObject(obj->prototype(), false);
    }
  }
  if (obj->attrs().empty()) {
    _strm << " {}";
    return true;
  }
  _strm << " {\n";
  ++_indentLevel;
  Node * savedScope = setActiveScope(obj);
  writeObjectContents(obj);
  setActiveScope(savedScope);
  --_indentLevel;
  _strm.indent(_indentLevel * 2);
  _strm << "}";
  return true;
}

void ProjectWriterXml::writeObjectContents(Object * obj) {
  SmallVector<Attributes::value_type, 64 > objectProperties;
  objectProperties.resize(obj->attrs().size());
  std::copy(obj->attrs().begin(), obj->attrs().end(), objectProperties.begin());
  std::sort(objectProperties.begin(), objectProperties.end(), StringDictComparator());
  for (SmallVectorImpl<Attributes::value_type>::const_iterator
      it = objectProperties.begin(), itEnd = objectProperties.end(); it != itEnd; ++it) {
    if (!filter(it->second)) {
      continue;
    }
    if (it->second->nodeKind() == Node::NK_UNDEFINED || it->second->nodeKind() == Node::NK_MODULE) {
      // TODO: This is kind of a hack, figure a better way to suppress unimportant attrs.
      continue;
    } else if (it->second->nodeKind() == Node::NK_ATTRDEF) {
      AttributeDefinition * attrDef = static_cast<AttributeDefinition *>(it->second);
      _strm.indent(_indentLevel * 2);
      _strm << "param " << it->first << " : " << attrDef->type() << " = ";
      write(attrDef->value(), false);
      _strm << "\n";
    } else {
      _strm.indent(_indentLevel * 2);
      _strm << it->first << " = ";
      write(it->second, false);
      _strm << "\n";
    }
  }
}

void ProjectWriterXml::writeRelativePath(Node * scope) {
  M_ASSERT(scope != NULL);
  if (scope == _activeScope) {
    return;
  } else if (scope->nodeKind() == Node::NK_MODULE) {
    if (scope != _activeModule) {
      Module * m = static_cast<Module *>(scope);
      _strm << path::parent(m->name()->value());
    } else {
      _strm << "#";
    }
  } else if (scope->nodeKind() == Node::NK_OBJECT) {
    Object * obj = static_cast<Object *>(scope);
    if (obj->parentScope() != NULL) {
      writeRelativePath(obj->parentScope());
    }
    _strm << obj->name();
  }
}

bool ProjectWriterXml::hasRelativePath(Object * obj) {
  for (Node * scope = obj; scope != NULL; scope = scope->parentScope()) {
    if (scope == _activeScope) {
      return true;
    } else if (scope->nodeKind() == Node::NK_MODULE) {
      return true;
    } else if (scope->nodeKind() == Node::NK_OBJECT) {
      Object * scopeObj = static_cast<Object *>(scope);
      if (scopeObj->name() == NULL) {
        return false;
      }
    } else {
      return false;
    }
  }
  return false;
}

bool ProjectWriterXml::filter(Node * n) {
  return true;
}

}
