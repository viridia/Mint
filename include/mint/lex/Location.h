/* ================================================================ *
   Represents a location in a source file.
 * ================================================================ */

#ifndef MINT_LEX_LOCATION_H
#define MINT_LEX_LOCATION_H

#ifndef MINT_CONFIG_H
#include "mint/config.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace mint {

class TextBuffer;

/** -------------------------------------------------------------------------
    Represents a location of a token in a text buffer or source file.
 */
struct Location {
  TextBuffer * source;      // Pointer to the text buffer
  unsigned begin;           // Starting byte offset of token relative to beginning of file
  unsigned end;             // Ending byte offset of token relative to beginning of file

  Location() : source(NULL), begin(0), end(0) {}
  Location(TextBuffer * buffer, unsigned b, unsigned e) : source(buffer), begin(b), end(e) {}
  Location(const Location & loc) : source(loc.source), begin(loc.begin), end(loc.end) {}

  const Location & operator=(const Location & loc) {
    source = loc.source;
    begin = loc.begin;
    end = loc.end;
    return *this;
  }

  // The union of two locations is a location that spans both, but only if they originate
  // from the same source.
  friend Location operator|(const Location & a, const Location & b) {
    Location result;
    if (a.source == b.source) {
      result.source = a.source;
      result.begin = a.begin < b.begin ? a.begin : b.begin;
      result.end = a.end > b.end ? a.end : b.end;
    } else if (a.source == NULL) {
      result = b;
    } else {
      result = a;
    }

    return result;
  }

  Location operator|=(const Location & a) {
    if (a.source == source) {
      if (a.begin < begin) begin = a.begin;
      if (a.end > end) end = a.end;
    } else if (source == NULL) {
      *this = a;
    }

    return *this;
  }

  bool operator==(const Location & in) const {
    return (source == in.source && begin == in.begin && end == in.end);
  }

  bool operator!=(const Location & in) const {
    return (source != in.source || begin != in.begin || end != in.end);
  }

  void trace() const;
};

}

#endif // MINT_LEX_LOCATION_H
