/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/support/CommandLine.h"
#include "mint/support/Diagnostics.h"

namespace mint {
namespace cl {

namespace {

  OptionBase * _options = NULL;
  OptionGroup * _groups = NULL;
  OptionGroupMap _groupMap;

  void initGroupMap() {
    static bool initialized = false;
    if (!initialized) {
      initialized = true;
      for (OptionGroup * grp = _groups; grp != NULL; grp = grp->next()) {
        OptionGroupMap::const_iterator it = _groupMap.find_as(grp->name());
        if (it != _groupMap.end()) {
          diag::error() << "Option group '" << grp->name() << "' already defined!";
          exit(-1);
        }
        _groupMap[grp] = NULL;
      }

      for (OptionBase * opt = _options; opt != NULL; opt = opt->next()) {
        OptionGroupMap::const_iterator it = _groupMap.find_as(opt->group());
        if (it == _groupMap.end()) {
          diag::error() << "Option group '" << opt->group() << "' not found!";
          exit(-1);
        }
        OptionGroup * group = it->first;
        group->addOption(opt);
      }
    }
  }
}

void Description::apply(OptionBase * option) const {
  option->setDescription(_desc);
}

void Group::apply(OptionBase * option) const {
  option->setGroup(_group);
}

void Abbrev::apply(OptionBase * option) const {
  option->setAbbrev(_abbrev);
}

// -------------------------------------------------------------------------
// OptionBase
// -------------------------------------------------------------------------

OptionBase::OptionBase(StringRef name) : _name(name), _present(false) {
  _next = _options;
  _options = this;
}

// -------------------------------------------------------------------------
// Option<bool>
// -------------------------------------------------------------------------

template<>
void Option<bool>::parse(StringRef argName, StringRef argValue) {
  if (argValue.empty()) {
    _value = true;
    _present = true;
  } else {
    diag::error() << "Invalid value for option '" << argName << "': " << argValue;
  }
}

// -------------------------------------------------------------------------
// Option<StringRef>
// -------------------------------------------------------------------------

template<>
void Option<StringRef>::parse(StringRef argName, StringRef argValue) {
  _value = argValue;
  _present = true;
}

// -------------------------------------------------------------------------
// OptionGroup
// -------------------------------------------------------------------------

OptionGroup::OptionGroup(StringRef name, StringRef description)
  : _name(name)
  , _description(description)
{
  _next = _groups;
  _groups = this;
}

void OptionGroup::getOptions(SmallVectorImpl<OptionBase *> & out) const {
  for (OptionSet::const_iterator it = _options.begin(), itEnd = _options.end(); it != itEnd; ++it) {
    out.push_back(it->first);
  }
}

// -------------------------------------------------------------------------
// Parser
// -------------------------------------------------------------------------

char ** Parser::parse(ArrayRef<StringRef> groups, iterator first, iterator last) {
  initGroupMap();
  // First find the option groups.
  typedef SmallVector<OptionGroup *, 16> GroupList;
  GroupList optGroups;
  for (ArrayRef<StringRef>::const_iterator
      it = groups.begin(), itEnd = groups.end(); it != itEnd; ++it) {
    OptionGroupMap::const_iterator gi = _groupMap.find_as(*it);
    if (gi == _groupMap.end()) {
      diag::error() << "Option group '" << *it << "' not found!";
      exit(-1);
    }
    optGroups.push_back(gi->first);
  }

  while (first < last) {
    StringRef arg = *first;
    if (arg.startsWith("--")) {
      size_t eq = arg.find('=', 2);
      StringRef argName = arg.substr(2, eq - 2);
      StringRef argValue = arg.substr(eq == StringRef::npos ? eq : eq + 1);
      OptionBase * opt = NULL;
      for (GroupList::const_iterator gi = optGroups.begin(), giEnd = optGroups.end(); gi != giEnd; ++gi) {
        OptionSet::const_iterator oi = (*gi)->_options.find_as(argName);
        if (oi != (*gi)->_options.end()) {
          opt = oi->first;
          break;
        }
      }
      if (opt != NULL) {
        opt->parse(argName, argValue);
      } else {
        diag::error() << "No such option: " << argName;
      }
      ++first;
    } else if (arg.startsWith("-")) {
      arg = arg.substr(1);
      //StringRef::const_iterator it = arg.begin() + 1;
      ++first;
    } else {
      break;
    }
  }

  return first;
}

void showHelp(StringRef groupName) {
  using namespace console;
  OptionGroupMap::const_iterator gi = _groupMap.find_as(groupName);
  if (gi == _groupMap.end()) {
    diag::error() << "No help available for '" << groupName << "'";
    exit(-1);
  }

  OptionGroup * group = gi->first;
  SmallVector<OptionBase *, 32> options;
  group->getOptions(options);
  if (group->description().empty()) {
    out() << group->name() << " options\n";
  } else {
    out() << group->description() << "\n";
  }
  for (SmallVectorImpl<OptionBase *>::const_iterator
      it = options.begin(), itEnd = options.end(); it != itEnd; ++it) {
    OptionBase * opt = *it;
    out() << "  " << opt->name() << "       " << opt->description() << "\n";
  }
}

}
}
