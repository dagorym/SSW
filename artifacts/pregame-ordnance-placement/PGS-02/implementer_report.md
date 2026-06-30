# Implementer Report

Status:
- success

Task summary:
- PGS-02: Fix mine/seeker placement source rows so they are reliably visible and clickable during BS_PlaceMines and BS_PlaceSeekers phases.

Changed files:
- src/tactical/FBattleDisplay.cpp

Validation commands run:
- make -C tests tactical-tests && ./tests/tactical/TacticalTests
- make -C tests/gui && xvfb-run -a ./tests/gui/GuiTests

Validation outcome:
- tactical tests: 215/215 PASSED. GUI tests: build PASSED; xvfb-run not available so runtime tests were not executed in this environment.

Implementation/code commit hash:
- 8627871

Artifacts written:
- artifacts/pregame-ordnance-placement/PGS-02/implementer_report.md
- artifacts/pregame-ordnance-placement/PGS-02/tester_prompt.txt
- artifacts/pregame-ordnance-placement/PGS-02/implementer_result.json

Implementation context:
- Root cause: drawPlaceMines() and drawPlaceSeekers() draw source rows starting near y=99-114px on a 120px-tall panel. applyRequestedDisplayHeight() was calling SetMinSize() but FBattleScreen::applyLayoutPolicy() was never notified, so the panel stayed at 120px and all rows were clipped.
- Fix: applyRequestedDisplayHeight() now calls GetParent()->SendSizeEvent() whenever the min height changes (guarded by m_inResizeReflow to prevent recursive loops). This triggers FBattleScreen::onSize -> applyLayoutPolicy() -> Layout() on the next event-loop iteration, growing the panel to show all rows.
- Click regions (m_shipNameRegions) and draw positions are both recalculated every paint pass from the same getActionButtonRowBottom() call, so alignment is preserved once the panel is the correct height.
- Single-source auto-selection is handled in FTacticalGame::beginMinePlacement/beginSeekerPlacement (not changed). The first paint may still show clipped rows briefly, but the panel grows and repaints before user input is possible.
- Changed file: src/tactical/FBattleDisplay.cpp - applyRequestedDisplayHeight() only (8 lines added).

Expected validation failures carried forward:
- None
