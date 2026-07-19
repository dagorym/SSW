# Implementer Report

Status:
- success

Task summary:
- Add the root-cause NULL/invalid-FMap guard in the gui draw chain (closes deferred item SF-nullfmap-paint-guard / reviewer FR-2). FMap::getMap() returns *m_map, and the private static m_map can be NULL while a FGame is set, so WXMapDisplay::getScale()/draw() (reached via FGamePanel::onPaint -> WXGameDisplay::draw) could dereference a null reference and call FMap::getMaxSize() with this=0x0. Added a new public static predicate FMap::hasMap() (returns m_map != NULL, tests existence without dereferencing) and consulted it in WXMapDisplay::draw()/getScale() before any call to FMap::getMap(): draw() now returns immediately (no-op, draws nothing) and getScale() returns a safe default of 1.0 when no map exists, instead of crashing. No behavior change when a valid FMap exists. This is defense-in-depth in the draw path itself, independent of and in addition to the existing P5-5 FMainFrame::onOpen setGame-deferral mitigation, which was left completely unchanged.

Changed files:
- src/gui/WXMapDisplay.cpp
- include/gui/WXMapDisplay.h
- src/strategic/FMap.cpp
- include/strategic/FMap.h
- doc/deferred-tasks.md
- AGENTS.md

Validation commands run:
- make -C src/strategic
- make -C src/gui
- make -C . check (top-level: builds all + runs SSWTests, TacticalTests, GuiTests under xvfb-run)

Validation outcome:
- src/strategic and src/gui build clean with no new warnings. Top-level `make check` completed with exit code 0: SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (83 tests). Test counts match the current worktree baseline (290 reflects FR-1's already-merged new test; TacticalTests/GuiTests counts are unchanged from the Phase 5 reviewer baseline), confirming no regression from this change. No behavioral test for the new FMap::hasMap() guard was added by this run since authoring new/updated tests is the Tester's responsibility (tests/gui/StrategicGuiLiveTest.{h,cpp}).

Implementation/code commit hash:
- 09cc698a

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/implementer_report.md
- artifacts/phase5-save-format-followups/FR-2/tester_prompt.txt
- artifacts/phase5-save-format-followups/FR-2/implementer_result.json

Implementation context:
- Fix locations: include/strategic/FMap.h declares `static bool hasMap();` (new); src/strategic/FMap.cpp implements it as `bool FMap::hasMap(){ return m_map != NULL; }` immediately after FMap::getMap(). src/gui/WXMapDisplay.cpp: WXMapDisplay::draw(wxDC&) now begins with `if (!FMap::hasMap()) { return; }` before its first `getScale(dc)`/`FMap::getMap()` call; WXMapDisplay::getScale(wxDC&) now begins with `if (!FMap::hasMap()) { return 1.0; }` before its own `FMap::getMap()` call. Neither WXGameDisplay.cpp/.h nor FMainFrame.cpp/FGamePanel.cpp were touched -- the guard is fully contained in WXMapDisplay per the plan's allowed-file list, which is sufficient because WXGameDisplay::draw() only ever reaches FMap through WXMapDisplay::draw().
- Why a non-positive-getMaxSize() guard would NOT have worked (and was explicitly rejected per plan Assumption A1): FMap::getMap() returns `*m_map` (a reference), so merely calling `map.getMaxSize()` and checking its sign still dereferences the NULL m_map first -- the crash already happened by the time any value could be inspected. FMap::hasMap() tests `m_map` itself (the pointer, not a value obtained by dereferencing it), so it is safe to call unconditionally, including before FMap has ever been created.
- The P5-5 FMainFrame::onOpen() setGame-deferral mitigation (src/FMainFrame.cpp) was NOT touched by this change and remains exactly as it was -- this guard is purely additive defense-in-depth at a lower layer.
- TESTER GUIDANCE -- how to reach the paint path with a NULL map (this is the key mechanism to exercise the new guard; the outer FGamePanel::onPaint() `if (m_game != NULL)` guard must NOT be the thing that blocks the repaint, or the new inner guard is never reached): FGame::create() (no-arg overload, src/strategic/FGame.cpp lines 23-25) constructs the FGame singleton via `FGame::FGame(IStrategicUI*)`, whose constructor sets `m_universe = NULL` and does nothing else map-related -- FMap::create(...) is only ever called from FGame's setup/newGame flow (e.g. createTFCassidine()/createSFNova() etc.), never from FGame::create() itself. So `FGame * game = &(FGame::create());` on its own gives you a live, non-NULL FGame with NO FMap side effect at all. Passing that game to `FGamePanel::setGame(game)` (include/FGamePanel.h:77, public) makes `FGamePanel::onPaint()`'s `if (m_game != NULL)` guard pass through to `m_gameDisplay.draw(dc, *m_game)` -> `WXGameDisplay::draw()` -> `WXMapDisplay::draw()`, which is exactly where the new `FMap::hasMap()` check now intercepts the call.
- FMap is itself a singleton (like FGame): `FMap::hasMap()` reflects whether `FMap::create(...)` has been called and not yet deleted since process start. Existing sibling tests in tests/gui/StrategicGuiLiveTest.cpp (testWXGameDisplayOffscreenRendersTurnCounterAndIcons() and the FMap-rendering test above it, both using `ensureFrontierMap(...)` at file line ~122) already follow a create-then-`delete &map;` / `delete game;` per-test convention, so `FMap::hasMap()` should read false between tests absent a bug -- but do not blindly assert that precondition without handling it defensively (e.g. if a prior test left a live map, delete it first via `delete &FMap::getMap();` to establish the false precondition deterministically before proceeding), since CppUnit does not guarantee execution order across the whole suite and this is a process-wide static.
- Recommended new test shape (following the existing file's idioms, e.g. testGamePanelPaintTracksParentSize() at line ~1380 and testWXGameDisplayOffscreenRendersTurnCounterAndIcons() at line ~1356): do NOT call ensureFrontierMap() or otherwise create an FMap in this test at all -- only construct `FGame * game = &(FGame::create());`, a `wxFrame`/`FGamePanel` pair (or reuse `renderOffscreen(...)` directly against `WXGameDisplay::draw(dc, *game)` for a narrower, non-GUI-runtime-dependent variant), call `panel->setGame(game)`, force a repaint (`panel->Refresh(); panel->Update(); m_harness.pumpEvents(...)` for the live-frame variant, or just invoke `WXGameDisplay draw` directly against an offscreen `wxMemoryDC` for the narrower variant matching the AGENTS.md-preferred rendering-test pattern for WXGameDisplay/WXMapDisplay), and assert the test reaches its final CPPUNIT_ASSERT without crashing -- matching the existing sibling P5-5 test's convention where 'reaching this assertion at all is the primary behavioral proof.' Clean up by deleting the FGame singleton at the end (`delete game;`) and do not leave a live FMap behind for later tests, since none was created.
- Before this fix, forcing that repaint (FGamePanel/WXGameDisplay with a live FGame but no live FMap) crashed the process by dereferencing FMap::getMap()'s null reference inside WXMapDisplay::getScale()/draw() and then calling FMap::getMaxSize() with this=0x0 -- the exact chain documented in doc/deferred-tasks.md's now-removed SF-nullfmap-paint-guard entry and in AGENTS.md's StrategicGuiLiveTest notes. After this fix, WXMapDisplay::draw() returns immediately and WXMapDisplay::getScale() returns 1.0, so the repaint completes safely with no map content drawn.
- Doxygen: include/strategic/FMap.h added a full Doxygen block for the new hasMap() predicate (author Claude Sonnet 5 (medium), Created/Last Modified Jul 19, 2026) and bumped the file header's Last Modified date. include/gui/WXMapDisplay.h and src/gui/WXMapDisplay.cpp had their draw()/getScale() Doxygen blocks updated to describe the new guard (author list extended to include Claude Sonnet 5 (medium), Last Modified bumped to Jul 19, 2026) and their file headers' author/date likewise updated.
- doc/deferred-tasks.md: the SF-nullfmap-paint-guard entry was removed entirely, following the repository's established precedent of fully removing (not merely annotating) resolved deferred-task entries (matching FR-1's SF-nested-load-returns removal and the prior F2-serialization/H2 entries).
- AGENTS.md: the StrategicGuiLiveTest notes paragraph describing the onOpen() setGame-deferral mitigation was updated so its closing sentence no longer states the root-cause gui-paint-path guard 'remains a tracked follow-up'; it now names FMap::hasMap() and the WXMapDisplay::draw()/getScale() guard as the closing root-cause fix, while explicitly preserving the description of the unchanged onOpen() mitigation.

Expected validation failures carried forward:
- None
