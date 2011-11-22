/* ================================================================== *
 * Hashing - basic hash algorithm (FNV-1).
 * ================================================================== */

#ifndef MINT_SUPPORT_HASHING_H
#define MINT_SUPPORT_HASHING_H

#ifndef MINT_CONFIG_H
#include "mint/config.h"
#endif

namespace mint {

unsigned hash(const char * first, const char * last);

} // namespace mint

#endif // MINT_SUPPORT_HASHING_H
