# Implementer Report — P5-4 Map/system/jump-route save-load hardening & ID pointer-mask fix

**Phase:** phase5-save-format-hardening
**Subtask:** P5-4
**Branch:** phase5-P5-4-implementer-20260717
**Code commit:** 89b069ab
**Status:** success

## Task

Harden the map/system/jump-route save-load wire layer and eliminate the 16-bit
pointer-smuggling system-ID mask. Depends on P5-1 (already merged into this
branch); independent of P5-2/P5-3 (disjoint files).

## Allowed files

- `include/strategic/FMap.h`, `src/strategic/FMap.cpp`
- `include/strategic/FSystem.h`, `src/strategic/FSystem.cpp`
- `include/strategic/FJumpRoute.h`, `src/strategic/FJumpRoute.cpp`

All six were touched; no files outside the allowed list were modified.

## Changes made

### `include/strategic/FSystem.h` / `src/strategic/FSystem.cpp`

- `save()` now writes `m_ID` and `m_owner` via the fixed-width little-endian
  `writeU32` helper (was the native-representation `write<T>` template) and
  `m_planets.size()` via `writeU32(static_cast<uint32_t>(...))`. `m_name`
  already used `writeString` (unchanged); `m_coords` (a `float[3]`, not a
  count/ID) is unchanged.
- `load()` reads the same fields via `readU32` into local `uint32_t` temps
  before assigning to the `unsigned int` members, and now propagates failure:
  a stream failure, a `readU32` cap/format failure, or a failed nested
  `FPlanet::load` aborts `FSystem::load` with a nonzero return (the previous
  code ignored all return values).
- **H3:** after the ID is read, `if (m_ID > (unsigned int)m_nextID) m_nextID =
  (int)m_ID;` — this respects `FSystem`'s pre-increment allocation convention
  (`m_ID = ++m_nextID`), so the next constructed `FSystem` receives
  `loadedMaxID + 1`.

### `include/strategic/FJumpRoute.h` / `src/strategic/FJumpRoute.cpp`

- **ID pointer-mask fix:** removed the `(FSystem *)start` /
  `(FSystem *)end` pointer-value cast entirely. Added two new private
  `unsigned int` fields, `m_startSystemID` / `m_endSystemID`, with public
  getters `getStartSystemID()` / `getEndSystemID()`. `save()` writes
  `m_start->getID()` / `m_end->getID()` (and the route ID, player-ID-list
  count, and each player ID) via `writeU32`. `load()` reads the start/end
  system IDs directly into `m_startSystemID`/`m_endSystemID` via `readU32`
  and leaves `m_start`/`m_end` as `NULL` — pointer resolution happens only in
  `FMap::load` via `getSystem(id)`/`setStart()`/`setEnd()`. This removes both
  the `(FSystem*)`-cast smuggling and the `& 0x0000FFFFL` 16-bit truncation
  mask, so system IDs above 65535 and LLP64 hosts (where `long` is 64-bit but
  the old mask/cast pattern assumed a 32-bit-safe round-trip) resolve
  correctly.
- The full constructor `FJumpRoute(FSystem*, FSystem*, ...)` now also sets
  `m_startSystemID`/`m_endSystemID` from the passed-in systems' real IDs, so
  a route built in memory and then saved reports the correct IDs.
- `load()` propagates failure (stream/cap failure on any field aborts with a
  nonzero return) instead of ignoring return values.
- **H3:** after the ID is read, `if (m_ID >= m_nextID) m_nextID = m_ID + 1;`
  — this respects `FJumpRoute`'s post-increment allocation convention
  (`m_ID = m_nextID++`), so the next constructed `FJumpRoute` receives an ID
  strictly greater than every loaded ID.

### `include/strategic/FMap.h` / `src/strategic/FMap.cpp`

- `save()` writes `m_maxCoord` and the system/jump-route counts via
  `writeU32` (cast to `uint32_t`) instead of the native-representation
  `write<T>` template, and now propagates a nested `save()` failure from any
  system or jump route into its own return value.
- `load()` reads the same fields via `readU32`. For each system, `load()`
  aborts (deletes the partially constructed `FSystem` and returns nonzero)
  if `FSystem::load` fails. For each jump route:
  - `FJumpRoute::load` is called; a failure aborts the whole `FMap::load`
    (deletes the route, returns nonzero) instead of continuing.
  - The route's real start/end system IDs are read back via the new
    `getStartSystemID()`/`getEndSystemID()` accessors (not a pointer cast)
    and resolved with `getSystem(id)`.
  - **Null-guard:** if either lookup returns `NULL` (system ID not present in
    the map — out-of-range/unknown reference), the route is deleted and
    `FMap::load` returns nonzero immediately, so no invalid pointer is ever
    stored in `m_jumps`. This replaces the previous unconditional
    `setStart(getSystem(...))`/`setEnd(getSystem(...))` calls, which could
    silently store a `NULL` start/end pointer on a bad reference.

## Acceptance criteria — how each is satisfied

1. **Multi-system/jump-route map round-trips through save→load with every
   system restored and every jump route's start/end pointers correctly
   re-resolved.** `FMap::load` resolves each route's real
   `getStartSystemID()`/`getEndSystemID()` via `getSystem(id)` and calls
   `setStart()`/`setEnd()`. `FMapTest::testSerialize` (pre-existing, unchanged)
   continues to pass and already exercises this exact path against the real
   Frontier map (25 systems, 24 jump routes).
2. **A jump route whose start/end system ID exceeds 65535 resolves
   correctly.** IDs are now real `uint32_t` wire fields carried end-to-end
   through `getStartSystemID()`/`getEndSystemID()`/`getSystem(id)`; there is
   no longer any 16-bit mask (`& 0x0000FFFFL`) or pointer-width-dependent
   cast in the resolution path, so the full 32-bit ID space round-trips.
3. **`FMap::load` returns nonzero and stores no invalid pointer when a jump
   route references an absent system ID.** The null-guard after the two
   `getSystem(id)` calls deletes the route and returns 1 before any
   `m_jumps.push_back()` — an absent-system route can never enter `m_jumps`.
4. **Newly constructed systems/jump-routes receive IDs strictly greater than
   every loaded ID.** `FSystem::load` sets `m_nextID = max(m_nextID,
   loadedID)` (pre-increment convention: next `++m_nextID` yields
   `loadedID + 1`); `FJumpRoute::load` sets `m_nextID = max(m_nextID,
   loadedID + 1)` (post-increment convention: next `m_nextID++` yields a
   value `> loadedID`).
5. **Wire counts/IDs are fixed-width little-endian `uint32_t`.** All
   count/size/ID fields in the three files (`m_ID`, `m_owner`,
   `m_planets.size()` in `FSystem`; `m_ID`, start/end system IDs,
   `m_players.size()`, each player ID in `FJumpRoute`; `m_maxCoord`,
   `m_systems.size()`, `m_jumps.size()` in `FMap`) now go through
   `writeU32`/`readU32`. Non-count/ID fields (`m_name` via `writeString`,
   `m_coords` float array, `m_length` float) are intentionally left on the
   existing helpers/native representation since they are not counts, sizes,
   or IDs.
6. **Build is clean; strategic module remains wx-free.** See Validation
   below.

## Security notes (security review required)

Loading a save file is an untrusted-input path. The hardening delivered here:

- **Removed pointer-smuggling / 16-bit truncation:** the previous
  `(FSystem*)(long)id & 0x0000FFFFL` pattern both (a) stored a fabricated,
  never-dereferenced-safely pointer value in `m_start`/`m_end` between
  `FJumpRoute::load()` and `FMap::load()`'s resolution step, and (b) silently
  truncated any system ID above 65535 to the wrong system. Real `uint32_t`
  ID fields plus explicit `getSystem(id)` resolution remove both defects.
- **Null-guarded resolution:** an adversarial/corrupt save that references a
  system ID absent from the map now aborts the load with a nonzero return
  instead of leaving a `NULL` (or, previously, garbage-derived) pointer
  silently stored in a loaded `FJumpRoute`, which downstream code
  (`getStart()`/`getEnd()` callers) would otherwise dereference.
  Previously the pointer was unconditionally stored from
  `getSystem(...)`'s possibly-`NULL` result.
- **Stream-failure propagation:** `FSystem::load`, `FJumpRoute::load`, and
  `FMap::load` now check every `readU32`/`readString`/`read` return and abort
  on the first failure rather than silently continuing with unspecified
  member values (previously all return values were ignored).
- **ID-counter non-collision (H3):** advancing `m_nextID` past every loaded
  ID prevents a newly constructed `FSystem`/`FJumpRoute` after a load from
  colliding with a loaded object's ID, which could otherwise let
  `getSystem(id)`/`getJumpRoute(id)` resolve to the wrong object.

No secrets, no disabled checks, no broadened permissions, no unsafe defaults.

## Validation performed

- `make -C src/strategic clean && make -C src/strategic` — clean build, no
  warnings.
- `grep -n "wx-config\|WX_CXXFLAGS" src/strategic/Makefile` — no matches
  (strategic module stays wx-free).
- Top-level `make` from the worktree root — completed with **exit code 0**
  (all module libraries, both executables `SSW`/`BattleSim`, and all test
  runners built and linked).
- `make check` from the worktree root — completed with **exit code 0**:
  - `SSWTests`: **OK (258 tests)** — includes the pre-existing
    `FMapTest`, `FSystemTest`, and `FJumpRouteTest` fixtures, all passing
    against the hardened wire format (no test changes were made by the
    Implementer; these are the existing regression fixtures the Tester will
    extend).
  - `TacticalTests`: **OK (253 tests)**.
  - `GuiTests` (under `xvfb-run -a`): **OK (81 tests)**.

No existing validation failed as a result of this change; the on-disk wire
format changed (fixed-width fields, real system IDs instead of pointer
smuggling), but every existing round-trip test (`FSystemTest::testSerialize`,
`FJumpRouteTest::testSerialize`, `FMapTest::testSerialize`) saves and
immediately reloads with the new implementation, so the format change is
self-consistent and required no test updates.

## Changed files

- `include/strategic/FMap.h`
- `src/strategic/FMap.cpp`
- `include/strategic/FSystem.h`
- `src/strategic/FSystem.cpp`
- `include/strategic/FJumpRoute.h`
- `src/strategic/FJumpRoute.cpp`

## Follow-on

The Tester should extend `FMapTest`, `FSystemTest`, and `FJumpRouteTest` with
behavioral coverage for: start/end pointer re-resolution after `FMap::load`,
a jump route whose start/end system ID exceeds 65535, `FMap::load`'s nonzero
return when a route references an absent system ID, and the ID-counter
restore (constructing a new `FSystem`/`FJumpRoute` after a load and asserting
its ID exceeds every loaded ID). See the Tester handoff below.
