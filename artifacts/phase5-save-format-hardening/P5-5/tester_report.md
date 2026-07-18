# Tester Report

Status:
- success

Task summary:
- P5-5 remediation pass 2 (Verifier-driven): validated the sole approved in-scope mitigation from the pass-1 Security/Verifier review -- FMainFrame::onOpen() now defers m_drawingPanel->setGame(m_game) until AFTER FGame::load() succeeds, instead of immediately after FGame::create(). On a failed/corrupt load, resetGame() tears the freshly-created game down while the panel's own m_game pointer is still NULL, so a spontaneous FGamePanel repaint delivered during the load-error message dialog's nested modal loop can no longer dereference the now-NULL FMap singleton (the previously reachable FGamePanel::onPaint -> WXGameDisplay::draw -> WXMapDisplay::getScale -> FMap::getMaxSize() with this=0x0 crash). FGame.h/FGame.cpp, the magic/version gate, and aggregate load-error propagation from pass 1 were confirmed untouched.

Branch name:
- phase5-P5-5-tester-20260717

Test commit hash:
- 925ad377a5e68d2905e4f07154dab1a55e172334

Test files added or modified:
- tests/gui/StrategicGuiLiveTest.cpp
- tests/gui/StrategicGuiLiveTest.h

Commands run:
- make -j$(nproc)
- make check

Pass/fail totals:
- GuiTests: 83 tests, OK
- SSWTests: 289 tests, OK
- TacticalTests: 253 tests, OK

Unmet acceptance criteria:
- None

Final test outcomes:
- make check: green across all three suites -- SSWTests (289 tests, OK), TacticalTests (253 tests, OK), GuiTests (83 tests, OK, run under xvfb-run -a).
- Existing testMainFrameOnOpenFailedLoadResetsGameAndLeavesMenuItemsDisabled (unshown frame) and testMainFrameOnOpenConfirmLoadsFromFullPathAndRestoresPostLoadState (success path, frame shown) both remain unchanged and pass, confirming the success path and prior failed-load menu-state coverage are untouched by this pass.
- New behavioral test testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint (tests/gui/StrategicGuiLiveTest.cpp/.h) shows the FMainFrame (unlike its sibling, which deliberately avoids Show() to sidestep the pre-mitigation crash) and drives a new RepaintPumpTimer helper that repeatedly calls frame->Refresh() every 5ms across onOpen()'s entire nested modal sequence (native file chooser, then the load-error wxGenericMessageDialog) while opening a corrupt/malformed save file. The test asserts the process survives (no crash) and that all seven game-dependent menu items remain disabled afterward, directly exercising the crash window described in the implementer's report and proving it is closed by the setGame()-deferral remediation.
- GuiTests suite count increased by exactly 1 matching the single new test added; no existing test assertions were changed.

Cleanup status:
- No temporary byproducts were left in the worktree: build artifacts (.o/.a/binaries) are git-ignored and were not staged or committed; git status is clean after the test commit.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-5/tester_report.md
- artifacts/phase5-save-format-hardening/P5-5/tester_result.json
- artifacts/phase5-save-format-hardening/P5-5/documenter_prompt.txt
