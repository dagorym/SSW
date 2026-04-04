# Reviewer Report — Milestone 8 Remediation

## Reviewer activation
- Requested agent: `reviewer`
- Repository-local definition found: no
- Shared definition found: yes
- Definition path used: `/home/tstephen/repos/agents/agents/reviewer.yaml`
- Precedence decision: shared reviewer definition applied because no repository-local reviewer definition exists in this repo.
- Review mode: feature-level, read-only review with artifact-only output in `artifacts/milestone8-remediation`

## Feature plan reviewed
- `plans/milestone8-remediation-plan.md`

## Inputs reviewed
- Subtask artifact sets under `artifacts/milestone8-remediation/m8r-subtask-{1..10}`, including implementer, tester, documenter, and verifier reports/results.
- Current repository state for the remediated runtime seams and validation/docs:
  - `src/tactical/FBattleDisplay.cpp`
  - `src/tactical/FBattleScreen.cpp`
  - `src/tactical/FTacticalGame.cpp`
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/tactical/FTacticalBattleScreenDelegationTest.cpp`
  - `tests/tactical/FTacticalMineDamageFlowTest.cpp`
  - `doc/DesignNotes.md`

## Overall feature completeness
The feature is substantially complete. The original blocking runtime regressions called out in the Milestone 8 reviewer output are fixed in the current branch state: the live move-done callback now routes through `FBattleScreen::completeMovePhase()` to `FTacticalGame::completeMovePhase()` (`src/tactical/FBattleDisplay.cpp:491-497`, `src/tactical/FTacticalGame.cpp:1449-1497`), and the destroyed-ship cleanup contract now preserves model-captured IDs until wx-side cleanup consumes them (`src/tactical/FBattleScreen.cpp:463-490`, `include/tactical/FTacticalGame.h:173-196`). The documentation also now distinguishes shipped Milestone 8 behavior from remediation and accurately separates wx delegation coverage from direct canonical model coverage (`doc/DesignNotes.md:626-645`).

The delivered subtasks collectively satisfy the main remediation goal: restore correct delegated destroyed-ship cleanup, restore canonical move-finalization routing on the live path, and validate/document the repaired seams. Tactical validation was rerun in Subtask 10 with `cd tests/tactical && make clean && make && ./TacticalTests`, producing `OK (69 tests)` per `artifacts/milestone8-remediation/m8r-subtask-10/verifier_report.md`.

## Findings

### BLOCKING
- No blocking findings identified.

### WARNING
1. `FBattleScreen::setPhase(PH_FINALIZE_MOVE)` still preserves a legacy screen-side finalize branch, and the tactical delegation test still locks that branch in as expected behavior.
   - Plan gap: Subtask 6 acceptance called for `FTacticalGame::completeMovePhase()` to remain the canonical post-move seam and for duplicated UI-side move-finalization logic not to remain on the path.
   - Evidence:
     - `src/tactical/FBattleScreen.cpp:196-203`
     - `tests/tactical/FTacticalBattleScreenDelegationTest.cpp:200-205`
     - `artifacts/milestone8-remediation/m8r-subtask-6/verifier_report.md`
   - Why it matters: the repaired live callback path is correct, but this adjacent helper seam still duplicates part of move finalization and increases the chance that future edits reintroduce split finalize behavior.

2. The selected-ship cleanup branch inside `FBattleScreen::clearDestroyedShips()` is still not directly pinned by tactical coverage.
   - Plan relevance: Subtask 7 aimed to replace broken fire-flow source assertions with runtime seam coverage that protects the corrected cleanup lifecycle.
   - Evidence:
     - `src/tactical/FBattleScreen.cpp:472-482`
     - `tests/tactical/FTacticalBattleScreenDelegationTest.cpp:211-218`
     - `artifacts/milestone8-remediation/m8r-subtask-3/verifier_report.md`
   - Why it matters: if the currently selected ship is destroyed during delegated fire cleanup, the screen now clears that UI selection correctly, but the suite does not directly fail if that branch regresses.

### NOTE
1. The accepted non-blocking limitation remains unchanged: there is still no end-to-end automated GUI playthrough, but the tactical seam coverage and Subtask 10 validation are sufficient for this remediation scope.
   - Evidence:
     - `artifacts/milestone8-remediation/m8r-subtask-10/tester_report.md`
     - `artifacts/milestone8-remediation/m8r-subtask-10/verifier_report.md`
     - `doc/DesignNotes.md`

## Missed functionality and edge-case coverage discussion
- I did not find evidence that the two original blocking runtime bugs remain. The repaired live runtime path is present in the code and is backed by tactical coverage split appropriately between wx delegation checks and direct canonical outcome checks.
- The remaining gaps are follow-on quality issues rather than feature blockers:
  - one neighboring finalize helper still duplicates model work outside the preferred canonical seam;
  - one newly introduced UI-state cleanup branch lacks direct regression coverage.

## Follow-up feature requests for planning
- Remove the remaining legacy `FBattleScreen::setPhase(PH_FINALIZE_MOVE)` screen-side `finalizeMovementState()` behavior, route that helper consistently through the canonical `FTacticalGame::completeMovePhase()` seam, and update `FTacticalBattleScreenDelegationTest` and any related documentation to reflect the unified contract.
- Add tactical regression coverage for `FBattleScreen::clearDestroyedShips()` when the currently selected ship appears in `getLastDestroyedShipIDs()`, asserting selection clearing, redraw behavior, single bookkeeping clear, and winner handling across the delegated fire cleanup seam.

## Final outcome
- `CONDITIONAL PASS`

The feature now meets the remediation's blocking runtime objectives and is safe to treat as substantially complete, but the two warning-level follow-ons above should be planned to fully align adjacent helper seams and regression coverage with the intended contract.
