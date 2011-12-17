/* ================================================================ *
   Mint
 * ================================================================ */

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OSError.h"
#include "mint/support/OStream.h"
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

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

//extern char **environ;

namespace mint {

// -------------------------------------------------------------------------
// StreamBuffer
// -------------------------------------------------------------------------

StreamBuffer::StreamBuffer(OStream & out) : _source(-1), _out(out), _size(0), _finished(true) {
  _buffer.resize(1024);
}

void StreamBuffer::setSource(StreamID source) {
  _source = source;
  int flags = ::fcntl(_source, F_GETFL, 0);
  if (flags == -1) {
    flags = 0;
  }
  int status = fcntl(_source, F_SETFL, flags | O_NONBLOCK);
  if (status == -1) {
    ::perror("setting stream to non-blocking mode");
    ::exit(-1);
  }
  _finished = false;
}

bool StreamBuffer::fillBuffer() {
  while (!_finished) {
    unsigned avail = _buffer.size() - _size;
    M_ASSERT(_size <= _buffer.size());
    M_ASSERT(avail <= _buffer.size());
    if (avail == 0) {
      return true;
    }
    ssize_t actual = ::read(_source, _buffer.begin() + _size, avail);
    if (actual < 0) {
      if (errno == EWOULDBLOCK) {
        return false;
      }
      ::perror("reading from child processes stream");
      ::abort();
      /// TODO: Set error code.
      _finished = true;
    } else if (actual == 0) {
      // End of stream.
      _finished = true;
    } else {
      _size += actual;
    }
  }
  return false;
}

bool StreamBuffer::processLines() {
  bool more = true;
  while (more && !_finished) {
    more = fillBuffer();
    if (_size == _buffer.size() || _finished) {
      if (_size > 0) {
        unsigned breakPos = _finished ? _size : lastLineBreak();
        _out.write(_buffer.data(), breakPos);
        unsigned remaining = unsigned(_size - breakPos);
        if (remaining > 0) {
          ::memcpy(_buffer.begin(), _buffer.begin() + _size, remaining);
        }
        _size = remaining;
      }
    }
  }
  return _finished;
}

bool StreamBuffer::flush() {
  processLines();
  _out.write(_buffer.data(), _size);
  _size = 0;
  return true;
}

void StreamBuffer::close() {
  flush();
  ::close(_source);
  _finished = true;
  _source = -1;
}

unsigned StreamBuffer::lastLineBreak() {
  unsigned pos = _size;
  while (pos > 0) {
    char ch = _buffer[pos - 1];
    if (ch == '\n' || ch == '\r') {
      break;
    }
    --pos;
  }
  return pos > 0 ? pos : _size;
}

// -------------------------------------------------------------------------
// Process
// -------------------------------------------------------------------------

Process * Process::_processList = NULL;

Process::Process(ProcessListener * listener)
  : _listener(listener)
  , _stdout(console::out())
  , _stderr(console::err())
{
  #if HAVE_UNISTD_H
    _pid = 0;
  #endif
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
  _commandBuffer.clear();
  _commandBuffer.reserve(bufsize);

  // Buffer to hold the character pointers to arguments.
  _argv.clear();
  _argv.reserve(args.size() + 2);

  // Convert program to a null-terminated string.
  const char * program = _commandBuffer.end();
  _argv.push_back(_commandBuffer.end());
  _commandBuffer.append(programName);
  _commandBuffer.push_back('\0');

  // Convert working dir to a null-terminated string.
  const char * wdir = _commandBuffer.end();
  _commandBuffer.append(workingDir);
  _commandBuffer.push_back('\0');

  // Convert arguments to null-terminated strings.
  for (ArrayRef<StringRef>::const_iterator
      it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    _argv.push_back(_commandBuffer.end());
    _commandBuffer.append(*it);
    _commandBuffer.push_back('\0');
  }
  _argv.push_back(NULL);

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
    // Create the pipes
    int fdout[2];
    int fderr[2];
    if (::pipe(fdout) == -1) {
      printPosixFileError("executing", programName, errno);
      return false;
    }

    if (::pipe(fderr) == -1) {
      printPosixFileError("executing", programName, errno);
      return false;
    }

    // Spawn the new process
    pid_t pid = ::fork();
    if (pid == 0) {
      // We're the child

      // Close the read pipe
      ::close(fdout[0]);
      ::close(fderr[0]);

      // Assign stdout to our pipe
      if (fdout[1] != STDOUT_FILENO) {
        if (::dup2(fdout[1], STDOUT_FILENO) != STDOUT_FILENO) {
          printPosixFileError("executing", programName, errno);
          ::_exit(-1);
        }
        ::close(fdout[1]);
      }

      // Assign stderr to our pipe
      if (fderr[1] != STDERR_FILENO) {
        if (::dup2(fderr[1], STDERR_FILENO) != STDERR_FILENO) {
          printPosixFileError("executing", programName, errno);
          ::_exit(-1);
        }
        ::close(fderr[1]);
      }

      // Change to working dir
      if (::chdir(wdir) == -1) {
        printPosixFileError("changing directory to", workingDir, errno);
        ::_exit(-1);
      }

      // TODO: Search for program ourselves and control the environment precisely.
      ::execvp(program, _argv.data());
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
      _stdout.setSource(fdout[0]);
      _stderr.setSource(fderr[0]);
      return true;
    }
  #else
    #error Unimplemented: Process::begin()
  #endif
}

bool Process::cleanup(int status, bool signaled) {
  _pid = 0;
  _stdout.close();
  _stderr.close();
  if (status == 0 && !signaled) {
    if (_listener) {
      _listener->processFinished(*this, true);
      return true;
    }
  } else if (signaled) {
    diag::info() << "Process terminated with signal " << status;
  } else {
    diag::info() << "Process terminated with exit code " << status;
    // VERBOSE output
    if (true) {
      SmallString<0> commandLine("  ");
      for (ArrayRef<char *>::const_iterator
          it = _argv.begin(), itEnd = _argv.end(); it != itEnd; ++it) {
        if (*it != NULL) {
          if (it != _argv.begin()) {
            commandLine.push_back(' ');
          }
          commandLine.append(*it);
        }
      }
      commandLine.push_back('\n');
      diag::status() << commandLine;
    }
  }

  if (_listener) {
    _listener->processFinished(*this, false);
  }
  return false;
}

bool Process::waitForProcessExit() {
  // See if there are even any processes, don't wait otherwise
  bool running = false;
  for (Process * p = _processList; p != NULL; p = p->_next) {
    if (p->_pid != 0) {
      running = true;
    }
  }
  if (!running) {
    diag::info() << "No processes running";
    return true;
  }

  int status;
  pid_t id = ::wait(&status);
  if (id == -1) {
    if (errno == EINTR) {
      diag::warn() << "::wait() interrupted";
      return false;
    }
    if (errno == ECHILD) {
      diag::warn() << "::wait() reported no child processes";
      abort();
      return false;
    }
    perror("wait for child process");
    M_ASSERT(false) << "Unexpected error code from call to wait()";
  }

  for (Process * p = _processList; p != NULL; p = p->_next) {
    if (p->_pid == id) {
      if (WIFEXITED(status)) {
        return p->cleanup(WEXITSTATUS(status), false);
      } else if (WIFSIGNALED(status)) {
        return p->cleanup(WTERMSIG(status), true);
      } else {
        M_ASSERT(false) << "Invalid result from call to wait()";
      }
    }
  }

  return true;
}

}
