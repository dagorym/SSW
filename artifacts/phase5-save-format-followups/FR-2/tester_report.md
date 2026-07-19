# Tester Report

Status:
- success

Task summary:
- Validated FR-2 pass-2 (Verifier-driven remediation closing the residual gui-draw-path gap in WXPlayerDisplay::drawFleets()). The implementer's already-committed fix (commit 7d679b5c) adds an `if (!FMap::hasMap()) { return; }` guard at the very top of drawFleets(), before `FMap *map = &(FMap::getMap())` is ever bound, so the FGamePanel::onPaint() -> WXGameDisplay::draw() chain is now root-cause-complete against a NULL/half-built FMap across both of its FMap-touching call paths (WXMapDisplay from pass-1, WXPlayerDisplay::drawFleets() from pass-2). Added one new behavioral test, testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash, that calls WXPlayerDisplay::drawFleets() directly against a real (non-mock) FPlayer/FFleet with a non-transit, nonzero getLocation() -- exactly the state that previously drove drawFleets() into binding a reference to a NULL FMap singleton and dereferencing it via map->getSystem(...) -- with FMap::hasMap() forced false, and asserts the call returns safely instead of crashing. FGame exposes no public API to add a player/fleet without first creating an FMap (both FGame::init() and FGame::load() call FMap::create()/load() before any player is constructed), so the fully-integrated fleet-bearing FGamePanel-repaint variant sketched in the handoff (option 1) is not constructible through public APIs; the focused direct-call variant (option 2 in the handoff) was used instead, and that unreachability is called out explicitly rather than silently substituted. All other acceptance criteria (unchanged rendering with a valid FMap, unchanged pass-1 guard and P5-5 mitigation, Doxygen update, doc/deferred-tasks.md staying clear of SF-nullfmap-paint-guard, green make check) were verified via the existing regression suite plus direct inspection of the already-committed implementer diff.

Branch name:
- phase5-FR-2-tester-20260719

Test commit hash:
- 10c6223e7e8e0ea249f159570b3f844026fe1ce3

Test files added or modified:
- tests/gui/StrategicGuiLiveTest.h
- tests/gui/StrategicGuiLiveTest.cpp

Commands run:
- make -C tests/gui (clean build, no warnings)
- xvfb-run -a ./GuiTests (run from tests/gui/) -> OK (85 tests)
- make check (top-level, from worktree root) -> SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests), exit 0

Pass/fail totals:
- GuiTests: 85 run / 0 failures (84 baseline + 1 new)
- SSWTests: 290 run / 0 failures (unchanged)
- TacticalTests: 253 run / 0 failures (unchanged)

Unmet acceptance criteria:
- None

Final test outcomes:
- make check (top-level) exit 0: SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests) -- no regressions vs. the FR-2 pass-1 baseline (290/253/84); the +1 in GuiTests is the new testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash case.
- New test testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash (tests/gui/StrategicGuiLiveTest.cpp) is fully behavioral: it defensively tears down any live FMap singleton, asserts !FMap::hasMap(), builds a real FPlayer and a real FFleet with a ship (createShip("AssaultScout")) and a non-transit, nonzero location set via FFleet::setLocation(unsigned int) (no FSystem/FMap dependency), adds the fleet to the player, then calls WXPlayerDisplay().drawFleets(dc, &player) on an offscreen wxMemoryDC and asserts the call returns (rendered.IsOk()) and FMap::hasMap() still reports false afterward -- reaching that final assertion is the proof the guard short-circuited before any FMap dereference.
- Confirmed the sibling pass-1 test testGamePanelRepaintWithNoLiveFMapDoesNotCrash (WXMapDisplay path via a player-less FGame) and the P5-5 test testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint (onOpen setGame-deferral mitigation) both still pass unmodified, confirming those two guards/mitigations are unchanged.
- Confirmed existing WXPlayerDisplay/WXGameDisplay rendering tests (testWXPlayerDisplayOffscreenRendersSystemAndTransitFleets, testWXGameDisplayOffscreenRendersTurnCounterAndIcons) still pass unmodified, confirming rendering behavior is unchanged when a valid FMap exists.
- Verified by direct source inspection (src/gui/WXPlayerDisplay.cpp, include/gui/WXPlayerDisplay.h, already committed by the Implementer at 7d679b5c) that the FMap::hasMap() guard is the very first statement in drawFleets(), strictly before `FMap *map = &(FMap::getMap())`, so no code path in drawFleets() can bind or dereference FMap::getMap() when hasMap() is false.
- Verified doc/deferred-tasks.md contains no SF-nullfmap-paint-guard entry (grep returns no matches) -- the item stays fully closed and was not re-deferred for the drawFleets path.
- Verified AGENTS.md's StrategicGuiLiveTest note (already updated by the Implementer) states the guard now covers both the WXMapDisplay path and the WXPlayerDisplay::drawFleets() path, and that the P5-5 FMainFrame::onOpen mitigation description is left intact.

Cleanup status:
- No temporary or non-handoff byproducts remain in the worktree; git status shows a clean working tree after the test commit.
- Rebuilt binaries/object files under src/ and tests/ are pre-existing, gitignored build outputs (not created by this run in a way that changes tracked state) and are not part of the handoff.

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/tester_report.md
- artifacts/phase5-save-format-followups/FR-2/tester_result.json
- artifacts/phase5-save-format-followups/FR-2/documenter_prompt.txt
