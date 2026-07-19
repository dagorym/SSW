# Save-Load Robustness Follow-Ups (Round 2) Plan (FF2-1, FF2-2, FF2-3)

**Planner:** claude-opus-4-8[1m] (planner role)
**Date:** Jul 19, 2026
**Source:** `artifacts/save-load-robustness-followups/reviewer_report.md` — the final
Reviewer PASS of the FF-1/FF-2 follow-ups surfaced four out-of-scope, pre-existing
trust-boundary residuals as Planner-ready follow-up feature requests: FR-A, FR-B,
FR-C, FR-D.
**Coordination base branch:** `phase5` (current non-master branch; it carries the
merged FF-1/FF-2 work these follow-ups build on). Use a dedicated per-plan
coordination branch off `phase5` if preferred; do not base on `master`/`main`.
**Status:** FINAL — all three material design decisions (D1, D2, D3 in Section 2)
were resolved with the user and are recorded as RESOLVED below.

---

## 1. Feature restatement (engineering terms)

All four requests harden the SAME untrusted-save-input trust boundary already
addressed by FF-1/FF-2: loading a corrupt/malicious on-disk save must never crash
the process and must never commit a half-built game as the live singleton. Each
item is a distinct, currently-open crash or silent-success path that survives the
FF-1/FF-2 work. Every item is security-sensitive (untrusted-input parsing).

1. **FR-A (HIGH) — Validate a fleet's `m_destination` (system) reference at load.**
   `FFleet::load()` reads `m_destination` (`readU32`) but never validates it. FF-1's
   load-time validation loop in `FGame::load()` validates a fleet's location (system)
   ID and jump-route ID, but **not** its destination. A crafted in-transit save with
   an out-of-range non-sentinel `m_destination` passes load and later reaches an
   unguarded `m_universe->getSystem(fleets[i]->getDestination())->addFleet(f)` NULL
   dereference in `FGame::moveFleets()` (`src/strategic/FGame.cpp:591`) on the next
   turn advancement — the same crash class FF-1 targets, a different field/path.

2. **FR-B (MEDIUM-HIGH, pairs with FR-A) — Reject the illegal in-transit +
   location-0 fleet state at load.** FF-1 (correctly) exempts `m_location == 0` as
   the "not yet in a system" sentinel. But a crafted save can set `m_inTransit = true`
   with `m_location = 0`, which passes FF-1's exemption and then reaches an unguarded
   `m_universe->getSystem(fleets[i]->getLocation())->removeFleet(...)` NULL
   dereference in `FGame::moveFleets()` (`src/strategic/FGame.cpp:576` on a failed
   jump, and `:590` on a completed jump). `getSystem(0)` is always NULL because
   system IDs start at 1. An in-transit fleet must have a real origin system, so
   `inTransit && location == 0` is an illegal state that should be rejected at load.

3. **FR-C (HIGH) — Harden `FPlanet::load()`.** `src/strategic/FPlanet.cpp:55-67`
   (a) calls `m_station = createShip(type)` and then `m_station->load(is)` with **no
   NULL check** — an unrecognized station type on a corrupt save leaves `m_station`
   NULL and triggers a NULL virtual-call crash; and (b) **unconditionally returns 0**,
   which neutralizes `FSystem::load()`'s existing `if (p->load(is) != 0)` abort check
   (`src/strategic/FSystem.cpp:105`), so this entire depth sits outside the
   aggregate-abort guarantee. More severe than the silent-continue class FF-2 fixed
   (unguarded NULL deref + guaranteed-0 return vs. a silent success).

4. **FR-D (MEDIUM) — Extend the aggregate-abort guarantee to the container-level
   scalar reads in `FPlayer::load()` and `FFleet::load()`.** FF-2 hardened
   `FVehicle::load()`'s own reads, but `FPlayer::load()` (`m_ID`, `m_name`,
   `m_iconName`, and the `uSize`/`fSize`/`dSize` counts) and `FFleet::load()` (`m_ID`,
   `m_name`, `m_owner`, `m_location`, `m_inTransit`, `m_destination`, transit/pos
   scalars, jump-route ID, `sCount`) still discard every scalar read return. A stream
   truncated strictly inside a fleet's or player's own scalar region (not inside a
   vehicle record) is the same silent-success hole FF-2 closed one level down; closing
   it completes the container-scalar depth of the guarantee.

---

## 2. Resolved design decisions

All three material design decisions below were presented to the user and resolved
one at a time; each is user-confirmed (each matched the planner's recommendation).

- **[D1 — FR-B fix placement] RESOLVED (user-confirmed): reject at load
  (root-cause).** The illegal `inTransit && location == 0` state is rejected in
  `FGame::load()`'s existing post-player-load fleet-validation loop (alongside FR-A),
  aborting via the existing `reportLoadError(...)`/no-live-singleton path — **not** by
  adding NULL guards to the `moveFleets()` `getSystem()` sinks. Chosen over
  "null-guard the `moveFleets()` sinks" because it is consistent with the established
  FF-1/FF-2 reject-corrupt-input-at-load root-cause pattern and makes the crash
  impossible rather than merely survivable, reusing the single validation seam already
  present in `FGame::load()`. `moveFleets()` is therefore not modified by this plan.

- **[D2 — FR-A/FR-B subtask granularity] RESOLVED (user-confirmed): one combined
  subtask (FF2-1).** FR-A and FR-B are delivered as a single subtask because both
  extend the identical fleet-validation loop in `FGame::load()`, touch the identical
  source file (`src/strategic/FGame.cpp`), and land their behavioral coverage in the
  identical test file (`tests/strategic/FGameSaveFormatTest.{h,cpp}`). This avoids two
  implementers colliding on the same function/loop and lets one Security review cover
  both fleet-state validations. (Not split into FF2-1a/FF2-1b.)

- **[D3 — FR-D inclusion] RESOLVED (user-confirmed): include as FF2-3.** FR-D
  (MEDIUM) is included in this plan (not deferred) because it completes the
  container-scalar depth of the aggregate-abort guarantee begun by FF-2 one level
  down, at low, self-contained cost, mirroring an already-reviewed pattern.

### Baked-in assumptions (safe bounded inference; not open decisions)

- **[A1]** `FFleet::NO_DESTINATION == FFleet::NO_ROUTE == static_cast<unsigned int>(-1)`
  (confirmed in `include/strategic/FFleet.h:41-42`). FR-A exempts `NO_DESTINATION`.
- **[A2]** FR-A validates `m_destination` whenever it is **not** `NO_DESTINATION`
  (must resolve via `FMap::getSystem(id)`), rather than only in the in-transit case.
  This mirrors FF-1's "validate when not the sentinel" pattern for location and
  jump-route, is strictly safe (a non-transit fleet legitimately carries
  `NO_DESTINATION`), and cannot false-reject a valid save.
- **[A3]** FR-C keeps `FPlanet::load()`'s existing native `read`/`readString` helpers
  (it is not in scope to convert `m_ID`/`stationCount` to `writeU32`/`readU32`); the
  fix is return-checking + the `createShip()` NULL guard only. `createShip(type)` is
  already called in `FPlanet::load()` today, so no new include is required. The
  weapon/defense `m_type` enum-ordinal portability concern remains the separately
  tracked `SF-enum-codes` deferred item and is untouched.

---

## 3. Confirmed repository facts

- **FR-A/FR-B crash sinks (reference only — NOT the fix site):** `FGame::moveFleets()`
  (`src/strategic/FGame.cpp:561-597`) dereferences, inside the `getInTransit()`
  branch: `getSystem(getLocation())->removeFleet(...)` (`:576` failed-jump path,
  `:590` completed-jump path) and `getSystem(getDestination())->addFleet(f)` (`:591`).
  `FMap::getSystem(id)` returns NULL when `id` does not resolve; `getSystem(0)` is
  always NULL (system IDs start at 1). The `NO_ROUTE`/`NO_DESTINATION` guard at `:580`
  protects the `getJumpRoute()` path but not the location/destination `getSystem()`
  dereferences.
- **FR-A/FR-B fix site (confirmed):** `FGame::load()`
  (`src/strategic/FGame.cpp:717-743`) already runs a per-fleet validation loop, after
  each player loads and **before** fleet-to-system wiring (`:745-750`) and
  `m_players.push_back(p)` (`:751`), that validates location (system) ID (exempting
  `0`) and jump-route ID (exempting `NO_ROUTE`), calling `delete p` +
  `reportLoadError(...)` + `return 1` on the first bad reference. FR-A/FR-B extend
  this same loop; the `delete p`-before-`push_back` ordering invariant (flagged by
  the FF-1 Security/Verifier reports) must be preserved.
- **`FFleet` accessors (confirmed):** `getDestination()` (`FFleet.h:326`),
  `getInTransit()` (`:167`), `getLocation()` (`:158`), `getJumpRoute()` (`:318`);
  `m_destination` is read via `readU32` at `FFleet.cpp:164-166`.
- **FR-C current state (confirmed):** `FPlanet::load()` (`src/strategic/FPlanet.cpp:55-67`)
  does `read(is,m_ID)`, `readString(is,m_name)`, `read(is,stationCount)`, and — when
  `stationCount` — `readString(is,type)`, `m_station = createShip(type)`,
  `m_station->load(is)`, all returns discarded, then `return 0` unconditionally.
  `FSystem::load()` (`src/strategic/FSystem.cpp:104-109`) does
  `FPlanet *p = new FPlanet; if (p->load(is) != 0){ ... }` — a real abort check that
  is dead code today because `FPlanet::load()` can never return nonzero.
- **FR-D current state (confirmed):** `FPlayer::load()`
  (`src/strategic/FPlayer.cpp:131-202`) discards `readU32(is,id)`,
  `readString(is,m_name)`, `readString(is,m_iconName)`, and the three
  `readU32(is,uSize/fSize/dSize)` count reads (it already checks each nested
  `createShip()==NULL` and `v->load()!=0`/`f->load()!=0`). `FFleet::load()`
  (`src/strategic/FFleet.cpp:145-208`) discards every scalar read (the `id`, `name`,
  `owner`, `location`, `inTransit`, `destination`, transit/length/speed, jump-route,
  icon, militia, home, holding, pos/dx/dy scalars, and `sCount`) while already
  checking `createShip()==NULL` and nested `v->load()!=0`.
- **Read-helper contract (confirmed, `include/core/FPObject.h`):** `readU32`,
  `readString`, and the `read<T>` template each return `0` on success and nonzero on
  stream failure / cap violation (post-I/O `is.fail()` check). So every read can be
  return-checked directly; FR-C and FR-D are deterministic mirrors of the FF-2 pattern.
- **Existing behavioral-test home + style (confirmed):**
  `tests/strategic/FGameSaveFormatTest.{h,cpp}` uses a `RecordingUI` mock (counts
  `showMessage()`), a `buildValidSaveBytes()` helper that produces a valid full-game
  save, and asserts by byte-patching a precise field to an out-of-range value or
  truncating at a computed offset, then driving `FGame::load()` and asserting
  `rc != 0`, `showMessageCalls == 1`, and no live singleton. FF-1 already added
  out-of-range location-ID / jump-route-ID / sentinel-positive-control tests here —
  the byte-offset computation pattern for a fleet field is established. Unit-level
  homes `tests/strategic/FPlanetTest.{h,cpp}` and `tests/strategic/FFleetTest.{h,cpp}`
  exist for `FPlanet::load`/`FFleet::load` direct-call coverage.

---

## 4. Scope

### IN scope
- **FF2-1 (FR-A + FR-B):** in `FGame::load()`'s existing post-player-load fleet
  validation loop, additionally (a) reject a fleet whose `m_destination` is a
  non-`NO_DESTINATION` value that does not resolve via `FMap::getSystem(id)`, and
  (b) reject a fleet in the illegal `getInTransit() == true && getLocation() == 0`
  state — both via the existing `reportLoadError(...)`/no-live-singleton abort.
- **FF2-2 (FR-C):** harden `FPlanet::load()` to NULL-check `createShip(type)` before
  `m_station->load(is)`, check its own `read`/`readString` and nested
  `m_station->load()` returns, and return nonzero on any failure so
  `FSystem::load()`'s existing check propagates the abort to `FGame::load()`.
- **FF2-3 (FR-D):** add return-checking to the container-level scalar reads in
  `FPlayer::load()` and `FFleet::load()`, aborting nonzero on failure (with the same
  not-yet-owned-object cleanup discipline the nested-load checks already use).
- Doxygen header updates on every touched function/class per repo convention; the
  FF-1 wording tightening called out below (FR-A).

### OUT of scope
- GUI draw-path NULL guards (remain unnecessary under the load-time-reject pattern;
  no gui file is modified).
- `moveFleets()` NULL-guarding and any other `moveFleets()` edit (excluded by resolved
  decision D1 — the fix is at load time; `moveFleets()` is not modified).
- Converting `FPlanet`'s `m_ID`/`stationCount` (or any other field) to fixed-width
  `writeU32`/`readU32`; `SF-enum-codes` and `SF-fhexmap` remain deferred and untouched.
- Any change to the wire format itself, or to the FF-1/FF-2/FR-1/FR-2 code already
  merged.
- `doc/rules/tactical_operations_manual.md` (non-negotiable — never edited).

---

## 5. Subtasks

### FF2-1 — Reject out-of-range fleet destination and illegal in-transit/location-0 state at load (FR-A + FR-B)

**Module:** strategic. **Security review: required** (untrusted-save-input parsing /
trust-boundary hardening).

**Allowed files:**
- `src/strategic/FGame.cpp`
- `include/strategic/FGame.h` (only if a Doxygen update is needed)
- `include/strategic/FFleet.h` (only if a Doxygen update is needed; do not change wire format)

**Task:** Extend the existing per-fleet validation loop in `FGame::load()`
(`src/strategic/FGame.cpp` ~lines 729-743, the loop that already validates location
and jump-route IDs), keeping the current `delete p` → `reportLoadError(...)` →
`return 1` abort pattern and the load-safety ordering invariant (validation runs
before fleet-to-system wiring and before `m_players.push_back(p)`). Add two checks:
1. **FR-A — destination:** read `(*f)->getDestination()`; if it is not
   `FFleet::NO_DESTINATION` and `m_universe->getSystem(destination) == NULL`, abort
   the load (delete the not-yet-owned player, report a distinct destination-specific
   message, return nonzero).
2. **FR-B — illegal in-transit state:** if `(*f)->getInTransit()` is true and
   `(*f)->getLocation() == 0`, abort the load (an in-transit fleet cannot legitimately
   have the "not in a system" location-0 sentinel; `getSystem(0)` is always NULL and
   would NULL-deref in `moveFleets()`). Report a distinct illegal-state message.

Confirm the sentinel value(s) from `FFleet.h`/`StrategicTypes.h` rather than assuming.
Also tighten the FF-1 wording flagged by the FF-1 Reviewer NOTE: the FF-1 in-code
comment / Doxygen / AGENTS.md sentence that opens with "located-object reference IDs"
should now accurately enumerate location, jump-route, **and destination** (and note
the illegal-state rejection). Do not modify any gui file; do not add gui coupling to
the strategic model; do not modify `moveFleets()` (root-cause fix is at load time).

**Acceptance criteria (implementation-outcome):**
- A save whose in-transit fleet carries an out-of-range non-sentinel `m_destination`
  makes `FGame::load()` return nonzero, report via the installed `IStrategicUI`
  exactly once, and commit no live game singleton.
- A save whose fleet has `m_inTransit == true` and `m_location == 0` makes
  `FGame::load()` return nonzero, report exactly once, and commit no live singleton.
- A valid save (destination resolves or is `NO_DESTINATION`; no in-transit fleet has
  location 0; all FF-1 location/jump-route references still resolve) loads unchanged —
  no false rejection, no happy-path behavior change.
- Loading then advancing a turn on a save with the two illegal states above no longer
  reaches a NULL `getSystem(...)` dereference in `moveFleets()` (because such saves
  are now rejected at load).
- No gui files modified; `moveFleets()` unmodified.
- Doxygen updated on the touched function(s); FF-1 "located-object reference IDs"
  wording tightened to include destination.
- `make check` is green (SSWTests / TacticalTests / GuiTests).

**Documentation Impact:** Doxygen on the touched `FGame::load()` (bump `@date Last
Modified`, add implementing model to `@author`). Update the `AGENTS.md` save-format
contributor paragraph so its FF-1 sentence names location, jump-route, **and**
destination validation and records the illegal in-transit/location-0 rejection. No
`doc/deferred-tasks.md` change (neither FR-A nor FR-B was a tracked deferred item;
both are being fixed). No `doc/UsersGuide.md` change beyond existing corrupt-save
File→Open coverage.

**Tester guidance (Tester-owned):** add behavioral coverage in
`tests/strategic/FGameSaveFormatTest.{h,cpp}` mirroring the existing FF-1
byte-patch/truncation style (`RecordingUI` + `buildValidSaveBytes()`): (a) patch an
in-transit fleet's serialized `m_destination` (the `readU32` field following the
`m_inTransit` byte) to an out-of-range value and assert `rc != 0`,
`showMessageCalls == 1`, no live singleton; (b) build/patch a save with a fleet
whose `m_inTransit` byte is set true and whose `m_location` is 0, asserting the same;
and (c) a positive control proving a valid save with a legitimate `NO_DESTINATION`
(and a normal in-transit fleet with a real, resolvable location + destination) still
loads clean (`rc == 0`, `showMessageCalls == 0`). Optionally add a
load-then-`nextTurn()`/`moveFleets()` guard test proving no crash on the rejected
saves. The Tester selects exact field offsets from the `FFleet::save()` field order.

---

### FF2-2 — Harden `FPlanet::load()` against unknown station type and truncation (FR-C)

**Module:** strategic. **Security review: required** (untrusted-save-input parsing /
trust-boundary hardening).

**Allowed files:**
- `src/strategic/FPlanet.cpp`
- `include/strategic/FPlanet.h` (Doxygen update for the new return-contract behavior)

**Task:** In `FPlanet::load()` (`src/strategic/FPlanet.cpp:55-67`): check the return
of each `read(is,m_ID)`, `readString(is,m_name)`, and `read(is,stationCount)`; when
`stationCount` is nonzero, check `readString(is,type)`, then NULL-check
`m_station = createShip(type)` **before** calling `m_station->load(is)` (abort nonzero
on NULL, mirroring the `createShip()==NULL` pattern already used in
`FFleet`/`FPlayer::load()`), then check `m_station->load(is)`'s return. On any failure
return nonzero (instead of the current unconditional `return 0`) so
`FSystem::load()`'s existing `if (p->load(is) != 0)` check fires and the abort
propagates through `FMap::load()` to `FGame::load()`'s aggregate-abort/no-live-
singleton path. Preserve the success path and the existing `stationCount == 0`
(no-station) case unchanged. Do not convert any field to fixed-width `readU32` (out of
scope). Ensure no leak/dangling `m_station`: on a `createShip()==NULL` abort leave
`m_station` NULL; on an `m_station->load()` failure the just-allocated `m_station` is
owned by `this` and freed by `~FPlanet()` when `FSystem::load()` deletes the
not-yet-owned planet — confirm that ownership/cleanup is correct (no double-free).

**Acceptance criteria (implementation-outcome):**
- `FPlanet::load()` returns nonzero on: a failed own read (`m_ID`/`m_name`/
  `stationCount`/`type`), an unknown station type (`createShip()==NULL`, no NULL
  virtual-call), and a failed nested `m_station->load()`.
- A save whose planet carries an unknown/corrupt station type makes `FGame::load()`
  return nonzero, report via the mock `IStrategicUI` exactly once, and commit no live
  singleton (previously: NULL virtual-call crash).
- A save truncated strictly inside a planet's station record makes `FGame::load()`
  return nonzero, report once, and commit no live singleton (previously: silent
  `return 0`).
- A valid save (including planets with and without stations) loads unchanged — no
  false rejection, no happy-path behavior change; no leak, dangling `m_station`, or
  double-free on the abort path.
- `make check` is green.

**Documentation Impact:** Doxygen on `FPlanet::load()` in `include/strategic/FPlanet.h`
(document the new nonzero-on-failure return contract and the `createShip()` NULL
guard; bump `@date Last Modified`, add implementing model to `@author`). One-line
update to the `AGENTS.md` save-format contributor paragraph recording that
`FPlanet::load()` now participates in the aggregate-abort guarantee (rejecting unknown
station types and truncated station records) rather than unconditionally returning 0.
No `doc/deferred-tasks.md` change.

**Tester guidance (Tester-owned):** add behavioral coverage in
`tests/strategic/FGameSaveFormatTest.{h,cpp}` (full-game aggregate-abort assertions)
and/or `tests/strategic/FPlanetTest.{h,cpp}` (unit-level `FPlanet::load` direct
call). Ensure the exercised save contains a planet **with a station** — if
`buildValidSaveBytes()` does not already include one, add a station via
`FPlanet::addStation(...)` before saving. Cover: (a) patch the station's serialized
type tag to an unknown string and assert `FGame::load()` returns nonzero, reports
once, no live singleton (and, at unit level, that `FPlanet::load()` returns nonzero
without a NULL deref); (b) truncate the stream inside the station record and assert
the same aggregate abort; and (c) a positive control proving a valid save with a
station (and one without a station) still loads clean.

---

### FF2-3 — Extend aggregate-abort to container-scalar reads in FPlayer::load()/FFleet::load() (FR-D)

**Module:** strategic. **Security review: required** (untrusted-save-input parsing /
trust-boundary hardening).

**Allowed files:**
- `src/strategic/FPlayer.cpp`
- `include/strategic/FPlayer.h` (only if a Doxygen update is needed)
- `src/strategic/FFleet.cpp`
- `include/strategic/FFleet.h` (only if a Doxygen update is needed)

**Task:** Add return-checking to the container-level scalar reads in both loaders,
mirroring FF-2's per-read pattern in `FVehicle::load()`:
- `FPlayer::load()` (`src/strategic/FPlayer.cpp:131-202`): check `readU32(is,id)`,
  `readString(is,m_name)`, `readString(is,m_iconName)`, and each of
  `readU32(is,uSize/fSize/dSize)`; on any nonzero read return, abort nonzero. Preserve
  the existing per-element `createShip()==NULL` and nested `v->load()`/`f->load()`
  checks and their not-yet-owned-object cleanup unchanged.
- `FFleet::load()` (`src/strategic/FFleet.cpp:145-208`): check the `id`, `name`,
  `owner`, `location`, `inTransit`, `destination`, `transitTime`, `jumpLength`,
  `speed`, `jumpRouteID`, `iconFile`, `isMilitia`, `home`, `isHolding`, `pos[0]`,
  `pos[1]`, `dx`, `dy`, and `sCount` reads; on any nonzero read return, abort nonzero.
  Preserve the H3 next-ID advance, the H4 no-normalization behavior, and the existing
  `createShip()==NULL`/nested `v->load()` checks unchanged.

A nonzero return propagates through the already-FR-1-hardened callers up to
`FGame::load()`'s aggregate-abort path. No object is owned at the point these scalar
reads fail (the loops that allocate ships/fleets run after the scalar reads), so no
special cleanup beyond returning nonzero is required for the scalar-read failures.

**Acceptance criteria (implementation-outcome):**
- Every container-level scalar read in `FPlayer::load()` and `FFleet::load()` has its
  return checked; a nonzero read aborts the respective loader with nonzero.
- A save stream truncated strictly inside a player's own scalar region (before/among
  its `m_ID`/`m_name`/`m_iconName`/count reads) makes `FGame::load()` return nonzero,
  report via the mock `IStrategicUI` once, and commit no live singleton.
- A save stream truncated strictly inside a fleet's own scalar region (before/among
  its `m_ID`…`sCount` reads, not inside a ship record) makes `FGame::load()` return
  nonzero, report once, and commit no live singleton.
- The success path and all existing FF-1/FF-2 nested-load and `createShip()==NULL`
  aborts are unchanged; no leak/dangling/double-free.
- `make check` is green.

**Documentation Impact:** Doxygen on `FPlayer::load()` and `FFleet::load()` (bump
`@date Last Modified`, add implementing model to `@author`; note the container-scalar
return-checking). One-line update to the `AGENTS.md` save-format contributor
paragraph recording that the aggregate-abort guarantee now covers the container-level
scalar reads in `FPlayer::load()`/`FFleet::load()` (completing the depth begun at
`FVehicle::load()` by FF-2). No `doc/deferred-tasks.md` change.

**Tester guidance (Tester-owned):** add behavioral coverage in
`tests/strategic/FGameSaveFormatTest.{h,cpp}` (full-game aggregate-abort) mirroring
the existing FF-2 truncation tests, and/or `tests/strategic/FPlayerTest.{h,cpp}` /
`tests/strategic/FFleetTest.{h,cpp}` for unit-level `load()` direct calls: (a)
truncate the stream inside a player's own scalar region (e.g. within `m_name` or a
count field) and assert `FGame::load()` aborts nonzero, reports once, no live
singleton; (b) truncate inside a fleet's own scalar region (e.g. within `m_name` or
before `sCount`, distinct from the FF-2 "inside a ship record" truncation) and assert
the same. Include a positive control proving a valid multi-player/multi-fleet save
still round-trips clean.

---

## 6. Documentation Impact (overall)

- Doxygen headers per subtask above (all touched `load()` methods; bump `@date Last
  Modified`, add implementing model to `@author`).
- `AGENTS.md`: extend the save-format contributor paragraph (single-source rule;
  concise, non-duplicative) to record (FF2-1) load-time validation of a fleet's
  **destination** reference plus rejection of the illegal in-transit/location-0 state,
  with the FF-1 "located-object reference IDs" wording tightened to enumerate
  location, jump-route, and destination; (FF2-2) `FPlanet::load()` now rejecting
  unknown station types and truncated station records instead of unconditionally
  returning 0; and (FF2-3) the aggregate-abort guarantee now reaching the
  container-level scalar reads in `FPlayer::load()`/`FFleet::load()`. Each subtask
  edits `AGENTS.md`, so serialize the doc edits (see Section 7).
- `doc/deferred-tasks.md`: no change — none of FR-A/FR-B/FR-C/FR-D was a tracked
  deferred item, and all are being fixed rather than deferred. `SF-enum-codes` and
  `SF-fhexmap` remain.
- No user-facing behavior change beyond the already-documented corrupt-save
  File→Open handling.
- `doc/rules/tactical_operations_manual.md`: never edited (non-negotiable).

## 7. Dependency Ordering & parallelization

- **Source files are disjoint** across the three subtasks: FF2-1 = `FGame.cpp`;
  FF2-2 = `FPlanet.cpp`; FF2-3 = `FPlayer.cpp`/`FFleet.cpp`. (FF2-1 and FF2-3 both may
  touch `FFleet.h` only for optional Doxygen — the Coordinator should ensure the FF2-3
  Doxygen edit to `FFleet::load()` and any FF2-1 Doxygen note stay non-conflicting; in
  practice FF2-1 need not touch `FFleet.h` at all.)
- **Two shared surfaces force serialization:** every subtask edits `AGENTS.md`, and
  all three add behavioral coverage to the shared
  `tests/strategic/FGameSaveFormatTest.{h,cpp}` file. To avoid merge contention on the
  doc and the shared test file, **run the three subtasks sequentially**, not in
  parallel. This matches the conservative serialization used by the prior
  save-load-robustness-followups plan (which serialized FF-1 → FF-2 for the shared
  `AGENTS.md` edit).
- **Recommended order: FF2-1 → FF2-2 → FF2-3** (two HIGH crash fixes first — FR-A/FR-B
  then FR-C — then the MEDIUM silent-success completion FR-D). There is **no
  functional dependency** in any direction; the ordering is purely to serialize the
  shared `AGENTS.md`/test-file edits and to land the highest-severity crashes first.

---

## 8. Implementer Prompts (Coordinator-ready)

### FF2-1 prompt

```
Your role is 'implementer'. Your task is as follows:

Reject two currently-unvalidated corrupt-save fleet states at LOAD TIME (reviewer
follow-ups FR-A and FR-B), extending the existing FF-1 fleet-validation loop in
FGame::load(). Fix at load time (root-cause), NOT with gui guards and NOT by
null-guarding FGame::moveFleets(). Do not modify any gui file and do not modify
moveFleets().

Background: FGame::load() (src/strategic/FGame.cpp, ~lines 717-743) already runs a
per-fleet validation loop -- after each FPlayer::load() and BEFORE fleet-to-system
wiring and before m_players.push_back(p) -- that validates a fleet's location
(system) ID (exempting the "not in a system" 0 sentinel) and its jump-route ID
(exempting FFleet::NO_ROUTE), calling `delete p` -> reportLoadError(...) -> `return 1`
on the first bad reference. Two gaps remain that crash FGame::moveFleets() on a
crafted save's next turn advancement:
  FR-A: a fleet's m_destination is never validated; an out-of-range non-sentinel
        destination reaches an unguarded getSystem(getDestination())->addFleet(f)
        NULL deref (FGame.cpp:591).
  FR-B: FF-1 exempts location==0, but m_inTransit==true with m_location==0 is illegal
        and reaches an unguarded getSystem(getLocation())->removeFleet(...) NULL deref
        (FGame.cpp:576 and :590); getSystem(0) is always NULL (system IDs start at 1).

Allowed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h (only if a Doxygen update is needed)
- include/strategic/FFleet.h (only if a Doxygen update is needed; do NOT change the wire format)

Task:
In FGame::load()'s existing per-fleet validation loop, preserving the current
`delete p` -> reportLoadError(...) -> `return 1` abort pattern and the load-safety
ordering invariant (validate before fleet-to-system wiring and before
m_players.push_back(p)), add:
  1. FR-A: read (*f)->getDestination(); if it is not FFleet::NO_DESTINATION and
     m_universe->getSystem(destination) == NULL, abort the load with a distinct
     destination-specific reportLoadError(...) message and return nonzero.
  2. FR-B: if (*f)->getInTransit() is true and (*f)->getLocation() == 0, abort the
     load with a distinct illegal-state reportLoadError(...) message and return
     nonzero.
Confirm FFleet::NO_DESTINATION (== static_cast<unsigned int>(-1)) from FFleet.h rather
than assuming. Also tighten the FF-1 wording flagged by the FF-1 Reviewer NOTE: the
FF-1 in-code comment / Doxygen / AGENTS.md sentence that currently opens with
"located-object reference IDs" must now accurately enumerate location, jump-route,
AND destination and note the illegal in-transit/location-0 rejection. Do not add gui
coupling to the strategic model; do not modify moveFleets() or any gui file.

Acceptance criteria:
- A save whose in-transit fleet carries an out-of-range non-sentinel m_destination
  makes FGame::load() return nonzero, report via the installed IStrategicUI exactly
  once, and commit no live game singleton.
- A save whose fleet has m_inTransit==true and m_location==0 makes FGame::load()
  return nonzero, report exactly once, and commit no live singleton.
- A valid save (destination resolves or is NO_DESTINATION; no in-transit fleet has
  location 0; FF-1 location/jump-route references still resolve) loads unchanged -- no
  false rejection, no happy-path behavior change.
- No gui files modified; moveFleets() unmodified.
- Doxygen updated on the touched function(s) (bump @date Last Modified; add your model
  to @author); FF-1 "located-object reference IDs" wording tightened to include
  destination.
- make check is green (SSWTests / TacticalTests / GuiTests).

Validation guidance:
Build and run `make check` from the repo root. Faster inner loop:
`cd tests/strategic && make && ./StrategicTests`.

Tester handoff:
Behavioral coverage belongs in tests/strategic/FGameSaveFormatTest.{h,cpp} and is the
Tester's responsibility -- do not author it yourself and do not add tester-owned files
to your changes. Leave a note describing how to (a) patch an in-transit fleet's
serialized m_destination (the readU32 field after the m_inTransit byte) to an
out-of-range value, (b) construct/patch a save with m_inTransit true and m_location 0,
and (c) a valid positive control (NO_DESTINATION and a normal resolvable in-transit
fleet), plus the relevant FFleet::save() field order for computing byte offsets.

Artifact guidance:
Write your implementer report and result under
artifacts/save-load-robustness-followups-2/FF2-1/ (repository-root-relative),
following the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs are
missing; the design is fully specified above and requires no clarification turn.

Do not report success unless all required artifacts exist and all changes are committed.
```

### FF2-2 prompt

```
Your role is 'implementer'. Your task is as follows:

Harden FPlanet::load() against a corrupt/malicious save (reviewer follow-up FR-C).
Today FPlanet::load() (src/strategic/FPlanet.cpp:55-67) (a) calls
m_station = createShip(type) then m_station->load(is) with NO NULL check -- an unknown
station type leaves m_station NULL and triggers a NULL virtual-call crash -- and (b)
unconditionally returns 0, which neutralizes FSystem::load()'s existing
`if (p->load(is) != 0)` abort check (src/strategic/FSystem.cpp:105), leaving this
depth entirely outside the aggregate-abort guarantee.

Allowed files:
- src/strategic/FPlanet.cpp
- include/strategic/FPlanet.h (Doxygen update for the new return contract)

Task:
In FPlanet::load(): check the return of read(is,m_ID), readString(is,m_name), and
read(is,stationCount); when stationCount is nonzero, check readString(is,type), then
NULL-check m_station = createShip(type) BEFORE calling m_station->load(is) (abort
nonzero on NULL, mirroring the createShip()==NULL pattern already used in
FFleet/FPlayer::load()), then check m_station->load(is)'s return. On any failure
return nonzero instead of the current unconditional `return 0`, so FSystem::load()'s
existing check fires and the abort propagates through FMap::load() to FGame::load()'s
aggregate-abort/no-live-singleton path. Preserve the success path and the
stationCount==0 (no-station) case unchanged. Do NOT convert any field to fixed-width
readU32 (out of scope; SF-enum-codes remains deferred). Ensure no leak/dangling
m_station and no double-free: on createShip()==NULL leave m_station NULL; on an
m_station->load() failure the just-allocated m_station is owned by `this` and freed by
~FPlanet() when FSystem::load() deletes the not-yet-owned planet -- confirm that is
correct.

Acceptance criteria:
- FPlanet::load() returns nonzero on a failed own read (m_ID/m_name/stationCount/type),
  an unknown station type (createShip()==NULL, no NULL virtual-call), and a failed
  nested m_station->load().
- A save whose planet carries an unknown/corrupt station type makes FGame::load()
  return nonzero, report via the mock IStrategicUI exactly once, and commit no live
  singleton (previously a NULL virtual-call crash).
- A save truncated strictly inside a planet's station record makes FGame::load()
  return nonzero, report once, and commit no live singleton (previously silent
  return 0).
- A valid save (planets with and without stations) loads unchanged -- no false
  rejection, no leak/dangling m_station/double-free on the abort path.
- Doxygen updated on FPlanet::load() (bump @date Last Modified; add your model to
  @author; document the new nonzero-on-failure return contract and createShip() NULL
  guard).
- make check is green (SSWTests / TacticalTests / GuiTests).

Validation guidance:
Build and run `make check` from the repo root. Faster inner loop:
`cd tests/strategic && make && ./StrategicTests`.

Tester handoff:
Behavioral coverage belongs in tests/strategic/FGameSaveFormatTest.{h,cpp} (full-game
aggregate-abort) and/or tests/strategic/FPlanetTest.{h,cpp} (unit-level) and is the
Tester's responsibility -- do not author it yourself and do not add tester-owned files
to your changes. Leave a note covering: ensuring the exercised save contains a planet
WITH a station (add one via FPlanet::addStation(...) before saving if
buildValidSaveBytes() does not already include one); patching the station's serialized
type tag to an unknown string; truncating inside the station record; and a valid
positive control (station present, and a planet with no station).

Artifact guidance:
Write your implementer report and result under
artifacts/save-load-robustness-followups-2/FF2-2/ (repository-root-relative),
following the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs are
missing; the design is fully specified above and requires no clarification turn.

Do not report success unless all required artifacts exist and all changes are committed.
```

### FF2-3 prompt

```
Your role is 'implementer'. Your task is as follows:

Extend the save-load aggregate-abort guarantee to the container-level scalar reads in
FPlayer::load() and FFleet::load() (reviewer follow-up FR-D), mirroring FF-2's
per-read return-checking in FVehicle::load(). Both loaders currently discard every
scalar read return, so a stream truncated strictly inside a player's or a fleet's own
scalar region (not inside a vehicle record) still returns 0 -- the same silent-success
hole FF-2 closed one level down.

Allowed files:
- src/strategic/FPlayer.cpp
- include/strategic/FPlayer.h (only if a Doxygen update is needed)
- src/strategic/FFleet.cpp
- include/strategic/FFleet.h (only if a Doxygen update is needed)

Task:
Add return-checking to the container-level scalar reads (per include/core/FPObject.h,
readU32/readString/read<T> each return 0 on success and nonzero on failure/cap
violation):
  - FPlayer::load() (src/strategic/FPlayer.cpp:131-202): check readU32(is,id),
    readString(is,m_name), readString(is,m_iconName), and each readU32(is,uSize/fSize/
    dSize); abort nonzero on any nonzero read. Preserve the existing per-element
    createShip()==NULL and nested v->load()/f->load() checks and their cleanup
    unchanged.
  - FFleet::load() (src/strategic/FFleet.cpp:145-208): check the id, name, owner,
    location, inTransit, destination, transitTime, jumpLength, speed, jumpRouteID,
    iconFile, isMilitia, home, isHolding, pos[0], pos[1], dx, dy, and sCount reads;
    abort nonzero on any nonzero read. Preserve the H3 next-ID advance, the H4
    no-normalization behavior, and the existing createShip()==NULL / nested v->load()
    checks unchanged.
A nonzero return propagates through the already-FR-1-hardened callers up to
FGame::load()'s aggregate-abort path. No object is owned at the point these scalar
reads fail (allocation loops run after the scalar reads), so returning nonzero is
sufficient; do not introduce spurious cleanup.

Acceptance criteria:
- Every container-level scalar read in FPlayer::load() and FFleet::load() has its
  return checked; a nonzero read aborts the respective loader with nonzero.
- A save stream truncated strictly inside a player's own scalar region makes
  FGame::load() return nonzero, report via the mock IStrategicUI once, and commit no
  live singleton.
- A save stream truncated strictly inside a fleet's own scalar region (not inside a
  ship record) makes FGame::load() return nonzero, report once, and commit no live
  singleton.
- The success path and all existing FF-1/FF-2 nested-load and createShip()==NULL
  aborts are unchanged; no leak/dangling/double-free.
- Doxygen updated on FPlayer::load() and FFleet::load() (bump @date Last Modified; add
  your model to @author).
- make check is green (SSWTests / TacticalTests / GuiTests).

Validation guidance:
Build and run `make check` from the repo root. Faster inner loop:
`cd tests/strategic && make && ./StrategicTests`.

Tester handoff:
Behavioral coverage belongs in tests/strategic/FGameSaveFormatTest.{h,cpp} (full-game
aggregate-abort) and/or tests/strategic/FPlayerTest.{h,cpp} / FFleetTest.{h,cpp}
(unit-level) and is the Tester's responsibility -- do not author it yourself and do
not add tester-owned files to your changes. Leave a note covering: truncating inside a
player's own scalar region (e.g. within m_name or a count field); truncating inside a
fleet's own scalar region (e.g. within m_name or before sCount, distinct from FF-2's
"inside a ship record" truncation); and a valid multi-player/multi-fleet positive
control.

Artifact guidance:
Write your implementer report and result under
artifacts/save-load-robustness-followups-2/FF2-3/ (repository-root-relative),
following the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs are
missing; the design is fully specified above and requires no clarification turn.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## 9. Coordinator notes

- Run each subtask through the default Implementer → Tester → Documenter → Security →
  Verifier chain (all three marked `Security review: required`), then a final Reviewer
  pass after all three merge back.
- Use a dedicated per-plan coordination branch off `phase5` (or reuse `phase5`
  directly, since it is a non-master branch); do not base on `master`/`main`.
- **Sequence FF2-1 → FF2-2 → FF2-3** (shared `AGENTS.md` edit and shared
  `tests/strategic/FGameSaveFormatTest.{h,cpp}` test file; two HIGH crash fixes before
  the MEDIUM completion fix). No functional dependency in any direction.
- Store artifacts under `artifacts/save-load-robustness-followups-2/<FF2-1|FF2-2|FF2-3>/`.
- **Design decisions (Section 2 D1/D2/D3) are RESOLVED and user-confirmed.** D1 =
  reject at load (no `moveFleets()`/gui guards); D2 = FR-A+FR-B combined in FF2-1;
  D3 = FR-D included as FF2-3. No open decisions remain; the plan is ready to launch
  as written.

---

## 10. Output Artifact Path

- **This plan:** `plans/save-load-robustness-followups-2-plan.md` (repository-root-relative).
- **Per-subtask coordination artifacts:**
  `artifacts/save-load-robustness-followups-2/FF2-1/`,
  `artifacts/save-load-robustness-followups-2/FF2-2/`,
  `artifacts/save-load-robustness-followups-2/FF2-3/`, plus the feature-level
  `artifacts/save-load-robustness-followups-2/reviewer_report.md` /
  `reviewer_result.json` written by the final Reviewer pass.
