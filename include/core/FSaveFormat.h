/**
 * @file FSaveFormat.h
 * @brief Save-format constants for the SSW binary serialization wire format
 *
 * This header defines the named constants that make up the versioned,
 * portable save-file contract shared by every @c FPObject::save/load
 * implementation:
 *  - @c kSaveMagic and @c kSaveFormatVersion, the fixed-width header tag and
 *    version number written at the head of a save file so a loader can
 *    reject files that are not in the current, supported format (no legacy
 *    read path).
 *  - @c kMaxSerializedStringBytes, the upper bound on the byte length of any
 *    single serialized string; @c FPObject::readString validates an encoded
 *    length against this cap before allocating any buffer, so a corrupt or
 *    adversarial length value cannot trigger an unbounded allocation.
 *
 * This header is core-pure: it must remain free of wxWidgets includes so the
 * non-GUI core module keeps building without wx-config/WX_CXXFLAGS.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 */

#ifndef _FSAVEFORMAT_H_
#define _FSAVEFORMAT_H_

#include <cstddef>
#include <cstdint>

namespace Frontier
{

/**
 * @brief 32-bit tag written first in every save file
 *
 * Chosen so the little-endian byte sequence written by @c writeU32 spells
 * the ASCII bytes 'S','S','W','1' in file order. A loader that does not see
 * exactly this value as the first four bytes of a stream must reject the
 * file (wrong magic / not an SSW save) rather than attempt to interpret it.
 */
const uint32_t kSaveMagic = 0x31575353u;

/**
 * @brief Current save-format version number
 *
 * Written immediately after @c kSaveMagic. A loader must reject any file
 * whose version does not match a version it knows how to read; there is no
 * legacy-format read path in this format.
 */
const uint32_t kSaveFormatVersion = 1u;

/**
 * @brief Maximum byte length a single serialized string may declare
 *
 * @c FPObject::readString compares the fixed-width length it reads from the
 * stream against this cap and returns a nonzero error code, without
 * allocating any string buffer, when the encoded length exceeds it. This
 * bounds worst-case allocation from a corrupt or adversarial save file.
 */
const size_t kMaxSerializedStringBytes = 65536;

};

#endif //_FSAVEFORMAT_H_
