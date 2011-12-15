/* ================================================================ *
   Mint
 * ================================================================ */

#include "mint/support/Diagnostics.h"
#include "mint/support/OSError.h"
#include "mint/support/Process.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

//extern char **environ;

namespace mint {

Process * Process::_processList = NULL;

Process::Process(ProcessListener * listener)
  : _listener(listener)
  #if HAVE_UNISTD_H
    , _pid(0)
  #endif
{
  _next = _processList;
  _processList = this;
}

bool Process::begin(StringRef programName, ArrayRef<StringRef> args, StringRef workingDir) {
  unsigned bufsize = programName.size() + workingDir.size() + 2;
  for (ArrayRef<StringRef>::const_iterator
      it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    bufsize += it->size() + 1;
  }

  // Buffer to hold the null-terminated argument strings.
  SmallString<0> commandBuffer;
  commandBuffer.clear();
  commandBuffer.reserve(bufsize);

  // Buffer to hold the character pointers to arguments.
  SmallVector<char *, 32> argv;
  argv.reserve(args.size() + 2);

  // Convert program to a null-terminated string.
  const char * program = commandBuffer.end();
  argv.push_back(commandBuffer.end());
  commandBuffer.append(programName);
  commandBuffer.push_back('\0');

  // Convert working dir to a null-terminated string.
  const char * wdir = commandBuffer.end();
  commandBuffer.append(workingDir);
  commandBuffer.push_back('\0');

  // Convert arguments to null-terminated strings.
  for (ArrayRef<StringRef>::const_iterator
      it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    argv.push_back(commandBuffer.end());
    commandBuffer.append(*it);
    commandBuffer.push_back('\0');
  }
  argv.push_back(NULL);

  // VERBOSE output
  if (false) {
    SmallString<0> commandLine;
    commandLine.append(programName);
    for (ArrayRef<StringRef>::const_iterator
        it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
      commandLine.push_back(' ');
      commandLine.append(*it);
    }
    commandLine.push_back('\n');
    diag::status() << commandLine;
  }

  #if HAVE_UNISTD_H
    // Spawn the new process
    pid_t pid = ::fork();
    if (pid == 0) {
      // We're the child
      if (::chdir(wdir) == -1) {
        printPosixFileError("changing directory to", workingDir, errno);
        ::_exit(-1);
      }
      // TODO: Search for program ourselves and control the environment precisely.
      ::execvp(program, argv.data());
      if (errno == ENOENT) {
        diag::error() << "Program '" << programName << "' not found.";
      } else {
        printPosixFileError("executing", programName, errno);
      }
      ::_exit(-1);
    } else if (pid == -1) {
      printPosixFileError("executing", programName, errno);
      return false;
    } else {
      // We're the parent
      _pid = pid;
      return true;
#if 0
      int status;
      pid_t id = ::wait(&status);
      if (id == -1) {
        printPosixFileError("executing", programName, errno);
      }
      if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code != 0) {
          if (_listener) {
            _listener->processFinished(*this, false);
          } else {
            diag::status() << "Process terminated with exit code " << code << "\n";
            exit(code);
          }
        }
      } else if (WIFSIGNALED(status)) {
        if (_listener) {
           _listener->processFinished(*this, false);
        } else {
          diag::status() << "Process terminated because of a signal\n";
        }
      }
      return true;
#endif
    }
  #else
    #error Unimplemented: Process::begin()
  #endif
}

bool Process::waitForProcessExit() {
  int status;
  pid_t id = ::wait(&status);
  if (id == -1) {
    if (errno == ECHILD) {
      return false;
    }
    //printPosixFileError("executing", programName, errno);
  }

  for (Process * p = _processList; p != NULL; p = p->_next) {
    if (p->_pid == id) {
      if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code == 0) {
          if (p->_listener) {
            p->_listener->processFinished(*p, true);
            return true;
          }
        }
      }

      if (p->_listener) {
        p->_listener->processFinished(*p, false);
      }

//          if (_listener) {
//            _listener->processFinished(*this, false);
//          } else {
//            diag::status() << "Process terminated with exit code " << code << "\n";
//            exit(code);
//          }
//        } else if (_listener) {
//          _listener->processFinished(*this, true);
//        }
//      } else if (WIFSIGNALED(status)) {
//        if (_listener) {
//           _listener->processFinished(*this, false);
//        } else {
//          diag::status() << "Process terminated because of a signal\n";
//        }
//      } else {
//
//      }
    }
  }

  return false;
}

//void Process::isFinished() {
//}

}
