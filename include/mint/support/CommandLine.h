/* ================================================================== *
 * Mint.
 * ================================================================== */

#ifndef MINT_SUPPORT_COMMANDLINE_H
#define MINT_SUPPORT_COMMANDLINE_H

#ifndef MINT_COLLECTIONS_ARRAYREFREF_H
#include "mint/collections/ArrayRef.h"
#endif

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#ifndef MINT_COLLECTIONS_TABLE_H
#include "mint/collections/Table.h"
#endif

namespace mint {
namespace cl {

class OptionBase;
class Parser;
typedef char ** iterator;

/** -------------------------------------------------------------------------
    Argument for building a command-line options.
 */
class OptionArg {
public:
  virtual void apply(OptionBase * option) const = 0;
};

/** -------------------------------------------------------------------------
    Specify a description for an option.
 */
class Description : public OptionArg {
public:
  Description(StringRef desc) : _desc(desc) {}

  void apply(OptionBase * option) const;

private:
  StringRef _desc;
};

/** -------------------------------------------------------------------------
    Specify what group an option is in.
 */
class Group : public OptionArg {
public:
  Group(StringRef group) : _group(group) {}

  void apply(OptionBase * option) const;

private:
  StringRef _group;
};

/** -------------------------------------------------------------------------
    Specify the abbreviated form of an option.
 */
class Abbrev : public OptionArg {
public:
  Abbrev(StringRef abbrev) : _abbrev(abbrev) {}

  void apply(OptionBase * option) const;

private:
  StringRef _abbrev;
};

/** -------------------------------------------------------------------------
    Base class for command-line option.
 */
class OptionBase {
public:

  /// Constructor
  OptionBase(StringRef name);

  /// The name of the option
  StringRef name() const { return _name; }
  void setName(StringRef name) { _name = name; }

  /// A description of the option
  StringRef description() const { return _description; }
  void setDescription(StringRef description) { _description = description; }

  /// What group the option is in
  StringRef group() const { return _group; }
  void setGroup(StringRef group) { _group = group; }

  /// Abbreviated name of this option
  StringRef abbrev() const { return _abbrev; }
  void setAbbrev(StringRef abbrev) { _abbrev = abbrev; }

  /// Function to parse the value of an option.
  virtual void parse(StringRef argName, StringRef argValue) = 0;

  /// Whether this option is present on the command line
  bool present() const { return _present; }

  /// Next option in initialization chain
  OptionBase * next() const { return _next; }

protected:
  friend class Parser;

  OptionBase * _next;
  StringRef _name;
  StringRef _abbrev;
  StringRef _description;
  StringRef _group;
  bool _present;
};

/** -------------------------------------------------------------------------
    TableKeyTraits for Options.
 */
struct OptionKeyTraits {
  static inline unsigned hash(const OptionBase * key) {
    return key->name().hash();
  }

  static inline unsigned equals(const OptionBase * sl, const OptionBase * sr) {
    return sl == sr || sl->name() == sr->name();
  }

  static inline unsigned hash(StringRef key) {
    return key.hash();
  }

  static inline unsigned equals(const OptionBase * sl, StringRef sr) {
    return sl->name() == sr;
  }
};

/// Defines a set of options
typedef Table<OptionBase, char, OptionKeyTraits> OptionSet;

/** -------------------------------------------------------------------------
    A command-line option.
 */
template<class T>
class Option : public OptionBase {
public:
  /// Constructors
  Option(StringRef name) : OptionBase(name), _value() {}
  Option(StringRef name, const OptionArg & a0) : OptionBase(name), _value() {
    a0.apply(this);
  }
  Option(StringRef name, const OptionArg & a0, const OptionArg & a1) : OptionBase(name), _value() {
    a0.apply(this);
    a1.apply(this);
  }
  Option(StringRef name, const OptionArg & a0, const OptionArg & a1, const OptionArg & a2)
    : OptionBase(name), _value() {
    a0.apply(this);
    a1.apply(this);
    a2.apply(this);
  }
  Option(StringRef name, const OptionArg & a0, const OptionArg & a1, const OptionArg & a2,
      const OptionArg & a3)
    : OptionBase(name), _value() {
    a0.apply(this);
    a1.apply(this);
    a2.apply(this);
    a3.apply(this);
  }
  Option(StringRef name, const OptionArg & a0, const OptionArg & a1, const OptionArg & a2,
      const OptionArg & a3, const OptionArg & a4)
    : OptionBase(name), _value() {
    a0.apply(this);
    a1.apply(this);
    a2.apply(this);
    a3.apply(this);
    a4.apply(this);
  }

  void parse(StringRef argName, StringRef argValue);

  const T & value() const { return _value; }
  operator const T &() const { return _value; }

protected:
  T _value;
};

/** -------------------------------------------------------------------------
    Defines a group of options.
 */
class OptionGroup {
public:
  OptionGroup(StringRef name, StringRef description);

  /// The name of the option
  StringRef name() const { return _name; }

  /// A description of the option
  StringRef description() const { return _description; }

  /// Get a list of all options, in sorted order
  void getOptions(SmallVectorImpl<OptionBase *> & out) const;

  /// Next option group in the initialization chain.
  OptionGroup * next() const { return _next; }

  /// Add an option to this group.
  void addOption(OptionBase * option) { _options[option] = 0; }

private:
  friend class Parser;

  OptionGroup * _next;
  StringRef _name;
  StringRef _description;
  OptionSet _options;
};

/** -------------------------------------------------------------------------
    TableKeyTraits for Option Groups.
 */
struct OptionGroupKeyTraits {
  static inline unsigned hash(const OptionGroup * key) {
    return key->name().hash();
  }

  static inline unsigned equals(const OptionGroup * sl, const OptionGroup * sr) {
    return sl == sr || sl->name() == sr->name();
  }

  static inline unsigned hash(StringRef key) {
    return key.hash();
  }

  static inline unsigned equals(const OptionGroup * sl, StringRef sr) {
    return sl->name() == sr;
  }
};

/// Defines a set of options
typedef Table<OptionGroup, OptionBase, OptionGroupKeyTraits> OptionGroupMap;

/// Print out help information for the specified option group.
void showHelp(StringRef group);

/** -------------------------------------------------------------------------
    Command-line option parser.
 */
class Parser {
public:
  /// Parse all options that begin with '-', and return a pointer to the next unparsed option.
  /// Only consider options that are in the specified option groups.
  static char ** parse(ArrayRef<StringRef> groups, iterator first, iterator last);

};

}}

#endif // MINT_SUPPORT_COMMANDLINE_H
