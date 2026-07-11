# Tester Report — CRIT-6 (FGameConfig readlink OOB write)

- **Status:** success
- **Task:** CRIT-6
- **Branch:** coord-phase1-critical-defects-CRIT-6-tester-20260710
- **Test commit:** cec2da37

## Testing Scope

Validate the fix for Critical defect C6 in `FGameConfig::FGameConfig()`
(`src/core/FGameConfig.cpp`). The pre-fix constructor stored `readlink()`'s
return in a `size_t` and unconditionally wrote `buf[size] = 0`, causing an
out-of-bounds write on `readlink()` failure (-1, which wraps to a huge unsigned
value) or when the path length is at/above buffer capacity, in a singleton
constructor with no error path.

The fix captures `readlink()`'s return in a signed `ssize_t`, requests at most
`sizeof(buf) - 1` bytes, computes the terminator index through the new pure
static helper `FGameConfig::computeSafeTerminatorIndex(long, size_t)`, and on
failure (`size <= 0`) leaves `m_basePath`/`m_executablePath` as empty strings.

## Implementation Surface Under Test

- `src/core/FGameConfig.cpp` — constructor + `computeSafeTerminatorIndex()`
- `include/core/FGameConfig.h` — `computeSafeTerminatorIndex()` declaration

## Acceptance Criteria → Coverage

| # | Acceptance Criterion | Verifying Test(s) | Result |
|---|----------------------|-------------------|--------|
| 1 | Normal executable path: `m_basePath` / `m_executablePath` resolve as before | `testConstructor` (existing, real `/proc/self/exe`), `testComputeSafeTerminatorIndexWithinBoundsReturnsRawResult` (in-bounds result returned unchanged) | PASS |
| 2 | `readlink` failure (-1) causes no OOB write and leaves config in a safe, defined state | `testComputeSafeTerminatorIndexReturnsZeroOnNegativeReadlinkResult`, `testComputeSafeTerminatorIndexReturnsZeroOnZeroReadlinkResult`, `testComputeSafeTerminatorIndexNeverWritesPastBufferAcrossBoundaryValues` | PASS |
| 3 | A path at/above buffer capacity writes no byte past `buf` | `testComputeSafeTerminatorIndexClampsWhenResultEqualsCapacity`, `testComputeSafeTerminatorIndexClampsWhenResultExceedsCapacity`, `testComputeSafeTerminatorIndexNeverWritesPastBufferAcrossBoundaryValues` (guarded-buffer end-to-end write) | PASS |
| 4 | Repo builds; existing suites still pass | Top-level `make` (exit 0), `cd tests && make` (exit 0), `./SSWTests` -> `OK (205 tests)` | PASS |

## New Behavioral Tests (tests/core/FGameConfigTest.cpp/.h)

1. `testComputeSafeTerminatorIndexWithinBoundsReturnsRawResult` — in-bounds
   results (5, 998 with capacity 1000) returned unchanged; normal-path
   arithmetic unaffected by the helper.
2. `testComputeSafeTerminatorIndexClampsWhenResultEqualsCapacity` — result == 1000
   with capacity 1000 clamps to 999 (would previously write `buf[1000]`).
3. `testComputeSafeTerminatorIndexClampsWhenResultExceedsCapacity` — result 5000
   with capacity 1000 clamps to 999.
4. `testComputeSafeTerminatorIndexReturnsZeroOnNegativeReadlinkResult` — -1 (the
   real `readlink` failure signal) returns 0, not `(size_t)-1`.
5. `testComputeSafeTerminatorIndexReturnsZeroOnZeroReadlinkResult` — 0 returns 0.
6. `testComputeSafeTerminatorIndexReturnsZeroWhenBufferCapacityZero` — degenerate
   zero-capacity buffer returns 0.
7. `testComputeSafeTerminatorIndexNeverWritesPastBufferAcrossBoundaryValues` —
   end-to-end guarded-buffer test: reproduces the constructor's exact write
   pattern `buf[computeSafeTerminatorIndex(...)] = 0` against a struct with
   0xAA guard bytes before and after an 8-byte buffer, for readlink results
   { -1, 0, 8, 9, 100 }, asserting the index is always `< capacity` and that no
   guard byte is corrupted.

These are behavioral tests: they construct real state and assert on observed
runtime results (return values and actual memory writes), not source text.

## Negative / Abuse-Case Coverage

The defect is memory-safety on externally-influenced input (the executable path
length and `readlink` failure). Negative paths are covered directly: `readlink`
failure (-1), zero-length result, at-capacity, and above-capacity boundaries,
plus a guarded-buffer write test that would flag any byte written outside the
buffer.

## Regression Evidence (fails-against-unfixed)

A standalone driver reproducing the pre-fix logic
`buf[static_cast<size_t>(readlinkResult)] = 0` (no clamp, no failure guard) was
compiled and run. For capacity 8 it produced out-of-bounds indices for
readlink results -1 (18446744073709551615), 8, 9, and 100 — exactly the cases
the new `CPPUNIT_ASSERT(terminatorIndex < bufferCapacity)` and guard-byte
assertions reject. With the fixed helper all cases clamp in-bounds and pass.
This confirms the new coverage fails against the unfixed code and passes after
the change.

## Commands Run

- `make -C src/core clean` / `make -C src/core` — exit 0, no warnings
- `make` (repo root, top-level build) — exit 0
- `make -C tests` — exit 0 (SSWTests linked)
- `./SSWTests` (run from `tests/`) — `OK (205 tests)`; baseline before new tests
  was 198, confirming all 7 new cases link and execute (+7)

Note: `./SSWTests` must be run with the working directory set to `tests/`;
several pre-existing `FGameHeaderDependencyTest` cases resolve source files
relative to cwd and report path-lookup failures when run from another directory.
This is unrelated to CRIT-6.

## Cleanup

Temporary scratch files (prompt copy, regression demo, execution logs) were
kept outside the repository under the session scratchpad; the worktree contains
only the committed test sources and artifacts.
