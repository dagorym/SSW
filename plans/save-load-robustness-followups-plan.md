# Save-Load Robustness Follow-Ups Plan (FF-1, FF-2; FF-3 folded)

**Planner:** claude-opus-4-8[1m] (planner role)
**Date:** Jul 19, 2026
**Source:** `artifacts/phase5-save-format-followups/reviewer_report.md` follow-up
requests FF-1, FF-2, FF-3 (final Reviewer PASS of the FR-1/FR-2 follow-ups).
**Coordination base branch:** `phase5` (current non-master branch; it carries the
FR-1/FR-2 work these follow-ups build on). Use a dedicated per-plan coordination
branch off `phase5` if preferred; do not base on `master`.

---

## 1. Feature restatement (engineering terms)

Two save-load robustness follow-ups surfaced by the FR-1/FR-2 Reviewer, both
hardening the untrusted-save-input trust boundary further:

1. **FF-1 — Reject out-of-range located-object IDs at load (root-cause).** A
   corrupt/malicious save can carry a fleet whose location (system) ID or
   jump-route ID does not resolve to any loaded `FSystem`/`FJumpRoute`. Today the
   load accepts it, and the gui draw path later dereferences the NULL result of
   `FMap::getSystem(id)` (in `WXPlayerDisplay::drawFleets()` and the
   `WXMapDisplay` jump-route rendering) and crashes. **Per the resolved design
   decision, this is fixed at load time** (not with gui guards): `FGame::load`
   validates each fleet's located-object reference IDs against the already-loaded
   `FMap` and aborts the load (nonzero return, reported via `IStrategicUI`, no
   live singleton) when any reference is out of range — consistent with the
   save-format-hardening phase's reject-corrupt-input-at-load philosophy. This is
   a **distinct** failure mode from the now-closed `SF-nullfmap-paint-guard` (that
   was a NULL *map*; this is a valid map that cannot resolve an untrusted ID).

2. **FF-2 — Deepen the load aggregate-abort guarantee into `FVehicle::load()`.**
   `FVehicle::load()` (`src/ships/FVehicle.cpp`) discards the return of its own
   scalar reads (`readU32`/`readString`/`read<T>`) and of the nested
   `w->load(is)`/`d->load(is)` weapon/defense loads; it returns nonzero only on
   the `createWeapon()`/`createDefense()==NULL` factory paths. So a stream
   truncated strictly inside a single vehicle's own scalar/weapon/defense region
   still returns 0, and FR-1's per-container aggregate-abort guarantee does not
   reach that depth. Harden `FVehicle::load()` to check its own read returns and
   the nested weapon/defense `load()` returns, returning nonzero (and cleaning up
   the in-progress vehicle state) on failure.

3. **FF-3 (folded into FF-2, not a standalone subtask).** The Reviewer requested
   direct behavioral coverage of the three FR-1 nested-return propagation sites
   the existing single test does not each exercise (the `FFleet` ship-loop and the
   `FPlayer` `m_unattached`/`m_destroyed` loops). This is **routine test-only
   work**, which is not decomposed into its own implementation subtask (planner
   role boundary). It is instead delivered as **expanded Tester coverage within
   FF-2**: FF-2's hardening makes `FVehicle::load()` return nonzero on in-vehicle
   truncation, so placing the truncated vehicle in a fleet, in `m_unattached`, and
   in `m_destroyed` drives all four FR-1 propagation sites end-to-end. FF-2's
   Tester guidance (Section 5) requires exactly this coverage.

---

## 2. Resolved design decisions

- **[D1 — FF-1 fix placement] RESOLVED: load-time validation (root-cause).** The
  out-of-range located-object-ID crash is fixed by validating reference IDs during
  load and aborting the load on failure, **not** by gui-path NULL guards. FF-1
  therefore touches the strategic model only; the gui draw path
  (`WXPlayerDisplay`/`WXMapDisplay`) is intentionally **not** modified — once the
  load rejects out-of-range IDs, the draw path can no longer receive one.
  (User-selected over "gui NULL guards" and "both".)

## 3. Confirmed repository facts

- **FF-1 crash sites (gui, for reference only — NOT modified by this plan):**
  `src/gui/WXPlayerDisplay.cpp` `drawFleets()` dereferences
  `map->getSystem(start)->getCoord(...)` / `map->getSystem(end)->getCoord(...)`
  (lines ~80-81) and `map->getSystem((*itr)->getLocation())->getCoord(...)` (lines
  ~100-101) with no NULL check; `src/gui/WXMapDisplay.cpp` jump-route rendering
  dereferences `(*itr)->getStart()->getCoord(...)` / `getEnd()` (lines ~57-60).
  `FMap::getSystem(id)` and `FMap::getJumpRoute(id)` return a pointer that is NULL
  when the ID does not resolve.
- **FF-1 load path:** `FGame::load()` loads `m_universe` (`FMap`, incl. all
  systems and jump routes) **before** loading players/fleets, so the full `FMap`
  is available to validate fleet reference IDs at load time. `FJumpRoute`
  start/end **system** IDs are already validated inside `FMap::load` (via
  `getSystem(id)` with a NULL-guard abort — P5-4 work); FF-1's remaining gap is
  the **fleet-level** references: a fleet's location (system) ID and its
  jump-route ID. Fleet location `0` means "not in a system" (gui guards
  `getLocation() != 0`); jump-route references use the `NO_ROUTE` sentinel — the
  Implementer confirms exact sentinel handling from `FFleet`/`StrategicTypes`.
- **FF-2 read-helper contract (confirmed):** `include/core/FPObject.h` —
  `readU32`, `readString`, and the `read<T>` template each return `0` on success
  and nonzero on stream failure / cap violation (post-I/O `is.fail()` check).
  So `FVehicle::load()` can check every read and nested `load()` return directly;
  the fix is a deterministic mirror of the FR-1 propagation pattern.
- **FF-2 current state:** `FVehicle::load()` (`src/ships/FVehicle.cpp:215-303`)
  already null-checks `createWeapon()`/`createDefense()` and returns 1 on unknown
  type; it already deletes prior `m_weapons`/`m_defenses` and re-points
  `m_currentDefense` safely. FF-2 adds return-checking to the scalar reads and the
  nested `w->load()`/`d->load()` calls, aborting nonzero on failure without leak,
  dangling entry, or double-free (mirroring the existing factory-abort cleanup
  discipline).
- **Existing behavioral-test homes (Tester-owned):**
  - FF-1: `tests/strategic/FGameSaveFormatTest.{h,cpp}` (aggregated `FGame::load()`
    error-propagation coverage vs a mock `IStrategicUI`).
  - FF-2 (+ FF-3 coverage): `tests/strategic/FGameSaveFormatTest.{h,cpp}` and/or
    `tests/ships/FVehicleTest.{h,cpp}` — the Tester selects the home matching each
    assertion's construction needs (full-game round-trip vs unit-level
    `FVehicle::load`).

## 4. Scope

### IN scope
- FF-1: load-time validation of fleet located-object reference IDs (system
  location ID and jump-route ID) against the loaded `FMap` in the strategic load
  path; abort the load (nonzero, reported via `IStrategicUI`, no live singleton)
  on any out-of-range reference.
- FF-2: harden `FVehicle::load()` to check its own scalar reads and nested
  weapon/defense `load()` returns, aborting nonzero on failure with correct
  cleanup.
- FF-3 coverage delivered inside FF-2's Tester scope (all four FR-1 propagation
  sites individually exercised).
- Doxygen header updates on every touched function/class per repo convention.

### OUT of scope
- GUI draw-path NULL guards for `getSystem()`/`getStart()`/`getEnd()` (explicitly
  excluded by decision D1 — the load-time reject makes them unnecessary; the gui
  files are not modified).
- `SF-enum-codes` and `SF-fhexmap` (remain deferred, untouched).
- Any change to the wire format itself or to the FR-1/FR-2 code already merged.

---

## 5. Subtasks

### FF-1 — Reject out-of-range located-object IDs at load

**Module:** strategic. **Security review: required** (untrusted-save-input
parsing / trust-boundary hardening).

**Allowed files:**
- `src/strategic/FGame.cpp`
- `include/strategic/FGame.h` (only if a helper declaration / Doxygen update is needed)
- `src/strategic/FFleet.cpp`
- `include/strategic/FFleet.h` (only if a helper declaration / Doxygen update is needed)
- `include/strategic/FMap.h` (only if a lookup/predicate helper is needed; do not
  add gui coupling)

**Task:** During the strategic load, after the `FMap` (systems + jump routes) is
loaded and fleets are read, validate each fleet's located-object reference IDs
against the loaded `FMap`: a fleet's location (system) ID must resolve via
`FMap::getSystem(id)` (when the location is a real system, i.e. not the "not in a
system" `0`/sentinel value), and its jump-route ID must resolve via
`FMap::getJumpRoute(id)` (when set to a real route, i.e. not the `NO_ROUTE`
sentinel). On the first unresolved reference, abort the load: return nonzero from
`FGame::load()` so the existing aggregate-abort machinery
(`reportLoadError(...)` → `IStrategicUI::showMessage(...)`, no half-built game
committed as the live singleton, `FMainFrame::onOpen` teardown via `resetGame()`)
fires. The Implementer chooses whether to validate inline in `FFleet::load()`
(using the `FMap` singleton) or in a post-fleet-load validation pass in
`FGame::load()`; either is acceptable, but the abort must propagate through
`FGame::load()`'s existing error path. Confirm exact sentinel values from
`FFleet`/`StrategicTypes` rather than assuming them.

**Acceptance criteria (implementation-outcome):**
- A save whose fleet carries an out-of-range location (system) ID causes
  `FGame::load()` to return nonzero, report via the installed `IStrategicUI`
  exactly once, and commit no live game singleton.
- A save whose fleet carries an out-of-range jump-route ID (in the in-transit /
  non-sentinel case) likewise aborts the load with the same error path.
- A valid save (all fleet references resolve, including the legitimate "not in a
  system" / `NO_ROUTE` sentinel cases) loads unchanged — no false rejection, no
  behavior change on the happy path.
- The gui draw path is unchanged (no gui files modified); it can no longer receive
  an out-of-range ID because the load now rejects such saves.
- `make check` is green (SSWTests / TacticalTests / GuiTests).

**Documentation impact:** Doxygen on the touched `load()` / validation
function(s); a concise note in the `AGENTS.md` save-format contributor paragraph
that load now rejects out-of-range fleet located-object references. No
`doc/deferred-tasks.md` change (FF-1 was never a tracked deferred item and is
being fixed, not deferred). No `doc/UsersGuide.md` change beyond what the existing
File→Open corrupt-save note already covers.

**Tester guidance (Tester-owned):** add behavioral coverage in
`tests/strategic/FGameSaveFormatTest.{h,cpp}` that builds a valid full-game save,
rewrites a fleet's location (system) ID (and, in a second case, its jump-route ID)
to an out-of-range value, and asserts `FGame::load()` returns nonzero, increments
the mock `IStrategicUI` `showMessage()` count by exactly one, and leaves no live
singleton. Include a positive control proving a valid save with legitimate
sentinel values still loads.

---

### FF-2 — Deepen the aggregate-abort guarantee into `FVehicle::load()`

**Module:** ships. **Security review: required** (untrusted-save-input parsing /
trust-boundary hardening).

**Allowed files:**
- `src/ships/FVehicle.cpp`
- `include/ships/FVehicle.h` (only if a Doxygen update is needed)

**Task:** In `FVehicle::load()`, check the return of every `readU32`/`readString`/
`read<T>` scalar read and of each nested `w->load(is)`/`d->load(is)` call. On any
nonzero read/load return, abort: clean up any partially-constructed state
consistent with the function's existing cleanup discipline (the weapon/defense
lists are already deleted/rebuilt within `load()`, and `m_currentDefense` is
re-pointed safely) — ensuring no leak, no dangling `m_currentDefense`, and no
double-free — and return nonzero so `FVehicle::load()`'s caller (`FFleet::load()`
/ `FPlayer::load()`, already FR-1-hardened) propagates the abort up to
`FGame::load()`. Preserve the existing `createWeapon()`/`createDefense()==NULL`
unknown-type abort and the CRIT-3/H2 active-defense fallback unchanged for the
success path.

**Acceptance criteria (implementation-outcome):**
- Every scalar read and nested weapon/defense `load()` in `FVehicle::load()` has
  its return checked; a nonzero return aborts `FVehicle::load()` with nonzero.
- A save stream truncated strictly inside a vehicle's own scalar / weapon /
  defense region causes `FGame::load()` to return nonzero, report via the mock
  `IStrategicUI` once, and commit no live singleton (the abort now reaches this
  depth, which it did not before).
- No leak, dangling `m_currentDefense`, or double-free on the abort path; the
  success path (valid vehicle) and the existing unknown-type abort are unchanged.
- `make check` is green.

**Documentation impact:** Doxygen on `FVehicle::load()` (bump `@date Last
Modified`, add implementing model to `@author`); one-line accuracy update to the
`AGENTS.md` save-format note that the aggregate-abort guarantee now reaches inside
a vehicle's own record. No `doc/deferred-tasks.md` change.

**Tester guidance (Tester-owned — includes the FF-3 coverage):** add behavioral
coverage that (a) truncates a stream inside a vehicle's own scalar region and
asserts `FGame::load()` aborts nonzero and reports once; and (b) exercises **all
four** FR-1 propagation sites individually by driving a nonzero
`FVehicle::load()` return for a vehicle placed, respectively, in a **fleet** (→
`FFleet` ship-loop + `FPlayer` fleet-loop), in **`m_unattached`**, and in
**`m_destroyed`**, asserting `FGame::load()` aborts in each case. A corrupt
in-ship weapon-type tag (existing `createWeapon()==NULL` path) or an in-vehicle
truncation may be used to trigger the nonzero `FVehicle::load()`. Homes:
`tests/strategic/FGameSaveFormatTest.{h,cpp}` for the full-game cases and/or
`tests/ships/FVehicleTest.{h,cpp}` for unit-level `FVehicle::load` cases.

---

## 6. Documentation impact (overall)

- Doxygen headers per subtask above.
- `AGENTS.md`: extend the save-format contributor note to record (a) load-time
  rejection of out-of-range fleet located-object references (FF-1) and (b) the
  aggregate-abort guarantee now reaching inside `FVehicle::load()` (FF-2). Keep it
  concise and non-duplicative (single-source rule).
- `doc/deferred-tasks.md`: no change — neither FF-1 nor FF-2 was a tracked
  deferred item, and both are being fixed rather than deferred. (`SF-enum-codes`
  and `SF-fhexmap` remain.)
- No user-facing behavior change beyond the already-documented corrupt-save
  File→Open handling.

## 7. Dependency ordering & parallelization

- **FF-1 and FF-2 are logically independent** (disjoint source: FF-1 =
  `src/strategic/FGame.cpp`/`FFleet.cpp`; FF-2 = `src/ships/FVehicle.cpp`).
- They share **no source files**, but both may edit `AGENTS.md`. Because of that
  documentation overlap, **sequence them (FF-1 → FF-2)** rather than running them
  in parallel, so the `AGENTS.md` edits do not contend; the Coordinator serializes
  the doc merge. (Conservative choice consistent with the prior follow-ups plan;
  there is no functional dependency in either direction.)
- FF-2 hardens `FVehicle::load()`, which `FFleet::load()`/`FPlayer::load()` (FR-1)
  already propagate; FF-1's fleet-reference validation is orthogonal to FF-2's
  in-vehicle read checks — neither blocks the other.

---

## 8. Implementer prompts (Coordinator-ready)

### FF-1 Implementer prompt

```
Your role is 'implementer'. Your task is as follows:

Reject out-of-range located-object reference IDs at load time (reviewer follow-up
FF-1). A corrupt/malicious save can carry a fleet whose location (system) ID or
jump-route ID does not resolve to any loaded FSystem/FJumpRoute; today the load
accepts it and the gui draw path later dereferences the NULL result of
FMap::getSystem(id) and crashes. Fix this at LOAD TIME (root-cause), not with gui
guards: validate each fleet's located-object references against the already-loaded
FMap and abort the load on any out-of-range reference. Do NOT modify the gui draw
path.

Allowed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h (only if a helper declaration / Doxygen update is needed)
- src/strategic/FFleet.cpp
- include/strategic/FFleet.h (only if a helper declaration / Doxygen update is needed)
- include/strategic/FMap.h (only if a lookup/predicate helper is needed; do not add gui coupling)

Task:
FGame::load() loads m_universe (FMap: all systems + jump routes) before players/
fleets, so the full FMap is available to validate fleet reference IDs. FJumpRoute
start/end SYSTEM IDs are already validated in FMap::load; the remaining gap is the
FLEET-level references. After fleets are loaded, validate each fleet's location
(system) ID via FMap::getSystem(id) (when the location is a real system, not the
"not in a system" 0/sentinel value) and its jump-route ID via
FMap::getJumpRoute(id) (when set to a real route, not the NO_ROUTE sentinel). On
the first unresolved reference, return nonzero from FGame::load() so the existing
aggregate-abort machinery fires: reportLoadError(...) -> IStrategicUI::showMessage(...),
no half-built game committed as the live singleton, FMainFrame::onOpen teardown via
resetGame(). You may validate inline in FFleet::load() (using the FMap singleton) or
in a post-fleet-load pass in FGame::load(); either is fine as long as the abort
propagates through FGame::load()'s existing error path. Confirm the exact sentinel
values (fleet "no system", NO_ROUTE) from FFleet/StrategicTypes rather than assuming.
Do not modify any gui file; do not add gui coupling to the strategic model.

Acceptance criteria:
- A save whose fleet carries an out-of-range location (system) ID makes
  FGame::load() return nonzero, report via the installed IStrategicUI exactly once,
  and commit no live game singleton.
- A save whose fleet carries an out-of-range jump-route ID (non-sentinel case)
  likewise aborts via the same error path.
- A valid save (all references resolve, including legitimate "no system"/NO_ROUTE
  sentinels) loads unchanged -- no false rejection, no happy-path behavior change.
- No gui files modified.
- Update Doxygen on the touched function(s) (bump @date Last Modified; add your
  model to @author).
- make check is green (SSWTests / TacticalTests / GuiTests).

Validation guidance:
Build and run `make check` from the repo root. Faster inner loop:
`cd tests/strategic && make && ./StrategicTests`.

Tester handoff:
Behavioral coverage (out-of-range location ID, out-of-range jump-route ID, and a
valid-sentinel positive control) belongs in tests/strategic/FGameSaveFormatTest.{h,cpp}
and is the Tester's responsibility -- do not author it yourself and do not add
tester-owned files to your changes. Leave a note describing how to mutate a valid
save's fleet reference ID to an out-of-range value.

Artifact guidance:
Write your implementer report and result under
artifacts/save-load-robustness-followups/FF-1/ (repository-root-relative), following
the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs are
missing; the design is fully specified above and requires no clarification turn.

Do not report success unless all required artifacts exist and all changes are
committed.
```

### FF-2 Implementer prompt

```
Your role is 'implementer'. Your task is as follows:

Deepen the save-load aggregate-abort guarantee into FVehicle::load() (reviewer
follow-up FF-2). FVehicle::load() currently discards the return of its own scalar
reads (readU32/readString/read<T>) and of its nested weapon/defense w->load(is)/
d->load(is) calls, returning nonzero only on the createWeapon()/createDefense()==NULL
factory paths. So a stream truncated strictly inside a single vehicle's own
scalar/weapon/defense region still returns 0, and FR-1's per-container abort
guarantee does not reach that depth. Close that gap.

Allowed files:
- src/ships/FVehicle.cpp
- include/ships/FVehicle.h (only if a Doxygen update is needed)

Task:
In FVehicle::load() (src/ships/FVehicle.cpp, ~lines 215-303), check the return of
every readU32/readString/read<T> scalar read and of each nested w->load(is)/
d->load(is) call. (Per include/core/FPObject.h these helpers already return 0 on
success and nonzero on stream failure / cap violation.) On any nonzero read/load
return, abort: clean up partially-constructed state consistent with the function's
existing discipline -- the m_weapons/m_defenses lists are deleted/rebuilt within
load() and m_currentDefense is re-pointed safely -- ensuring no leak, no dangling
m_currentDefense, and no double-free, then return nonzero. Preserve unchanged: the
existing createWeapon()/createDefense()==NULL unknown-type abort, and the CRIT-3/H2
active-defense-index fallback on the success path. The nonzero return propagates
through the already-FR-1-hardened FFleet::load()/FPlayer::load() up to FGame::load().

Acceptance criteria:
- Every scalar read and nested weapon/defense load() in FVehicle::load() has its
  return checked; a nonzero return aborts FVehicle::load() with nonzero.
- A save stream truncated strictly inside a vehicle's own scalar/weapon/defense
  region makes FGame::load() return nonzero, report via the mock IStrategicUI once,
  and commit no live singleton.
- No leak, dangling m_currentDefense, or double-free on the abort path; the success
  path and the existing unknown-type abort are unchanged.
- Update Doxygen on FVehicle::load() (bump @date Last Modified; add your model to
  @author).
- make check is green (SSWTests / TacticalTests / GuiTests).

Validation guidance:
Build and run `make check` from the repo root. Faster inner loops:
`cd tests/ships && make && ./ShipsTests` and `cd tests/strategic && make && ./StrategicTests`.

Tester handoff:
Behavioral coverage is the Tester's responsibility -- do not author it yourself and
do not add tester-owned files to your changes. Leave a note for the Tester covering
BOTH: (a) an in-vehicle scalar-region truncation that makes FGame::load() abort
nonzero; and (b) driving a nonzero FVehicle::load() for a vehicle placed in a fleet,
in m_unattached, and in m_destroyed so all four FR-1 propagation sites (FFleet
ship-loop, FPlayer fleet-loop/m_unattached/m_destroyed) are each exercised (this
also delivers reviewer follow-up FF-3). Suggested homes:
tests/strategic/FGameSaveFormatTest.{h,cpp} and/or tests/ships/FVehicleTest.{h,cpp}.

Artifact guidance:
Write your implementer report and result under
artifacts/save-load-robustness-followups/FF-2/ (repository-root-relative), following
the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs are
missing; the design is fully specified above and requires no clarification turn.

Do not report success unless all required artifacts exist and all changes are
committed.
```

---

## 9. Coordinator notes

- Run each subtask through the default Implementer → Tester → Documenter →
  Security → Verifier chain (both marked `Security review: required`), then a final
  Reviewer pass after both merge back.
- Use a dedicated per-plan coordination branch off `phase5` (or reuse `phase5`
  directly, since it is a non-master branch); do not base on `master`/`main`.
- **Sequence FF-1 then FF-2** (documentation-overlap on `AGENTS.md`); no functional
  dependency.
- FF-3 is intentionally NOT a subtask — it is delivered as expanded Tester coverage
  inside FF-2.
- Store artifacts under `artifacts/save-load-robustness-followups/<FF-1|FF-2>/`.
