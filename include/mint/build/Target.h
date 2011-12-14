/* ================================================================== *
 * Node
 * ================================================================== */

#ifndef MINT_BUILD_TARGET_H
#define MINT_BUILD_TARGET_H

#ifndef MINT_SUPPORT_GC_H
#include "mint/support/GC.h"
#endif

#ifndef MINT_GRAPH_STRING_H
#include "mint/graph/String.h"
#endif

#ifndef MINT_SUPPORT_OSTREAM_H
#include "mint/support/OStream.h"
#endif

namespace mint {

class Object;
class Target;
class File;

typedef SmallVector<Target *, 8> TargetList;
typedef SmallVector<File *, 8> FileList;

/** -------------------------------------------------------------------------
    A build target.
 */
class Target : public GC {
public:
  enum TargetState {
    /// Target has not been initialized
    UNINIT,

    /// Target is being initialized (that is, searching for dependencies.)
    INITIALIZING,

    /// Target has been initialized
    INITIALIZED,

    /// Checking the state of this target
    CHECKING_STATE,

    /// Target is waiting on depends
    WAITING,

    /// Target is ready to build.
    READY,

    /// Target is ready to build and in the ready queue.
    READY_IN_QUEUE,

    /// Target is being built.
    BUILDING,

    /// Target is finished building.
    FINISHED,

    /// Target has had a fatal error and cannot continue.
    ERROR,
  };

  /// Constructor
  Target(Object * definition)
    : _state(UNINIT)
    , _definition(definition)
    , _sortKey(NULL)
    , _cycleCheck(false)
  {}

  /// Destructor
  virtual ~Target() {}

  /// Unique path to this target from the base of the project, or empty string if this
  /// is an anonymous target.
  String * path();

  /// Current build state of this target
  TargetState state() const { return _state; }
  void setState(TargetState state) { _state = state; }

  /// Object that defines this target.
  Object * definition() const { return _definition; }

  /// The source location defining this target
  Location location() const;

  /// Add a target to the list of dependencies, and also add the inverse relationship
  void addDependency(Target * dep);

  /// List of all targets that this target depends on.
  const TargetList & depends() const { return _depends; }

  /// List of all targets that depend on this one.
  const TargetList & dependents() const { return _dependents; }

  /// List of all source files for this target.
  const FileList & sources() const { return _sources; }

  /// Add a source file to this target
  void addSource(File * source);

  /// List of all output files produced by this target.
  const FileList & outpus() const { return _outputs; }

  /// Add an output file to this target
  void addOutput(File * output);

  /// Return the string representing the sort key of this target
  String * sortKey();

  /// Check whether this target is up to date
  void checkState();

  /// Garbage collection trace function.
  void trace() const;

  /// Print the name of this target to a stream
  void print(OStream & strm) const;

private:
  TargetState _state;
  Object * _definition;
  String * _sortKey;
  TargetList _depends;
  TargetList _dependents;
  FileList _sources;
  FileList _outputs;
  bool _cycleCheck;
};

/// Stream operator for Targets.
OStream & operator<<(OStream & strm, const Target & target);
OStream & operator<<(OStream & strm, const Target * target);
OStream & operator<<(OStream & strm, Target::TargetState state);

}

#endif // MINT_BUILD_TARGET_H
