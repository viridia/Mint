/* ================================================================== *
 * Node
 * ================================================================== */

#include "mint/graph/Type.h"
#include "mint/support/Assert.h"
#include "mint/support/OStream.h"
#include "mint/support/Hashing.h"

namespace mint {

// -------------------------------------------------------------------------
// Type
// -------------------------------------------------------------------------

#ifdef TYPE_KIND
#undef TYPE_KIND
#endif

#define TYPE_KIND(x) #x,
#define TYPE_KIND_RANGE(x, start, end)

static const char * typeKindNames[] = {
#include "mint/graph/TypeKind.def"
};

#undef TYPE_KIND

const char * Type::kindName(TypeKind tk) {
  if (unsigned(tk) < (sizeof(typeKindNames) / sizeof(typeKindNames[0]))) {
    return typeKindNames[unsigned(tk)];
  }
  return "<Invalid Type Kind>";
}

void Type::print(OStream & strm) const {
  switch (_typeKind) {
    case VOID: strm << "void"; break;
    case ANY: strm << "any"; break;
    case BOOL: strm << "bool"; break;
    case INTEGER: strm << "int"; break;
    case FLOAT: strm << "float"; break;
    case STRING: strm << "string"; break;

    case LIST: {
      strm << "list";
      break;
    }

    case DICTIONARY: {
      strm << "dict";
      break;
    }

    case FUNCTION: {
      strm << "function";
      break;
    }

    case OBJECT: {
      strm << "object";
      break;
    }

    case MODULE: {
      strm << "module"; break;
    }

    case PROJECT: {
      strm << "project"; break;
    }

    default: strm << "<bad type>"; break;
  }
}

// -------------------------------------------------------------------------
// DerivedType
// -------------------------------------------------------------------------

DerivedType * DerivedType::create(TypeKind tk, TypeArray args) {
  size_t size = sizeof(DerivedType) + sizeof(Type *) * (args.size() - 1);
  char * mem = new char[size];
  return new (mem) DerivedType(tk, args);
}

DerivedType::DerivedType(TypeKind typeKind, TypeArray params)
  : Type(typeKind)
  , _size(params.size())
{
  Type ** dst = _data;
  for (TypeArray::const_iterator it = params.begin(), itEnd = params.end(); it != itEnd; ++it) {
    Type * n = *it;
    RefCountable::acquire(n);
    *dst++ = n;
  }
}

DerivedType::~DerivedType() {
  for (TypeArray::const_iterator it = params().begin(), itEnd = params().end(); it != itEnd; ++it) {
    const Node * n = *it;
    RefCountable::release(n);
  }
}

Type * DerivedType::param(unsigned index) const {
  M_ASSERT(index < _size) << "Type parameter index " << index << " out of range";
  return _data[index];
}

void DerivedType::print(OStream & strm) const {
  Type::print(strm);
  strm << "[";
  for (TypeArray::const_iterator it = params().begin(), itEnd = params().end(); it != itEnd; ++it) {
    if (it != params().begin()) {
      strm << ", ";
    }
    const Node * n = *it;
    n->print(strm);
  }
  strm << "]";
}

unsigned DerivedType::hash() const {
  unsigned hashVal = ::mint::hash(
      reinterpret_cast<const char *>(&_data[0]),
      reinterpret_cast<const char *>(&_data[_size]));
  return hashVal ^ (unsigned(typeKind()) << 2);
}

}
