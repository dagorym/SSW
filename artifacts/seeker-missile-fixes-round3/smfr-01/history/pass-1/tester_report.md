# Tester Report

Status:
- failure

Task summary:
- Fix lower-panel list-widget visibility so the placed-ordnance removal list (mine and seeker placement phases) and the seeker activation/deactivation list render in the visible lower-panel middle region instead of being clipped below the ship-status display. Apply the same panel-height auto-expansion approach already used by drawOffensiveSeekerPendingRows() to drawPlaceMines(), drawPlaceSeekers(), and drawSeekerActivation().

Branch name:
- sf2-SMFR-01-tester-20260619

Test commit hash:
- 74938b9

Test files added or modified:
- tests/gui/TacticalGuiLiveTest.cpp
- tests/gui/TacticalGuiLiveTest.h
- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp
- tests/tactical/FTacticalBattleDisplayFireFlowTest.h

Commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests
- make -C tests gui-tests
- DISPLAY=:0 ./tests/gui/GuiTests

Pass/fail totals:
- None

Unmet acceptance criteria:
- {'criterion': 'drawSeekerActivation() reserves vertical space so rendered rows are fully visible', 'expected': 'After drawSeekerActivation() draws activation list rows extending below the initial 120-px minimum, requestedDisplayHeight must reflect the expanded height so GetMinSize().GetHeight() accommodates all rows.', 'actual': 'drawSeekerActivation() correctly computes activationListBottom = y + BORDER and updates requestedDisplayHeight. However, drawCurrentShipStats() is called immediately after in draw() for BS_Battle state and calls setLowerPanelState() which unconditionally overwrites requestedDisplayHeight with a stats-only height, discarding the expansion. Activation list rows render below the panel minimum height.', 'implementation_defect': 'In src/tactical/FBattleDisplay.cpp draw() for BS_Battle: drawSeekerActivation(dc) expands requestedDisplayHeight, then drawCurrentShipStats(dc) calls setLowerPanelState() (line 582) which overwrites requestedDisplayHeight with max(buttonRowHeight, statsBottom), ignoring the activation list expansion. Fix: setLowerPanelState() should preserve a previously-expanded requestedDisplayHeight, OR drawSeekerActivation() should run AFTER drawCurrentShipStats() so it can expand on top of the stats-based height.'}

Final test outcomes:
- tactical_tests
- gui_tests

Cleanup status:
- None

Artifacts written:
- artifacts/seeker-missile-fixes-round3/smfr-01/tester_report.md
- artifacts/seeker-missile-fixes-round3/smfr-01/tester_result.json
