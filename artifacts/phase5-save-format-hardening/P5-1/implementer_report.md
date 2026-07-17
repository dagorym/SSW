# Implementer Report — P5-1 Core wire-format primitives & format constants

**Phase:** phase5-save-format-hardening
**Subtask:** P5-1
**Branch:** phase5-P5-1-implementer-20260717
**Code commit:** 456c7929
**Status:** success

## Task

Rebuild the shared serialization substrate in `FPObject` and add the
save-format constants. This is the foundation every serializable class in the
phase depends on (P5-2 … P5-5 consume the helpers and constants defined here).

## Allowed files

- `include/core/FPObject.h`
- `src/core/FPObject.cpp`
- (optional new) `include/core/FSaveFormat.h`

All three were touched; no files outside the allowed list were modified.

## Changes made

### New: `include/core/FSaveFormat.h` (core-pure, wx-free)

Defines the three named save-format constants in `namespace Frontier`:

- `kSaveMagic = 0x31575353u` — 32-bit header tag. Its little-endian byte
  emission spells the ASCII bytes `S`,`S`,`W`,`1` in file order.
- `kSaveFormatVersion = 1u` — current format version (uint32_t).
- `kMaxSerializedStringBytes = 65536` — the 64 KB string cap (`size_t`).

The header includes only `<cstddef>`/`<cstdint>`; no wx includes.

### `include/core/FPObject.h`

- Added `#include "core/FSaveFormat.h"` and `<cstdint>`.
- Declared the new fixed-width helpers:
  - `const int writeU32(std::ostream &os, uint32_t v) const;`
  - `int readU32(std::istream &is, uint32_t &v);`
- Changed the inline `write<T>` template from `const int` returning a hardcoded
  `0` to returning `os.fail() ? 1 : 0` (stream-state check after the write).
  (The redundant leading `const` on the return of a by-value `int` was dropped
  so the template can return the computed status cleanly; behavior for callers
  is unchanged on success — still `0`.)
- Changed the inline `read<T>` template to return `is.fail() ? 1 : 0`.
- Updated the `writeString` / `readString` Doxygen contracts to document the
  fixed-width length encoding, the embedded-NUL preservation, the 64 KB cap,
  and the error-code return contract.
- Updated `@author` (added `Claude Sonnet 5 (medium)`) and
  `@date Last Modified: Jul 17, 2026` on every changed function.

### `src/core/FPObject.cpp`

- Implemented `writeU32`: emits exactly 4 bytes, least-significant first, built
  with byte shift/mask (`(v >> 8) & 0xFF`, etc.), not a raw struct copy; returns
  `os.fail() ? 1 : 0`.
- Implemented `readU32`: reads exactly 4 bytes, returns nonzero on stream
  failure *before* touching the out-parameter, then reassembles the value with
  explicit little-endian shift/or. `v` is left unspecified on the failure path.
- Rewrote `writeString`:
  - Oversize guard: returns nonzero if `s.size() > kMaxSerializedStringBytes`
    (before writing anything).
  - Writes the length via `writeU32(os, (uint32_t)s.size())`.
  - Writes the raw bytes with `os.write(s.data(), s.size())` (no `c_str()`), so
    embedded `'\0'` bytes are preserved instead of truncating the payload.
  - Returns `os.fail() ? 1 : 0`.
- Rewrote `readString`:
  - Reads the length via `readU32`; returns nonzero on stream failure.
  - **Validates the length against `kMaxSerializedStringBytes` BEFORE allocating
    anything** — a cap violation returns nonzero with no buffer allocated.
  - Allocates and reads only for `size > 0`, checks stream state after the read,
    and assigns via `s.assign(begin, end)` (length-preserving, NUL-safe).
  - Clears `s` for the zero-length case.

## Acceptance criteria — how each is satisfied

1. **readString rejects >64 KB / failed stream with no allocation, and embedded
   NUL round-trips.** The cap check runs immediately after `readU32` and returns
   `1` before the `std::vector<char>` is constructed. On the write side,
   `os.write(s.data(), s.size())` plus the length prefix preserves the exact
   byte count including interior `'\0'`; on read, `s.assign(begin,end)` restores
   the exact length and bytes.
2. **Lossless little-endian round-trip of counts/lengths.** `writeU32`/`readU32`
   are exact inverses using explicit byte shift/mask; `writeString`/`readString`
   use them for the length prefix. Emitted bytes are inspectable LE order.
3. **write/read/writeString/readString return nonzero on a failed stream, 0 on
   success.** All four now check `fail()` (directly, or via the helpers they
   call) and propagate a nonzero code.
4. **64 KB cap and magic/version are named constants.** `kSaveMagic`,
   `kSaveFormatVersion`, `kMaxSerializedStringBytes` in `FSaveFormat.h`; no
   inline literals in the string/format logic.
5. **Core stays wx-free; core builds clean.** `src/core/Makefile` has zero
   `wx-config`/`WX_CXXFLAGS` references (verified); the core module and the
   top-level `make` build clean.

## Security notes (security review required)

Loading a save file is an untrusted-input path. The two hardening properties
delivered here are:

- **Unbounded-allocation DoS guard:** `readString` validates the decoded length
  against the 64 KB cap *before* allocating the buffer, so a corrupt/adversarial
  length field cannot force a large allocation.
- **Stream-state propagation:** every read/write helper now reports failure via
  a nonzero return, giving callers (P5-2 … P5-5) the hook to abort a load on a
  truncated or garbage stream instead of silently continuing.
- **Embedded-NUL wire-integrity fix:** the previous `os << s.c_str()` truncated
  string payloads at the first NUL, a latent data-corruption bug; the new
  length-prefixed `os.write(s.data(), s.size())` preserves the exact payload.

No secrets, no disabled checks, no broadened permissions, no unsafe defaults.

## Validation performed

- `make -C src/core clean` then `make -C src/core` — clean build, no warnings.
- Confirmed `src/core/Makefile` contains no `wx-config` / `WX_CXXFLAGS` usage.
- Top-level `make` from the worktree root — completed with **exit code 0**
  (all module libraries, both executables, and all test runners linked).

## Changed files

- `include/core/FSaveFormat.h` (new)
- `include/core/FPObject.h`
- `src/core/FPObject.cpp`

## Follow-on

Downstream subtasks (P5-2 vehicle/weapon/defense, P5-3 player/fleet, P5-4
map/system/jump-route, P5-5 header/orchestration) consume `writeU32`/`readU32`,
the error-code contract, and the `FSaveFormat.h` constants. Behavioral test
coverage for P5-1 is authored by the Tester (see tester handoff).
