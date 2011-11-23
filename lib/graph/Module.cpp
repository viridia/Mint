/* ================================================================== *
 * Module
 * ================================================================== */

#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/project/Project.h"

#include "mint/support/OStream.h"

namespace mint {

void Module::setProperty(String * name, Node * value) {
  _properties[name] = value;
  _keyOrder.push_back(name);
}

Node * Module::getPropertyValue(String * name) const {
  StringDict<Node>::const_iterator it = _properties.find(name);
  if (it != _properties.end()) {
    return it->second;
  }
  for (ImportList::const_iterator m = _importScopes.end(); m != _importScopes.begin(); ) {
    --m;
    it = (*m)->_properties.find(name);
    if (it != (*m)->_properties.end()) {
      return it->second;
    }
  }
  return NULL;
}

void Module::findTargets(SmallVectorImpl<Object *> & out) const {
  Fundamentals * fundamentals = _project->fundamentals();
  if (fundamentals == NULL) {
    return;
  }
  Object * target = fundamentals->target.ptr();
  for (StringDict<Node>::const_iterator it = _properties.begin(), itEnd = _properties.end();
      it != itEnd; ++it) {
    Node * n = it->second;
    if (n->nodeKind() == Node::NK_OBJECT) {
      Object * obj = static_cast<Object *>(n);
      if (obj->inheritsFrom(target)) {
        out.push_back(obj);
      }
    }
  }
}

void Module::writeTargets(OStream & strm, StringRef modulePath) const {
  Fundamentals * fundamentals = _project->fundamentals();
  if (fundamentals == NULL) {
    return;
  }
  Object * target = fundamentals->target.ptr();
  for (StringDict<Node>::const_iterator it = _properties.begin(), itEnd = _properties.end();
      it != itEnd; ++it) {
    Node * n = it->second;
    if (n->nodeKind() == Node::NK_OBJECT) {
      Object * obj = static_cast<Object *>(n);
      if (obj->inheritsFrom(target)) {
        Node * sources = obj->getPropertyValue("sources");
        Node * outputs = obj->getPropertyValue("outputs");
        Node * depends = obj->getPropertyValue("depends");

        SmallString<64> path(modulePath);
        path.push_back(':');
        path.append(it->first->value());
        strm << "    target {\n";
        strm << "      name = \"" << path << "\"\n";
        if (sources != NULL && sources->nodeKind() == Node::NK_LIST) {
          Oper * op = static_cast<Oper *>(sources);
          if (op->size() > 0) {
            strm << "      sources = [\n";
            for (Oper::const_iterator li = op->begin(), liEnd = op->end(); li != liEnd; ++li) {
              strm << "        " << *li << ",\n";
            }
            strm << "      ]\n";
          }
        }
        if (outputs != NULL &&
            (outputs->nodeKind() == Node::NK_LIST)) {
          Oper * op = static_cast<Oper *>(outputs);
          if (op->size() > 0) {
            strm << "      outputs = [\n";
            for (Oper::const_iterator li = op->begin(), liEnd = op->end(); li != liEnd; ++li) {
              strm << "        " << *li << ",\n";
            }
            strm << "      ]\n";
          }
        }
        if (depends != NULL &&
            (depends->nodeKind() == Node::NK_LIST)) {
          Oper * op = static_cast<Oper *>(depends);
          if (op->size() > 0) {
            strm << "      depends = [\n";
            for (Oper::const_iterator li = op->begin(), liEnd = op->end(); li != liEnd; ++li) {
              strm << "        " << *li << ",\n";
            }
            strm << "      ]\n";
          }
        }
        strm << "    },\n";
      }
    }
  }
  // Now handle included modules...
}

}
