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

/** -------------------------------------------------------------------------
    A class which can be used to spawn processes that run commands.
 */
class Process {
public:

  /// Constructor
  Process();

  bool begin(StringRef programName, ArrayRef<StringRef> args, StringRef workingDir);
  void isFinished();

private:
  #if HAVE_UNISTD_H
    pid_t _pid;
  #endif
};

} // namespace

#endif // MINT_SUPPORT_PROCESS_H
