/* ================================================================== *
 * Diagnostic functions and macros
 * ================================================================== */

#ifndef MINT_SUPPORT_DIAGNOSTICS_H
#define MINT_SUPPORT_DIAGNOSTICS_H

#ifndef MINT_SUPPORT_OSTREAM_H
#include "mint/support/OStream.h"
#endif

#ifndef MINT_LEX_LOCATION_H
#include "mint/lex/Location.h"
#endif

namespace mint {

/// ---------------------------------------------------------------
/// Various diagnostic functions.
///
/// Typical usage:
///
///   diag::error(location) << "Undefined symbol: " << name;
///
namespace diag {
  enum Severity {
    DEBUG = 0,
    STATUS,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    OFF,

    SEVERITY_LEVELS,
  };

  enum RecoveryState {
    OPEN,         // All messages are allowed
    GATED,        // Follow-up messages allowed, but not new ones.
    CLOSED,       // All error messages supressed
  };

  /// Stream class which prints "Assertion failed" and aborts. */
  class MessageStream : public OStrStream {
  public:
    MessageStream(Severity severity, Location location)
      : _severity(severity)
      , _location(location)
    {}
    MessageStream(const MessageStream & src)
      : _severity(src._severity)
      , _location(src._location)
    {}

    ~MessageStream();

    MessageStream &operator=(const MessageStream & src) {
      _severity = src._severity;
      _location = src._location;
      return *this;
    }

  private:

    Severity _severity;
    Location _location;
  };

  /// Set the output stream that errors are written to.
  OStream * setOutputStream(OStream * strm);

  /// Write a message to the output stream
  void writeMessage(Severity sev, Location loc, StringRef msg);

  /// reset counters for testing
  void reset();

  /// Fatal error.
  inline MessageStream fatal(Location loc = Location()) {
    return MessageStream(FATAL, loc);
  }

  /// Error.
  inline MessageStream error(Location loc = Location()) {
    return MessageStream(ERROR, loc);
  }

  /// Warning message.
  inline MessageStream warn(Location loc = Location()) {
    return MessageStream(WARNING, loc);
  }

  /// Info message.
  inline MessageStream info(Location loc = Location()) {
    return MessageStream(INFO, loc);
  }

  /// Debugging message.
  inline MessageStream debug(Location loc = Location()) {
    return MessageStream(DEBUG, loc);
  }

  /// Return true if we're recovering from another error.
  bool inRecovery();

  /// Let it know that we've recovered, and can start reporting errors again.
  void recovered();

  /// Get message count by severity.
  int messageCount(Severity sev);

  /// Get the count of errors. */
  inline int errorCount() {
    return messageCount(ERROR) + messageCount(FATAL);
  }

  /// Get the count of warnings.
  inline int warningCount() { return messageCount(WARNING); }

  /// Increase the indentation level.
  void indent();

  /// Decrease the indentation level.
  void unindent();

  /// Get the current indent level.
  int indentLevel();

  /// Set the current indentation level.
  int setIndentLevel(int level);

  /// Convenience class that increases indentation level within a scope.
  class AutoIndent {
  public:
    AutoIndent(bool enabled = true) : enabled_(enabled) { if (enabled_) indent(); }
    ~AutoIndent() { if (enabled_) unindent(); }

  private:
    bool enabled_;
  };
}

}

#endif // MINT_SUPPORT_DIAGNOSTICS_H
