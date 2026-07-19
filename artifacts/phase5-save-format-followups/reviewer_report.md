# Reviewer Report — Phase 5 Save-Format Hardening Follow-Ups (FR-1, FR-2)

**Reviewer:** claude-opus-4-8[1m] (reviewer role)
**Date:** Jul 19, 2026
**Coordination branch:** `phase5` (reviewed from worktree branch
`phase5-reviewer-20260719`, based on `phase5`)
**Scope:** Final, read-only, feature-level review across both subtasks.

---

## Feature plan reviewed

- `plans/phase5-save-format-followups-plan.md` — implements reviewer follow-ups
  FR-1 and FR-2 from `artifacts/phase5-save-format-hardening/reviewer_report.md`,
  closing the two tracked deferrals `SF-nested-load-returns` and
  `SF-nullfmap-paint-guard`.

## Inputs reviewed

- Governing plan Section 4 (scope) and Section 5 (per-subtask acceptance
  criteria) for FR-1 and FR-2.
- Source/test/doc diff isolated to the follow-ups feature only (`817485a7...HEAD`,
  i.e. changes since the hardening Reviewer PASS): `src/strategic/FPlayer.cpp`,
  `src/strategic/FFleet.cpp`, `include/strategic/FPlayer.h`,
  `include/strategic/FFleet.h`, `src/strategic/FMap.cpp`,
  `include/strategic/FMap.h`, `src/gui/WXMapDisplay.cpp`,
  `include/gui/WXMapDisplay.h`, `src/gui/WXPlayerDisplay.cpp`,
  `include/gui/WXPlayerDisplay.h`, `tests/strategic/FGameSaveFormatTest.{h,cpp}`,
  `tests/gui/StrategicGuiLiveTest.{h,cpp}`, `doc/deferred-tasks.md`, `AGENTS.md`.
- Cross-checked the full `WXGameDisplay::draw()` paint chain
  (`src/gui/WXGameDisplay.cpp`) and `FVehicle::load()`
  (`src/ships/FVehicle.cpp`) to independently verify root-cause completeness and
  the accuracy of the carried non-blocking notes.
- Per-subtask artifacts: FR-1 (implementer/tester/documenter/security/verifier),
  FR-2 (same, plus `history/pass-1/` remediation cycle).
- Independent `make check` from the worktree root.

---

## Overall feature completeness

**COMPLETE.** Both subtasks deliver exactly the governing plan's Section 4 scope
and satisfy every acceptance criterion, confirmed at file level (not by trusting
subtask verdicts):

### FR-1 — Propagate nested `load()` return codes (closes `SF-nested-load-returns`)
- All four nested-load sites now check the return: `FPlayer::load()`'s
  `m_unattached` loop, fleet loop, and `m_destroyed` loop
  (`src/strategic/FPlayer.cpp`), and `FFleet::load()`'s ship loop
  (`src/strategic/FFleet.cpp`). On a nonzero nested return each `delete`s the
  just-allocated sub-object (which is not yet in the owning container, so no leak,
  no dangling entry, no double-free) and returns nonzero — mirroring the existing
  `createShip()==NULL → return 1;` abort. **Verified at file level.**
- The pre-existing unknown-ship-type abort is preserved unchanged.
- The `FFleet` ship-loop check is genuinely reachable: `FVehicle::load()`
  (`src/ships/FVehicle.cpp:250-276`) returns `1` on an unknown/corrupt
  weapon/defense type tag, so this is not dead code.
- Behavioral coverage:
  `FGameSaveFormatTest::testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce`
  writes a real valid full-game save, truncates immediately before a fleet's first
  ship record, and asserts `FGame::load()` returns nonzero and the mock
  `IStrategicUI::showMessage()` fires exactly once. This fails against pre-FR-1
  code (fleet-loop discarded the nested return, pushed the half-built fleet,
  returned 0) and passes after — a true behavioral test, registered in the suite.
- Doxygen `@author`/`@date` obligations met on both touched `load()` headers and
  both file headers; `SF-nested-load-returns` removed from
  `doc/deferred-tasks.md`; the `AGENTS.md` save-format contributor note is
  extended accurately to describe the nested-return propagation.

### FR-2 — Root-cause NULL/half-built-`FMap` gui-draw guard (closes `SF-nullfmap-paint-guard`)
- New public static predicate `FMap::hasMap()`
  (`include/strategic/FMap.h`/`src/strategic/FMap.cpp`) tests `m_map != NULL`
  without dereferencing — exactly the mechanism the plan's Assumption A1
  anticipated (not the disallowed `getMaxSize()`-non-positive substitute).
- The guard is applied at **all three** `FMap`-touching seams reachable from
  `WXGameDisplay::draw()`: `WXMapDisplay::draw()` (no-op return),
  `WXMapDisplay::getScale()` (safe default `1.0`), and
  `WXPlayerDisplay::drawFleets()` (early return before any
  `&(FMap::getMap())` reference bind). I independently enumerated
  `WXGameDisplay::draw()`'s call chain — `WXMapDisplay::draw()` (→ `getScale()`),
  per-player `WXPlayerDisplay::drawFleets()`, and `drawTurnCounter()` (which
  touches only `FGame&`, never `FMap`). **The gui paint path reached from
  `FGamePanel::onPaint()` is therefore root-cause-complete against a NULL/half-built
  `FMap`.**
- The P5-5 `FMainFrame::onOpen()` setGame-deferral mitigation is untouched
  (confirmed not in the feature diff); the two guards are additive
  defense-in-depth, with no behavior change when a valid `FMap` exists.
- Behavioral coverage: `testGamePanelRepaintWithNoLiveFMapDoesNotCrash` drives a
  real `FGamePanel::onPaint() → WXGameDisplay::draw() → WXMapDisplay` repaint with
  a set `FGame` but no `FMap`; `testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash`
  drives `drawFleets()` directly with a real `FPlayer`/`FFleet` at a nonzero,
  non-transit location and no `FMap`. Both would crash against unfixed code and
  reach their assertions after the fix — genuine behavioral tests, registered.
- Doxygen `@author`/`@date` obligations met on `FMap::hasMap()` and the touched
  gui draw functions; `SF-nullfmap-paint-guard` removed from
  `doc/deferred-tasks.md`; the `AGENTS.md` GUI-tests / paint-path note is rewritten
  to state the root-cause guard now exists and no longer describes it as an open
  follow-up, and does not overstate coverage (it correctly limits the claim to the
  two `FMap`-touching call paths of `WXGameDisplay::draw()`).

### Scope-expansion assessment (FR-2 → `WXPlayerDisplay`)
The FR-2 plan originally listed `WXMapDisplay.*`, `WXGameDisplay.*`, `FMap.*`. The
pass-1 Security review returned CONDITIONAL PASS on a WARNING that the pass-1
guard covered only the `WXMapDisplay` path while `WXGameDisplay::draw()` reaches
`FMap` via a **second, same-chain** path (`WXPlayerDisplay::drawFleets()`, which
binds `FMap *map = &(FMap::getMap())` — itself UB against a NULL reference). The
pass-2 remediation added `src/gui/WXPlayerDisplay.cpp/.h` to close it. **This
expansion is justified:** `drawFleets()` is part of the very "gui draw chain" the
plan set out to make root-cause-complete, so guarding only `WXMapDisplay` would
have left the plan's own acceptance criterion ("the gui draw chain no longer
dereferences the `FMap` singleton when it does not exist") unmet. This is
completion of the stated scope, not scope creep, and it was driven through the
correct Security→remediation→Verifier cycle.

### Deferred-item and documentation hygiene
- `doc/deferred-tasks.md` now retains exactly `SF-enum-codes` and `SF-fhexmap`;
  both closed items are fully removed (verified). This matches plan Section 6.
- `AGENTS.md` edits are accurate and consistent with the delivered code in both
  the save-format contributor note (FR-1) and the GUI-tests paint-path note
  (FR-2); neither overstates coverage.

### Independent build/test
`make check` from the worktree root (foreground, full run):
- **SSWTests: OK (290 tests)** — +1 vs the 289 hardening baseline (the FR-1
  truncation test).
- **TacticalTests: OK (253 tests)** — unchanged.
- **GuiTests: OK (85 tests)** (under `xvfb-run -a`) — +2 vs the 83 baseline (the
  two FR-2 no-map repaint tests).
All three suites green; worktree tree clean, all feature changes committed.

---

## Findings

### BLOCKING
- None.

### WARNING
- None.

### NOTE
1. **Pre-existing out-of-range fleet-location crash (untracked).** After a valid
   `FMap` exists, `WXPlayerDisplay::drawFleets()` (`src/gui/WXPlayerDisplay.cpp`,
   the `map->getSystem(location)->getCoord(...)` deref) and the analogous
   `WXMapDisplay` system/jump-route lookups do not NULL-check the `FSystem*`
   returned by `getSystem()`. A corrupt save carrying an out-of-range fleet
   location ID makes `getSystem()` return NULL and crash. This is a **distinct
   failure mode** from `SF-nullfmap-paint-guard` (a valid map that cannot resolve
   an untrusted ID, not a NULL map), is **pre-existing and not regressed by FR-2**,
   and is correctly outside FR-2's stated scope. It was surfaced by the FR-2
   Security review but is not yet recorded in `doc/deferred-tasks.md`. Translated
   into follow-up request **FF-1** below.
2. **`FVehicle::load()` self-region truncation gap (out of FR-1 scope).**
   `FVehicle::load()` (`src/ships/FVehicle.cpp:215-303`) still discards the return
   of its own scalar reads (`readU32`/`readString`/`read<T>`) and of the nested
   `w->load(is)`/`d->load(is)` weapon/defense loads; it returns nonzero only on the
   `createWeapon()`/`createDefense()==NULL` factory paths. So a stream truncated
   strictly inside a single vehicle's own scalar region can still return 0, and
   FR-1's aggregate-abort guarantee does not reach that depth. This is a deeper
   robustness gap correctly outside FR-1's stated scope. Translated into follow-up
   request **FF-2** below.
3. **FR-1 behavioral coverage exercises one of the four propagation sites
   directly.** The delivered test truncates *before* a fleet's first ship record,
   so `FFleet::load()` aborts via its pre-existing `createShip()==NULL` check and
   the test directly exercises `FPlayer::load()`'s **fleet-loop** FR-1
   propagation. The `FFleet` ship-loop `v->load()!=0` path and the `FPlayer`
   `m_unattached`/`m_destroyed` `v->load()!=0` paths (reachable e.g. via a corrupt
   in-ship weapon-type tag that makes `FVehicle::load()` return nonzero) are not
   each independently exercised. This is acceptable — the four sites are
   byte-identical mirror code, file-verified, and the Security/Verifier both judged
   the behavioral coverage sufficient for the security-sensitive behavior — so it
   is **not blocking**. Optional hardening of coverage is captured as **FF-3**.
4. **Redundant (harmless) double `hasMap()` check.** `drawFleets()` checks
   `FMap::hasMap()` and then calls `WXMapDisplay::getScale()`, which re-checks
   internally. Cheap, side-effect-free, single-threaded; no correctness or security
   impact. No action required.

### Missed functionality / edge cases
No plan-scoped functionality, integration point, cross-subtask expectation, or
required documentation obligation was missed. The two closed deferrals are
genuinely closed; the two guards and the four nested-return checks are complete
for their stated scope; both `AGENTS.md` notes and `doc/deferred-tasks.md` are
accurate. The residual items in NOTE 1–2 are explicitly out-of-scope, pre-existing
concerns already surfaced (NOTE 1) or acknowledged (NOTE 2) in the subtask
artifacts, not regressions or omissions of this feature.

---

## Follow-up feature requests for planning

- **FF-1 (save-input validation — out-of-range located-object IDs).** Add
  NULL-`FSystem*` guards (or upstream save-load validation of fleet/system/jump
  location IDs) so a valid `FMap` that cannot resolve an untrusted, out-of-range ID
  does not crash the gui draw path. Affected sites:
  `WXPlayerDisplay::drawFleets()` (`map->getSystem(location)->getCoord(...)`) and
  the analogous system/jump-route lookups in `WXMapDisplay`. Distinct from the
  now-closed `SF-nullfmap-paint-guard` (NULL map) — this is a valid-map,
  bad-ID validation gap. Recommend recording as a new deferred item and scoping a
  gui-module + save-input-validation follow-up. Traceable to FR-2 Security NOTE.
- **FF-2 (deepen the load aggregate-abort guarantee into `FVehicle::load()`).**
  Harden `FVehicle::load()` to check its own scalar-field reads and the nested
  `w->load()`/`d->load()` returns for mid-record truncation, returning nonzero on
  failure so FR-1's per-container abort guarantee extends to truncation strictly
  inside a single vehicle's own scalar/weapon/defense region. Traceable to the
  FR-1 scope boundary (note carried in the subtask artifacts). Recommend a strategic/ships
  save-load-hardening follow-up with matching behavioral coverage (truncate inside a
  vehicle's scalar region and assert `FGame::load()` aborts).
- **FF-3 (optional — complete FR-1 propagation-site coverage).** Add behavioral
  tests that drive the `FFleet` ship-loop and `FPlayer`
  `m_unattached`/`m_destroyed` nested-return propagation directly (e.g. a save with
  a corrupt in-ship weapon-type tag so `FVehicle::load()` returns nonzero), so all
  four FR-1 sites are individually exercised rather than relying on the
  mirror-code argument. Low priority.

---

## Final outcome

**PASS.**

Both subtasks deliver the governing plan's full Section 4 scope and satisfy every
acceptance criterion at file level; both closed deferrals are removed from
`doc/deferred-tasks.md`; documentation obligations (Doxygen `@author`/`@date`
bumps, accurate `AGENTS.md` notes) are met without overstatement; the FR-2 gui
paint path is independently confirmed root-cause-complete across all
`FMap`-touching seams of `WXGameDisplay::draw()`; and `make check` is green
(SSWTests 290 / TacticalTests 253 / GuiTests 85). No blocking or warning findings.
Three non-blocking follow-up feature requests (FF-1, FF-2, FF-3) are handed off to
the Planner; none gate this feature.
