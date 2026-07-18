# Implementer Report

Status:
- success

Task summary:
- P5-5 remediation pass 2 (Verifier-driven): apply the single approved in-scope mitigation from the pass-1 Security/Verifier review -- restructure FMainFrame::onOpen so the drawing panel is only wired to the freshly-created FGame (m_drawingPanel->setGame(m_game)) AFTER FGame::load() succeeds, instead of immediately after FGame::create(). On a failed/corrupt load, resetGame() now tears the freshly-created game down while the panel's m_game pointer is still NULL, so a spontaneous FGamePanel repaint delivered during the load-error message dialog's nested modal loop can no longer dereference the now-NULL FMap singleton (FGamePanel::onPaint -> WXGameDisplay::draw -> WXMapDisplay::getScale -> FMap::getMaxSize() with this=0x0). This does not change FGame.h/FGame.cpp, the magic/version gate, or aggregate load-error propagation, all of which were already correct from pass 1; it only reorders when the panel is associated with the game inside onOpen(). The success path is unchanged: setGame() still runs, immediately followed by the existing Refresh()/menu-enable/turn-state logic.

Changed files:
- src/FMainFrame.cpp
- include/FMainFrame.h

Validation commands run:
- make -C <worktree-root> all_clean
- make -C <worktree-root> (top-level build)
- make -C <worktree-root> check (top-level: full build then SSWTests, TacticalTests, GuiTests under xvfb-run)

Validation outcome:
- make check is green across all three suites, exit 0: SSWTests OK (289 tests), TacticalTests OK (253 tests), GuiTests OK (82 tests, under xvfb-run). The existing StrategicGuiLiveTest::testMainFrameOnOpenFailedLoadResetsGameAndLeavesMenuItemsDisabled (failed-load onOpen path) and StrategicGuiLiveTest::testMainFrameOnOpenConfirmLoadsFromFullPathAndRestoresPostLoadState (success onOpen path) both still pass unchanged. No test files were added or modified by the implementer in this pass (test-authoring is Tester-owned); the mitigation is a pure reordering inside FMainFrame::onOpen with no new branches or return codes, so no existing test's expected control-flow path changed.

Implementation/code commit hash:
- c28c489ad813691159cc7c8b537a17fe02247b96

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-5/implementer_report.md
- artifacts/phase5-save-format-hardening/P5-5/tester_prompt.txt
- artifacts/phase5-save-format-hardening/P5-5/implementer_result.json

Implementation context:
- src/FMainFrame.cpp: in FMainFrame::onOpen(), the line `m_drawingPanel->setGame(m_game);` that previously ran directly after `m_game = &(FGame::create(m_strategicUI));` was removed from that position. It now runs only inside the `else` branch of `if (!is.is_open() || m_game->load(is) != 0) { resetGame(); } else { m_drawingPanel->setGame(m_game); ... }`, i.e. only after a confirmed-successful load.
- include/FMainFrame.h: the Doxygen comment on onOpen() was updated to describe the new setGame()-deferral ordering and why it matters (no spontaneous repaint of a not-yet-loaded/half-built game during the load-error dialog's nested modal loop).
- FGamePanel::onPaint() (src/FGamePanel.cpp, untouched) already guards its draw call with `if (m_game != NULL)`; the mitigation relies on that pre-existing guard by simply keeping the panel's m_game pointer NULL until load() has succeeded, so no FGamePanel/WXGameDisplay/WXMapDisplay code needed to change.
- resetGame() (src/FMainFrame.cpp, unchanged) still unconditionally calls m_drawingPanel->clearGame(); on the failure path this is now a no-op (the panel's m_game was never set), which is harmless and keeps resetGame() usable from its other call sites (onNew() failure path, onClose()) unchanged.
- This is a minimal, localized reordering: no new member variables, no new control-flow branches, no change to onOpen()'s existing is_open()/load() return-code check or to the resetGame()/success-branch structure introduced in P5-5 pass 1.
- Two follow-up defects remain out of scope for this pass and must be recorded by the Documenter in doc/deferred-tasks.md: (1) FPlayer::load()/FFleet::load() ignore some sub-object load() return codes (src/strategic/FPlayer.cpp and src/strategic/FFleet.cpp), so a deep-truncation-inside-fleet input can silently produce a logically-inconsistent game instead of tripping FGame::load()'s aggregate abort -- this is a P5-3-scope gap, not touched here; (2) the deeper root-cause NULL-FMap paint guard belongs in the gui module (FGamePanel::onPaint / WXGameDisplay::draw / WXMapDisplay::getScale) and is still not fixed -- this pass's mitigation only prevents the panel from being associated with an unloaded game during onOpen(), it does not add a NULL guard to the paint path itself, so any other code path that could show a live FGamePanel with a NULL/partially-built FMap remains unguarded.

Expected validation failures carried forward:
- None
