# Tester Report (Pass-2 Remediation)

## Status
success

## Task Summary
TMF-06: Skip post-fire damage-summary dialog when no weapon actually fired.

Production fix (pass-1): added `weaponsFired` field to `FTacticalCombatReportSummary`, populated from `report.attacks.size()` in `buildTacticalCombatReportSummary`. Guarded `showTacticalDamageSummaryDialog` in both `onDefensiveFireDone` and `onOffensiveFireDone` with `if (summary.weaponsFired > 0)`.

Pass-2 remediation: addressed Verifier WARNING that AC2/AC3 dialog show/skip had no direct behavioral observation of the dialog suppression path. Added behavioral GUI tests that drive the actual wx event handlers with a `CountingMockTacticalUI` intercepting the `ITacticalUI::showDamageSummary` seam, directly observing that the count does not increase when the guard suppresses the call.

## Branch Name
tmf-TMF-06-tester-20260630

## Test Commit Hash
f9a4e51

## Test Files Added or Modified
- tests/gui/TacticalGuiLiveTest.h — added `testDefensiveFireDoneSkipsDialogWhenNoWeaponsFired` and `testOffensiveFireDoneSkipsDialogWhenNoWeaponsFired` declarations and Doxygen comments; added CPPUNIT_TEST entries to suite
- tests/gui/TacticalGuiLiveTest.cpp — added `#include "tactical/ITacticalUI.h"`; added `CountingMockTacticalUI` and `FireDoneObserverBattleScreen` classes in anonymous namespace; added two test implementations

## New Behavioral Coverage (Pass-2)

### CountingMockTacticalUI
Implements `ITacticalUI` with a counter on `showDamageSummary`. All other methods are safe no-ops. Installed on `m_tacticalGame` in `FireDoneObserverBattleScreen` so that `FBattleScreen::showTacticalDamageSummaryDialog` routes through the mock instead of a real wx dialog.

### FireDoneObserverBattleScreen
Subclasses `FBattleScreen`. After base construction (which installs WXTacticalUI), replaces the installed ITacticalUI with `CountingMockTacticalUI` and sets state to `BS_Battle` so the fire-phase draw paths are reachable.

### testDefensiveFireDoneSkipsDialogWhenNoWeaponsFired
1. Creates `FireDoneObserverBattleScreen`, sets phase to `PH_DEFENSE_FIRE`, shows it
2. Draws directly with `wxMemoryDC` on the `FBattleDisplay` child to trigger `drawDefensiveFire()` which connects `m_buttonDefensiveFireDone` via the `m_first` flag path
3. Finds the button by label, force-enables it
4. Fires `wxCommandEvent` programmatically via `GetEventHandler()->ProcessEvent`
5. Asserts `CountingMockTacticalUI::showDamageSummaryCallCount` did NOT increase
6. With empty game (no ships loaded), `fireAllWeapons()` returns `weaponsFired == 0` and the guard suppresses the dialog call — PASS

### testOffensiveFireDoneSkipsDialogWhenNoWeaponsFired
Mirror of defensive test: sets `PH_ATTACK_FIRE`, triggers draw to connect `m_buttonOffensiveFireDone`, fires click, asserts count did not increase — PASS

## Commands Run
- `make -C tests/gui` (build GUI tests)
- `DISPLAY=:0 tests/gui/GuiTests` (run all GUI tests)
- `make -C tests tactical-tests` (rebuild tactical tests from clean)
- `tests/tactical/TacticalTests` (run all tactical tests)

## Pass/Fail Totals
- GuiTests: 57/65 (8 pre-existing failures — same as pass-1 baseline; +2 new tests both PASS)
- TacticalTests: 249/249 PASS (no regressions)

## Acceptance Criteria Coverage

| AC | Description | Coverage | Status |
|----|-------------|----------|--------|
| AC1 | `weaponsFired` populated correctly (0 for empty, N for N fired incl. all-miss) | Behavioral: `FTacticalCombatReportTest` (pass-1, retained) | PASS |
| AC2 | No weapon fired → dialog skipped; >=1 fired → dialog shown | **Behavioral (NEW)**: `testDefensiveFireDoneSkipsDialogWhenNoWeaponsFired` and `testOffensiveFireDoneSkipsDialogWhenNoWeaponsFired` directly observe zero dialog invocations through ITacticalUI mock; source-contract supplements retained in `FTacticalBattleDisplayFireFlowTest` | PASS |
| AC3 | Mine-damage path (`ITacticalUI::showDamageSummary` directly) unaffected; `FTacticalGame.h/.cpp` unmodified | Source-contract: mine path unchanged; `FTacticalGame.cpp` not modified | PASS |

## Coverage Scope Note (AC2 Limitation Disclosure)
The new behavioral tests observe the zero-weapons case (dialog suppressed). The positive case (dialog invoked when weaponsFired >= 1) is not directly exercised in the new tests because setting up an empty-game fire phase that returns a non-zero `weaponsFired` requires ship-population infrastructure not available without a full scenario load. The positive show path is validated indirectly by the existing `testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior` test in `TacticalGuiLiveTest` (which exercises the live WXTacticalUI showDamageSummary path through `WXTacticalUI` directly) combined with the `FTacticalCombatReportTest` behavioral tests confirming `weaponsFired` is set correctly for non-empty reports.

## Unmet Acceptance Criteria
None.

## Pre-Existing Failures (Unchanged)
All 8 GuiTests failures are pre-existing source-contract or geometry tests unrelated to TMF-06:
1. testSetupPlacementSourceRowsAndOrdnanceColorContracts
2. testSeekerActivationPanelSourceContracts
3. testBattleDisplayNarrowWidthStacksShipStatsBelowButtons
4. testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo
5. testBattleSimGuiLiveTest (BattleSimGuiLiveTest)
6. Three additional source-contract tests unchanged from pass-1 baseline

## Cleanup Status
No temporary non-handoff byproducts left in the worktree.

## Artifacts Written
- artifacts/tactical-misc-fixes/TMF-06/tester_report.md
- artifacts/tactical-misc-fixes/TMF-06/tester_result.json
- artifacts/tactical-misc-fixes/TMF-06/documenter_prompt.txt
