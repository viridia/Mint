/* ================================================================== *
 * Parser
 * ================================================================== */

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/TextBuffer.h"

#if HAVE_SIGNAL_H
#include <signal.h>
#endif

namespace mint {
namespace diag {

static int messageCountArray[SEVERITY_LEVELS];
static RecoveryState recovery;
static int currentIndentLevel;
static OStream * outputStream;

static const char * severityNames[SEVERITY_LEVELS] = {
  "",
  "",
  "",
  "warning: ",
  "error: ",
  "error: ",
};

static const char * severityMethodNames[SEVERITY_LEVELS] = {
  "debug",
  "status",
  "info",
  "warning",
  "error",
  "fatal",
};

MessageStream::~MessageStream() {
  //flush();
  writeMessage(_severity, _location, str());
}

OStream * setOutputStream(OStream * strm) {
  OStream * result = outputStream;
  outputStream = strm;
  return result;
}

void writeMessage(Severity sev, Location loc, StringRef msg) {
  if (outputStream == NULL) {
    outputStream = &console::err();
  }

  M_ASSERT(!msg.empty()) << "Zero-length diagnostic message";

  switch (sev) {
    case FATAL:
      break;

    case ERROR:
      // If we are recovering from another error, don't show this one.
      if (recovery == CLOSED || recovery == GATED) {
        recovery = CLOSED;
        return;
      }

      recovery = GATED;
      break;

    case WARNING:
    case INFO:
    case STATUS:
      // Allow info messages to follow-up a fatal, but not
      // if the fatal was suppressed from an earlier fatal.
      if (recovery == CLOSED) {
        return;
      }
      break;

    default:
      break;
  }

  messageCountArray[(int)sev] += 1;

  bool colorChanged = false;
  if (outputStream->isTerminal()) {
    if (sev >= ERROR) {
      outputStream->changeColor(OStream::RED, true);
      colorChanged = true;
    } else if (sev == WARNING) {
      outputStream->changeColor(OStream::YELLOW, true);
      colorChanged = true;
    } else if (sev == INFO) {
      outputStream->changeColor(OStream::CYAN, true);
      colorChanged = true;
    } else if (sev == STATUS) {
      outputStream->changeColor(OStream::CYAN, false);
      colorChanged = true;
    }
  }

  unsigned lineStartOffset;
  unsigned lineEndOffset;
  bool showErrorLine = false;
  if (loc.source != NULL && !loc.source->filePath().empty()) {
    M_ASSERT(loc.end >= loc.begin);
    unsigned lineIndex = loc.source->findContainingLine(loc.begin);
    lineStartOffset = loc.source->lines()[lineIndex];
    lineEndOffset = loc.source->lines()[lineIndex + 1];
    unsigned beginCol = loc.begin - lineStartOffset;
    *outputStream << loc.source->filePath() << ":" << lineIndex + 1 << ":" << beginCol + 1 << ": ";
    showErrorLine = true;
  }

  *outputStream << severityNames[(int)sev];
  outputStream->indent(currentIndentLevel * 2);
  *outputStream << msg;
  if (sev != STATUS) {
    *outputStream << "\n";
  }
  //outputStream->flush();

  if (colorChanged) {
    outputStream->resetColor();
  }

  if (showErrorLine) {
    if (colorChanged) {
      outputStream->changeColor(OStream::SAVEDCOLOR, true);
    }

    TextBuffer::const_iterator text = loc.source->begin();
    while (lineEndOffset > lineStartOffset &&
        (text[lineEndOffset - 1] == '\n' || text[lineEndOffset - 1] == '\r')) {
      --lineEndOffset;
    }
    unsigned beginCol = loc.begin - lineStartOffset;
    unsigned endCol = loc.end < lineEndOffset
        ? loc.end - lineStartOffset
        : lineEndOffset - lineStartOffset;
    *outputStream << StringRef(text + lineStartOffset, lineEndOffset - lineStartOffset) << "\n";
    if (colorChanged) {
      outputStream->resetColor();
    }
    outputStream->indent(beginCol);
    while (beginCol < endCol) {
      *outputStream << "^";
      ++beginCol;
    }
    *outputStream << "\n";
  }

  if (sev == FATAL) {
#if HAVE_SIGNAL_H
    raise(SIGINT);
#endif
  } else if (sev == ERROR && outputStream == &console::err()) {
#if HAVE_SIGNAL_H
    raise(SIGINT);
#endif
  }
}

void reset() {
  for (int i = 0; i < SEVERITY_LEVELS; ++i) {
    messageCountArray[i] = 0;
  }
  recovery = OPEN;
}

bool inRecovery() {
  return recovery != OPEN;
}

void recovered() {
  recovery = OPEN;
}

int messageCount(Severity sev) {
  return messageCountArray[(int)sev];
}

const char * severityMethodName(Severity sev) {
  M_ASSERT(unsigned(sev) < SEVERITY_LEVELS);
  return severityMethodNames[sev];
}

void indent() {
  ++currentIndentLevel;
}

void unindent() {
  --currentIndentLevel;
}

int indentLevel() {
  return currentIndentLevel;
}

int setIndentLevel(int level) {
  int result = currentIndentLevel;
  currentIndentLevel = level;
  return result;
}

}}
