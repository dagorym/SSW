Security Review Report

Scope reviewed:
- Committed FR-2 changes (diff 4c668207..1242446c) closing deferred item SF-nullfmap-paint-guard, part of plans/phase5-save-format-followups-plan.md.
- Code reviewed: include/strategic/FMap.h + src/strategic/FMap.cpp (new public static predicate FMap::hasMap()); include/gui/WXMapDisplay.h + src/gui/WXMapDisplay.cpp (hasMap() guards in draw() and getScale()); tests/gui/StrategicGuiLiveTest.{h,cpp} (new no-crash forced-repaint test); doc/deferred-tasks.md and AGENTS.md (docs only).
- VERIFIED: FMap::hasMap() returns (m_map != NULL) and never dereferences the pointer (src/strategic/FMap.cpp:36-38); safe to call before FMap is ever created and after deletion because ~FMap resets the static m_map=0 (src/strategic/FMap.cpp:60). Single-rooted static singleton, single-threaded wx GUI, so no lifetime/race concern.
- VERIFIED: Both WXMapDisplay::draw() (src/gui/WXMapDisplay.cpp:34-36) and WXMapDisplay::getScale() (src/gui/WXMapDisplay.cpp:96-98) consult FMap::hasMap() BEFORE their first FMap::getMap() call; draw() also only calls getScale() after its own guard, so the documented crash (FGamePanel::onPaint -> WXGameDisplay::draw -> WXMapDisplay::getScale -> FMap::getMaxSize() with this=0x0) is fully closed for these two functions.
- VERIFIED: No behavior change when a valid FMap exists -- the guards are pure early-out short-circuits added ahead of unchanged rendering logic; normal rendering is not weakened.
- VERIFIED: getScale()'s 1.0 default introduces no downstream divide-by-zero or invalid use -- the only site that divides by scale (FMainFrame::onLeftDClick, src/FMainFrame.cpp:259-263) guards `if (scale > 0.0)` and 1.0 flows into m_game->handleMapClick(), which itself guards `if (m_universe != NULL)` (src/strategic/FGame.cpp:442); drawing nothing is inherently safe.
- VERIFIED: FMainFrame::onOpen setGame-deferral mitigation (P5-5) is left unchanged -- src/FMainFrame.cpp is not in the FR-2 diff.
- VERIFIED: FR-2 introduces no new unsafe path; the change is add-only guards plus a non-dereferencing predicate. Panel dangling-pointer safety is preserved (FMainFrame::resetGame calls m_drawingPanel->clearGame() before delete m_game, src/FMainFrame.cpp:339-340).

Why specialist review was triggered:
- The crash closed by FR-2 is reachable from untrusted input: opening a corrupt/malicious .ssw save file can leave a live FGame singleton whose FMap was never built, and a subsequent FGamePanel repaint would dereference FMap::getMap()'s null reference (calling FMap::getMaxSize() with this=0x0), crashing the process (availability / NULL-pointer-dereference).
- FR-2 is the root-cause defense-in-depth guard in the paint path itself, complementing the P5-5 onOpen setGame-deferral mitigation that closed the one then-known trigger.
- Security review was explicitly requested by the plan because the guard is the trust-boundary defense for the gui paint path against untrusted save input.

Acceptance criteria / plan reference:
- plans/phase5-save-format-followups-plan.md (FR-2; closes SF-nullfmap-paint-guard from doc/deferred-tasks.md).
- P5-5 pass-2 precedent: FMainFrame::onOpen setGame-deferral; AGENTS.md save-format / gui paint-guard notes.

Findings

BLOCKING
- None

WARNING
- src/gui/WXPlayerDisplay.cpp:28 - WXGameDisplay::draw() reaches FMap through a SECOND, unguarded path -- WXPlayerDisplay::drawFleets() -- so the claim that the draw chain reaches FMap only via WXMapDisplay::draw(), and that SF-nullfmap-paint-guard is fully closed, is overstated.
  WXGameDisplay::draw() (src/gui/WXGameDisplay.cpp:37) calls playerDisplay.drawFleets(dc, *itr), which calls FMap::getMap() directly (WXPlayerDisplay.cpp:28) and later dereferences the returned map pointer via map->getJumpRoute() (line 46) and map->getSystem() (lines 61-62, 81-82) WITHOUT any FMap::hasMap() guard. FR-2 guarded only WXMapDisplay::draw()/getScale(). If FMap::m_map were NULL while a player held a fleet, map->getSystem() would dereference this=NULL and crash -- the exact bug class SF-nullfmap-paint-guard set out to eliminate for the whole FGamePanel/WXGameDisplay draw chain. Non-blocking today ONLY because the codebase maintains an implicit invariant that fleets exist only when the FMap exists (initMap runs before initFleets in FGame::init, src/strategic/FGame.cpp:98 vs 111; FMap::create precedes player/fleet load in FGame::load, line 698 vs 708), so the per-fleet loop is empty whenever m_map is NULL. Because that reachability protection is an implicit ordering invariant rather than a paint-path guard, recommend (a) adding `if (!FMap::hasMap()) return;` at the top of WXPlayerDisplay::drawFleets() (or bailing when the map pointer is null) to make the trust boundary root-cause-complete, and/or (b) narrowing the AGENTS.md / deferred-tasks.md closure wording to state the guard covers the WXMapDisplay portion of the draw chain.

NOTE
- src/gui/WXPlayerDisplay.cpp:28 - `FMap *map = &(FMap::getMap());` binds a reference to *m_map and takes its address; when m_map is NULL this is technically undefined behavior (reference bound to a dereferenced null pointer).
  In practice this compiles to a plain pointer copy (no memory load) so `map` becomes NULL without a fault, and it predates FR-2 (FR-2 did not touch this file). It is only dereferenced inside the per-fleet loop, which is empty when no map/fleets exist. Flagged for awareness; the WARNING-finding guard would also resolve it cleanly.
- tests/gui/StrategicGuiLiveTest.cpp:1400 - The new behavioral test testGamePanelRepaintWithNoLiveFMapDoesNotCrash uses a player-less FGame::create(), so WXGameDisplay::draw()'s empty player loop never invokes WXPlayerDisplay::drawFleets(); the drawFleets FMap sub-path is not exercised by any no-map repaint test.
  The test correctly and directly proves the WXMapDisplay::draw()/getScale() guard for the documented crash (reaching the final assertion after a real forced repaint is genuine behavioral evidence). The gap is only that it does not cover the second (drawFleets) draw-chain path called out in the WARNING finding. If that path is guarded, add a companion repaint test with a game whose player holds a located fleet but no FMap.

Test sufficiency assessment:
- SUFFICIENT for the specific documented crash: testGamePanelRepaintWithNoLiveFMapDoesNotCrash is a genuine behavioral test -- it installs a live FGame on a shown FGamePanel with no FMap, forces a real repaint through FGamePanel::onPaint -> WXGameDisplay::draw -> WXMapDisplay::draw()/getScale(), and asserts survival; it would have crashed against the unfixed code, satisfying the repo's behavioral-verification mandate.
- GAP (non-blocking): the test does not exercise the WXPlayerDisplay::drawFleets() FMap sub-path (player-less game -> empty player loop). See NOTE finding; add fleet-bearing no-map repaint coverage if/when drawFleets is guarded.

Documentation / operational guidance assessment:
- AGENTS.md and doc/deferred-tasks.md were updated to describe the hasMap() predicate, the WXMapDisplay guards, the confirming test, and that the onOpen deferral remains; the doxygen headers on FMap::hasMap() and the two WXMapDisplay methods are accurate and clear.
- OVERSTATEMENT (non-blocking): the docs remove SF-nullfmap-paint-guard and describe the root-cause guard as closing the paint-path trust boundary, but the guard covers only the WXMapDisplay portion of the WXGameDisplay draw chain; WXPlayerDisplay::drawFleets() remains an unguarded FMap access path. The original deferred-task text explicitly scoped the fix to 'any other path... in the FGamePanel/WXGameDisplay draw chain', which drawFleets is. Recommend tightening the closure wording (or adding the drawFleets guard so the wording becomes accurate).

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/security_report.md
- artifacts/phase5-save-format-followups/FR-2/security_result.json

Outcome:
- CONDITIONAL PASS
