/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_BUILD_JOBMGR_H
#define MINT_BUILD_JOBMGR_H

#ifndef MINT_BUILD_TARGETMGR_H
#include "mint/build/TargetMgr.h"
#endif

#ifndef MINT_SUPPORT_PROCESS_H
#include "mint/support/Process.h"
#endif

#if HAVE_CPLUS_QUEUE
#include <queue>
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4355)
#undef ERROR
#endif 

namespace mint {

class Object;
class JobMgr;

/** -------------------------------------------------------------------------
    Less-than comparator for targets.
 */
struct TargetLess {
  bool operator()(Target * ls, Target * rs) {
    if ((ls->path() == NULL) != (rs->path() == NULL)) {
      return ls->path() == NULL;
    }
    return ls->sortKey()->value().compare(rs->sortKey()->value()) > 0;
  }
};

/** -------------------------------------------------------------------------
    Priority queue for targets.
 */
typedef std::priority_queue<Target *, SmallVector<Target *, 64>, TargetLess> TargetQueue;

/** -------------------------------------------------------------------------
    Represents a single build job.
 */
class Job : public GC, public ProcessListener {
public:
  enum Status {
    RUNNING = 0,
    FINISHED,
    ERROR
  };

  typedef SmallVector<Node *, 4> Actions;

  /// Constructor
  Job(JobMgr * mgr, Target * target)
    : _mgr(mgr), _target(target), _status(RUNNING), _process(this), _outputDir(NULL)
  {}

  /// Target that this job is building
  Target * target() const { return _target; }

  /// Start this job
  void begin();

  /// True if this job is completed
  bool isFinished() const { return _status != RUNNING; }

  /// Status of this job
  Status status() const { return _status; }

  // Overrides

  void trace() const;
  void processFinished(Process & process, bool success);

private:
  void runNextAction();

  JobMgr * _mgr;
  Target * _target;
  Status _status;
  Process _process;
  Actions _actions;
  String * _outputDir;
};

typedef SmallVector<Job *, 16> JobList;

/** -------------------------------------------------------------------------
    Manages build jobs.
 */
class JobMgr : public GC {
public:
  /// Constructor
  JobMgr(TargetMgr * targets) : _targets(targets), _maxJobCount(4), _error(false) {}

  /// The maximum number of jobs to run simultaneously.
  unsigned maxJobCount() const { return _maxJobCount; }
  void setMaxJobCount(unsigned count) { _maxJobCount = count; }

  /// Return the target manager.
  TargetMgr * targets() const { return _targets; }

  /// Add a target to the list of ready targets. Note that this will add the target in
  /// order.
  void addReady(Target * target);

  /// Add all targets that are currently ready.
  void addAllReady();

  /// Remove the next ready target.
  Target * nextReady();

  /// Return the number of targets in the ready queue.
  size_t readyCount() const { return _ready.size(); }

  /// Used by jobs to signal that they are done.
  void jobFinished(Job * job);

  /// Start running jobs
  void run();

  /// Garbage collection trace function.
  void trace() const;

private:
  TargetMgr * _targets;
  TargetQueue _ready;
  JobList _jobs;
  unsigned _maxJobCount;
  bool _error;
};

}

#endif // MINT_BUILD_TARGETMGR_H
