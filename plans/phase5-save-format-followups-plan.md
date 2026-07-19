# Phase 5 Save-Format Hardening — Reviewer Follow-Ups Plan

**Planner:** claude-opus-4-8[1m] (planner role)
**Date:** Jul 19, 2026
**Source:** `artifacts/phase5-save-format-hardening/reviewer_report.md` follow-up
requests FR-1 and FR-2; `doc/deferred-tasks.md` entries
`SF-nested-load-returns` and `SF-nullfmap-paint-guard`.
**Coordination base branch:** create a dedicated per-plan coordination branch off
`phase5` (do not use `master`/`main`).

---

## 1. Feature restatement (engineering terms)

Two independent hardening follow-ups closing the two tracked deferrals carried out
of the Phase 5 save-format-hardening review (both non-blocking at review time):

1. **FR-1 / `SF-nested-load-returns`** — Make the save-load aggregate-abort
   guarantee robust against deep truncation. `FPlayer::load()` and
   `FFleet::load()` currently discard the return codes of nested
   `FVehicle::load()` / `FFleet::load()` calls, so a stream truncated *inside* a
   ship/fleet record lets the discarding caller return 0 while the data is
   incomplete — `FGame::load()`'s per-player abort check is bypassed and a
   logically-inconsistent game is committed as the live singleton instead of the
   intended load-error path. Data-integrity/robustness, not memory-safety.

2. **FR-2 / `SF-nullfmap-paint-guard`** — Add the root-cause NULL/invalid-`FMap`
   guard in the gui draw chain so a `FGamePanel` shown before or during an
   unloaded/partially-loaded game cannot dereference a NULL/half-built `FMap`
   singleton (`FGamePanel::onPaint` → `WXGameDisplay::draw` →
   `WXMapDisplay::getScale` → `FMap::getMaxSize()` with `this=0x0`). P5-5 already
   closed the one reachable `onOpen()` trigger via a call-site mitigation; this
   adds defense-in-depth in the paint path itself for any other path.

---

## 2. Confirmed repository facts

- **FR-1 discard sites (confirmed at file level):**
  - `src/strategic/FPlayer.cpp` `FPlayer::load()`:
    - `m_unattached` loop — `v->load(is);` return discarded (line ~154).
    - fleet loop — `f->load(is);` return discarded (line ~160).
    - `m_destroyed` loop — `v->load(is);` return discarded (line ~177).
  - `src/strategic/FFleet.cpp` `FFleet::load()`:
    - ship loop — `v->load(is);` return discarded (line ~197).
  - Both loops already null-check `createShip(type)` and `return 1;` on unknown
    type, so the abort-and-return pattern already exists to mirror.
  - `FGame::load()` (`src/strategic/FGame.cpp`) already checks each player's
    `load()` return and routes failures through `reportLoadError(...)` →
    `IStrategicUI::showMessage(...)` (console fallback), returning nonzero and
    tearing down via `resetGame()`; FR-1 only needs the nested callers to
    propagate correctly for that existing top-level machinery to fire.
- **FR-2 crash chain (confirmed at file level):**
  - `src/FGamePanel.cpp` `FGamePanel::onPaint()` already guards `if (m_game !=
    NULL)` before calling `m_gameDisplay.draw(dc, *m_game)`; the residual gap is
    that `m_game` can be set while the `FMap` singleton is NULL/half-built.
  - `src/gui/WXGameDisplay.cpp` `WXGameDisplay::draw(wxDC&, FGame&)` constructs a
    `WXMapDisplay` and calls into it.
  - `src/gui/WXMapDisplay.cpp` `WXMapDisplay::getScale()` and `draw()` both call
    `FMap::getMap()` and then `map.getMaxSize()`.
  - `FMap::getMap()` (`src/strategic/FMap.cpp`) returns `*m_map`; when the private
    static `m_map` pointer is NULL, this is a NULL-reference deref (the header
    doxygen at `include/strategic/FMap.h:97` already documents that `getMap()`
    "returns a null reference" when the map does not exist). `m_map` is a
    **private static** (`include/strategic/FMap.h:43`); there is currently **no
    public predicate** to test existence without dereferencing.
- **Existing behavioral-test homes (Tester-owned):**
  - FR-1: `tests/strategic/FGameSaveFormatTest.{h,cpp}` already covers aggregated
    `FGame::load()` error propagation (wrong magic, unsupported version, truncated
    stream, unknown factory type) asserted against a mock `IStrategicUI`'s
    `showMessage()` count plus a nonzero `load()` return — the natural home for a
    deep-truncation-inside-a-fleet case.
  - FR-2: `tests/gui/StrategicGuiLiveTest.{h,cpp}` already covers the
    corrupt-load forced-repaint case
    (`testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint`) — the
    natural home for a forced-repaint-with-NULL-map case.
- The two items are in **different modules with no shared source files** (FR-1:
  strategic `.cpp`; FR-2: gui `.cpp`/`.h`, plus at most a small strategic `FMap`
  predicate). Independent.

## 3. Assumptions (labeled — not confirmed fact)

- **[Assumption A1 — FR-2 detection mechanism]** Because `FMap::m_map` is a
  private static and `getMap()` returns `*m_map`, the gui paint path cannot test
  map existence without a strategic-side accessor. The expected implementation is
  a small public static predicate on `FMap` (e.g. `static bool hasMap();` /
  `static bool exists();` returning `m_map != NULL`) consumed by the gui guard.
  Adding such a predicate to `include/strategic/FMap.h` + `src/strategic/FMap.cpp`
  is included in FR-2's allowed files. The Implementer chooses the exact predicate
  name and the exact guard insertion point(s) within the gui draw chain; this is
  an implementation detail, not a scope-changing design decision. (A guard on a
  non-positive `getMaxSize()` is **not** an acceptable substitute, because it
  still dereferences the NULL `m_map`.)
- **[Assumption A2]** `master...phase5` is the correct lineage; the coordination
  branch is cut from `phase5`.

## 4. Scope

### IN scope
- FR-1: propagate nested `load()` return codes in `FPlayer::load()` (all three
  loops) and `FFleet::load()` (ship loop); on any nonzero nested return, delete
  the just-created sub-object and return nonzero.
- FR-2: add a NULL/invalid-`FMap` guard in the gui draw chain (plus the minimal
  strategic `FMap` existence predicate needed to test it safely) so a forced
  repaint with no/half-built map is a no-op rather than a crash.
- Doxygen header updates on every touched function/class per repository
  convention (`@date Last Modified` bump, add implementing model to `@author`).
- Close-out of the two `doc/deferred-tasks.md` entries as resolved.

### OUT of scope
- `SF-enum-codes` (weapon/defense enum-ordinal portability) — remains deferred.
- `SF-fhexmap` (tactical `FHexMap::save/load` stub) — remains deferred.
- Any change to the wire format, `FGame::save/load` header, or the P5-5 `onOpen`
  call-site mitigation (which stays in place).
- Broadening the gui guard into a general FGamePanel/WXGameDisplay redesign.

---

## 5. Subtasks

### FR-1 — Propagate nested `load()` return codes in `FPlayer`/`FFleet`

**Module:** strategic. **Security review: required** (untrusted-save-input
parsing / trust-boundary hardening).

**Allowed files:**
- `src/strategic/FPlayer.cpp`
- `include/strategic/FPlayer.h` (only if a Doxygen header update is needed)
- `src/strategic/FFleet.cpp`
- `include/strategic/FFleet.h` (only if a Doxygen header update is needed)

**Task:** In `FPlayer::load()`, capture the return of `v->load(is)` in both the
`m_unattached` loop and the `m_destroyed` loop, and of `f->load(is)` in the fleet
loop. In `FFleet::load()`, capture the return of `v->load(is)` in the ship loop.
On any nonzero nested return: `delete` the just-created sub-object (the `FVehicle*`
/ `FFleet*` that was allocated in that iteration and not yet — or, if already,
then removed from — the owning container) and `return` nonzero from the enclosing
`load()`, mirroring the existing `createShip(...) == NULL → return 1;` abort
pattern and honoring each class's ownership/cleanup contract (no leak, no
double-free, no dangling entry left in `m_unattached`/`m_destroyed`/`m_fleets`/
`m_ships`). Do not commit a partially-populated object.

**Acceptance criteria (implementation-outcome):**
- Every nested `v->load(is)` / `f->load(is)` call in `FPlayer::load()` and
  `FFleet::load()` has its return value checked.
- A nonzero nested return causes the enclosing `load()` to free the just-created
  sub-object and return nonzero, with no element left dangling in the owning
  container and no memory leak or double-free.
- The existing unknown-ship-type (`createShip == NULL`) abort behavior is
  preserved.
- With the change, a save stream truncated inside a fleet's ship record causes
  `FGame::load()` to return nonzero and route through the existing
  `reportLoadError(...)` / `IStrategicUI::showMessage(...)` path, committing no
  live game singleton (behavior verified by the Tester).
- `make check` is green (SSWTests / TacticalTests / GuiTests).

**Documentation impact:** Doxygen `save`/`load` headers on the touched
`FPlayer`/`FFleet` functions; `doc/deferred-tasks.md` — mark
`SF-nested-load-returns` resolved/removed. No `AGENTS.md` change expected unless
the existing save-format note's error-contract wording needs a one-line accuracy
tweak.

**Tester guidance (Tester-owned — not an Implementer deliverable):** add
behavioral coverage in `tests/strategic/FGameSaveFormatTest.{h,cpp}` that writes a
valid full-game save, truncates the byte stream inside a fleet's ship record, and
asserts `FGame::load()` returns nonzero, increments the mock `IStrategicUI`
`showMessage()` count, and leaves no live game singleton. Must fail against the
pre-fix code and pass after.

---

### FR-2 — Root-cause NULL/invalid-`FMap` guard in the gui draw chain

**Module:** gui (+ minimal strategic `FMap` existence predicate). **Security
review: required** (crash/NULL-deref hardening on an untrusted-save-triggered
path).

**Allowed files:**
- `src/gui/WXMapDisplay.cpp`
- `include/gui/WXMapDisplay.h` (only if a signature/Doxygen update is needed)
- `src/gui/WXGameDisplay.cpp` (only if the guard is placed here instead of/in
  addition to WXMapDisplay)
- `include/gui/WXGameDisplay.h` (only if needed)
- `src/strategic/FMap.cpp` (add the existence predicate — see Assumption A1)
- `include/strategic/FMap.h` (declare the existence predicate)

**Task:** Add a NULL/invalid-`FMap` guard in the gui draw chain so that when the
`FMap` singleton does not exist (static `m_map == NULL`), `WXMapDisplay::getScale`
/ `WXMapDisplay::draw` (and/or `WXGameDisplay::draw`) skip the map draw / return a
safe default scale instead of dereferencing `FMap::getMap()`'s null reference.
Because `m_map` is a private static, add a minimal public static existence
predicate to `FMap` (e.g. `static bool hasMap();` returning `m_map != NULL`) and
consult it in the guard rather than dereferencing first. The guard must be
independent of the P5-5 `onOpen` call-site mitigation (leave that mitigation in
place) and must not change rendering behavior when a valid map exists.

**Acceptance criteria (implementation-outcome):**
- The gui draw chain no longer dereferences the `FMap` singleton when it does not
  exist; a forced repaint of a `FGamePanel` whose map is unset/half-built is a
  no-op (or draws only non-map content) and does not crash.
- Rendering output/behavior is unchanged when a valid `FMap` exists (existing
  `WXMapDisplay`/`WXGameDisplay` rendering tests still pass).
- The guard is in the gui draw path itself (not solely reliant on
  `FMainFrame::onOpen`); the existing `onOpen` mitigation remains in place.
- `make check` is green (SSWTests / TacticalTests / GuiTests, the last under
  `xvfb-run -a`).

**Documentation impact:** Doxygen headers on the new `FMap` predicate and any
touched gui draw function; `doc/deferred-tasks.md` — mark
`SF-nullfmap-paint-guard` resolved/removed; the `AGENTS.md` GUI-tests / paint-path
notes and the `FMap`-null-paint follow-up reference should be updated to reflect
that the root-cause guard now exists (the note currently states the root-cause
guard "remains a tracked follow-up").

**Tester guidance (Tester-owned — not an Implementer deliverable):** add
behavioral coverage in `tests/gui/StrategicGuiLiveTest.{h,cpp}` that forces a
`FGamePanel`/`WXGameDisplay` repaint (e.g. via an offscreen `wxMemoryDC` or a
shown frame) with the `FMap` singleton unset/half-built and asserts no crash and a
safe no-op draw. Must fail (crash) against the pre-fix code and pass after.

---

## 6. Documentation impact (overall)

- `doc/deferred-tasks.md`: remove/mark-resolved both `SF-nested-load-returns`
  (FR-1) and `SF-nullfmap-paint-guard` (FR-2); retain `SF-enum-codes` and
  `SF-fhexmap`.
- `AGENTS.md`: FR-2 — update the `FMap`-null-paint / `SF-nullfmap-paint-guard`
  references so they no longer describe the root-cause guard as an open follow-up;
  FR-1 — one-line accuracy tweak to the save-format error-contract note only if
  wording now overstates/understates the nested-load guarantee.
- Doxygen headers per subtask above.
- No user-facing behavior change requiring a `doc/UsersGuide.md` edit (both are
  robustness/crash-hardening of already-documented failure handling).

## 7. Dependency ordering & parallelization

- **FR-1 and FR-2 are independent and may run in parallel.** They touch disjoint
  source files (FR-1: `src/strategic/FPlayer.cpp`, `src/strategic/FFleet.cpp`;
  FR-2: `src/gui/*`, `src/strategic/FMap.cpp`/`.h`). FR-1 does **not** touch
  `FMap`, so there is no shared-file contention with FR-2's `FMap` predicate.
- Both edit `doc/deferred-tasks.md`, but in separate, non-overlapping entries; the
  Coordinator should serialize only the final doc-merge if the two branches are
  developed concurrently (routine merge, not a design dependency).
- No ordering dependency exists in either direction; neither blocks the other.

---

## 8. Implementer prompts (Coordinator-ready)

### FR-1 Implementer prompt

```
Your role is 'implementer'. Your task is as follows:

Harden the save-load path so the top-level aggregate-abort guarantee in
FGame::load() is not bypassed by deep-truncation input (closes deferred item
SF-nested-load-returns / reviewer FR-1).

Allowed files:
- src/strategic/FPlayer.cpp
- include/strategic/FPlayer.h (only if a Doxygen header update is needed)
- src/strategic/FFleet.cpp
- include/strategic/FFleet.h (only if a Doxygen header update is needed)

Task:
In FPlayer::load(), capture the return value of v->load(is) in BOTH the
m_unattached loop and the m_destroyed loop, and of f->load(is) in the fleet loop.
In FFleet::load(), capture the return value of v->load(is) in the ship loop.
On any nonzero nested return: delete the just-created sub-object allocated in that
iteration and return nonzero from the enclosing load(), mirroring the existing
`createShip(...) == NULL -> return 1;` abort pattern. Ensure no element is left
dangling in the owning container (m_unattached / m_destroyed / m_fleets / m_ships),
no memory leak, and no double-free (respect each class's ownership/cleanup
contract). Do not commit a partially-populated object. Preserve the existing
unknown-ship-type (createShip == NULL) abort behavior unchanged.

Acceptance criteria:
- Every nested v->load(is) / f->load(is) return in FPlayer::load() and
  FFleet::load() is checked.
- A nonzero nested return frees the just-created sub-object and returns nonzero,
  with nothing left dangling in the owning container and no leak/double-free.
- A save stream truncated inside a fleet's ship record makes FGame::load() return
  nonzero, route through the existing reportLoadError(...) /
  IStrategicUI::showMessage(...) path, and commit no live game singleton.
- Update Doxygen headers on the touched load() functions (bump @date Last
  Modified; add your model to @author).
- Mark deferred item SF-nested-load-returns resolved/removed in
  doc/deferred-tasks.md.
- make check is green (SSWTests / TacticalTests / GuiTests).

Validation guidance:
Build and run `make check` from the repo root (builds all + runs SSWTests,
TacticalTests, and GuiTests under xvfb-run). For a faster inner loop use
`cd tests/strategic && make && ./StrategicTests`.

Tester handoff:
Behavioral coverage for the deep-truncation-inside-a-fleet case belongs in
tests/strategic/FGameSaveFormatTest.{h,cpp} and is the Tester's responsibility --
do not author it yourself and do not add tester-owned files to your changes.
Leave a clear note for the Tester describing the exact truncation point to
exercise.

Artifact guidance:
Write your implementer report and result under
artifacts/phase5-save-format-followups/FR-1/ (repository-root-relative), following
the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs
are missing; the design is fully specified above and requires no clarification
turn.

Do not report success unless all required artifacts exist and all changes are
committed.
```

### FR-2 Implementer prompt

```
Your role is 'implementer'. Your task is as follows:

Add the root-cause NULL/invalid-FMap guard in the gui draw chain so a FGamePanel
shown before or during an unloaded/partially-loaded game cannot dereference a
NULL/half-built FMap singleton (closes deferred item SF-nullfmap-paint-guard /
reviewer FR-2). This is defense-in-depth beyond the already-in-place P5-5
FMainFrame::onOpen call-site mitigation, which must remain unchanged.

Allowed files:
- src/gui/WXMapDisplay.cpp
- include/gui/WXMapDisplay.h (only if a signature/Doxygen update is needed)
- src/gui/WXGameDisplay.cpp (only if you place the guard here as well)
- include/gui/WXGameDisplay.h (only if needed)
- src/strategic/FMap.cpp (add a minimal public static existence predicate)
- include/strategic/FMap.h (declare the existence predicate)

Task:
FMap::getMap() returns *m_map, and the private static m_map can be NULL while a
FGame is set, so WXMapDisplay::getScale()/draw() (reached via
FGamePanel::onPaint -> WXGameDisplay::draw) can dereference a null reference and
call FMap::getMaxSize() with this=0x0. Add a minimal public static predicate to
FMap (e.g. `static bool hasMap();` returning m_map != NULL) and consult it in the
gui draw chain BEFORE dereferencing FMap::getMap(): when the map does not exist,
skip the map draw / return a safe default scale instead of crashing. Do NOT guard
merely on getMaxSize() being non-positive -- that still dereferences the NULL
m_map. Do not change rendering behavior when a valid map exists. Leave the
FMainFrame::onOpen setGame-deferral mitigation in place.

Acceptance criteria:
- The gui draw chain no longer dereferences the FMap singleton when it does not
  exist; a forced repaint of a FGamePanel with an unset/half-built map is a no-op
  (or draws only non-map content) and does not crash.
- Rendering output/behavior is unchanged when a valid FMap exists (existing
  WXMapDisplay/WXGameDisplay rendering tests still pass).
- The guard lives in the gui draw path itself (not solely reliant on
  FMainFrame::onOpen); the existing onOpen mitigation is unchanged.
- Add Doxygen headers on the new FMap predicate and any touched gui draw function
  (bump @date Last Modified; add your model to @author).
- Mark deferred item SF-nullfmap-paint-guard resolved/removed in
  doc/deferred-tasks.md, and update the AGENTS.md references that currently
  describe the root-cause guard as an open follow-up.
- make check is green (SSWTests / TacticalTests / GuiTests under xvfb-run).

Validation guidance:
Build and run `make check` from the repo root. For a faster gui inner loop use
`cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff:
Behavioral coverage that forces a repaint with the FMap singleton unset/half-built
and asserts no crash belongs in tests/gui/StrategicGuiLiveTest.{h,cpp} and is the
Tester's responsibility -- do not author it yourself and do not add tester-owned
files to your changes. Leave a clear note for the Tester describing how to reach
the paint path with a NULL map.

Artifact guidance:
Write your implementer report and result under
artifacts/phase5-save-format-followups/FR-2/ (repository-root-relative), following
the existing per-subtask artifact convention used under artifacts/.

Security review: required

Continue past preflight and proceed with implementation when no blocking inputs
are missing; the design is fully specified above and requires no clarification
turn.

Do not report success unless all required artifacts exist and all changes are
committed.
```

---

## 9. Coordinator notes

- Run each subtask through the default Implementer → Tester → Documenter →
  Security → Verifier chain (both are marked `Security review: required`), then a
  final Reviewer pass after both merge back.
- Use a dedicated per-plan coordination branch off `phase5`; do not base on
  `master`/`main`.
- FR-1 and FR-2 may be executed in parallel; serialize only the final
  `doc/deferred-tasks.md` merge if developed concurrently.
- Store artifacts under `artifacts/phase5-save-format-followups/<FR-1|FR-2>/`.
