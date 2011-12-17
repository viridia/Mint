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
class OStream;

/** -------------------------------------------------------------------------
    Used to buffer the output of a stream from a child process. This makes
    sure that the output isn't garbled by insuring that only complete lines
    get written.
 */
class StreamBuffer {
public:
  typedef int StreamID;

  StreamBuffer(OStream & out);

  /// Set the source I/O handle for the buffered stream. This also sets the source
  /// to be non-blocking.
  void setSource(StreamID source);

  /// Process as many lines as possible, until there is no more data available
  /// on the input source at this time. Returns true if we've reached the end of
  /// the input.
  bool processLines();

  bool flush();

  void close();

private:
  /// Fill the buffer from the source. Return true if the buffer is actually
  /// full, return false if we couldn't fill the buffer for any reason (generally
  /// meaning that there's no more data available at this moment.)
  bool fillBuffer();

  /// Return the offset of the last line break, or end of buffer if there
  /// was no line break.
  unsigned lastLineBreak();

  StreamID _source;
  OStream & _out;
  unsigned _size;
  SmallString<0> _buffer;
  bool _finished;
};

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
  bool cleanup(int status, bool signaled);

  static Process * _processList;

  ProcessListener * _listener;
  Process * _next;

  SmallString<0> _commandBuffer;
  SmallVector<char *, 32> _argv;

  StreamBuffer _stdout;
  StreamBuffer _stderr;

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
