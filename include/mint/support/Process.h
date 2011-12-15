/* ================================================================ *
   Mint
 * ================================================================ */

#ifndef MINT_SUPPORT_PROCESS_H
#define MINT_SUPPORT_PROCESS_H

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#include "mint/collections/SmallString.h"
#endif

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

namespace mint {

class ProcessListener;

/** -------------------------------------------------------------------------
    A class which can be used to spawn processes that run commands.
 */
class Process {
public:

  /// Constructor
  Process(ProcessListener * listener);

  /// Run a command as a subprocess.
  bool begin(StringRef programName, ArrayRef<StringRef> args, StringRef workingDir);

  //void isFinished() const;

  /// Wait for a process to exit.
  static bool waitForProcessExit();

private:
  static Process * _processList;

  ProcessListener * _listener;
  Process * _next;
  #if HAVE_UNISTD_H
    pid_t _pid;
  #endif
};

/** -------------------------------------------------------------------------
    Interface used to listen for processes being finished.
 */
class ProcessListener {
public:
  virtual void processFinished(Process & process, bool success) = 0;
};

} // namespace

#endif // MINT_SUPPORT_PROCESS_H
