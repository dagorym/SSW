# Implementer Report

Status:
- success

Task summary:
- FR-2 pass-2 Verifier-driven remediation: close the residual unguarded NULL/half-built FMap dereference in the second gui draw path -- WXPlayerDisplay::drawFleets() -- so the FGamePanel::onPaint -> WXGameDisplay::draw chain is root-cause-complete against a NULL/half-built FMap singleton, and correct the AGENTS.md guard-coverage note. Pass-1 changes (FMap::hasMap() + WXMapDisplay guards) are retained unchanged.

Changed files:
- src/gui/WXPlayerDisplay.cpp -- add FMap::hasMap() early-return guard at the top of drawFleets() before binding FMap *map = &(FMap::getMap()); add strategic/FMap.h include
- include/gui/WXPlayerDisplay.h -- update drawFleets() Doxygen to describe the guard; bump Last Modified to Jul 19, 2026 and add Claude Opus 4.8 (1M context) author
- AGENTS.md -- correct the StrategicGuiLiveTest / null-FMap paint note so it states the guard now covers BOTH the WXMapDisplay path AND the WXPlayerDisplay::drawFleets() path (gui paint path now root-cause-complete); P5-5 FMainFrame::onOpen mitigation description left intact

Validation commands run:
- make -C <worktree> check  (from repo root; builds all + runs SSWTests, TacticalTests, GuiTests under xvfb-run)

Validation outcome:
- GREEN. make check passed: SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (84 tests). Exit 0. (A prior run reported 'Text file busy' only because a leftover background make check was holding the SSWTests binary concurrently; after killing it the clean foreground run passed.)

Implementation/code commit hash:
- 7d679b5cac99c0ced21f5316f2fe71c2e5bb696f

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/implementer_report.md
- artifacts/phase5-save-format-followups/FR-2/tester_prompt.txt
- artifacts/phase5-save-format-followups/FR-2/implementer_result.json

Implementation context:
- Guard site: src/gui/WXPlayerDisplay.cpp WXPlayerDisplay::drawFleets(wxDC&, FPlayer*). The very first statement is now `if (!FMap::hasMap()) { return; }`, placed BEFORE `FMap *map = &(FMap::getMap());` (binding a reference/pointer to *m_map when m_map is NULL is itself UB, independent of later dereference). FMap::hasMap() (include/strategic/FMap.h / src/strategic/FMap.cpp, added in FR-2 pass-1) returns m_map != NULL without dereferencing.
- Reachability: WXGameDisplay::draw() (src/gui/WXGameDisplay.cpp) calls playerDisplay.drawFleets(dc, *itr) for every non-NULL player. drawFleets() only touches FMap (map->getSystem()/getJumpRoute()) when a player owns at least one fleet with a location/jump. Under current code a located fleet cannot exist without a live FMap (FGame::init() calls initMap() before initFleets(); FGame::load() creates/loads FMap and aborts before constructing any player if the map load fails), so the crash is currently unreachable in production -- but that is an IMPLICIT ordering invariant, which is exactly what FR-2 removes. The guard makes it explicit.
- HOW TO REACH THE PAINT PATH WITH A NULL MAP (for the behavioral test): the pass-1 test used the no-arg FGame::create() overload (no FMap side effect) but that FGame has NO players, so WXGameDisplay::draw()'s player loop never calls drawFleets(). To exercise drawFleets specifically you need an FGame that (a) has NO live FMap, and (b) has at least one FPlayer that owns at least one FFleet with a drawable location so drawFleets reaches the map access. Options to investigate in order of preference: (1) build a minimal FGame via FGame::create(), then add an FPlayer with an FFleet through the available public/strategic APIs while deliberately NOT calling FMap::create() (ensure FMap::hasMap() is false first by tearing down any residual singleton); install that FGame on a shown FGamePanel via setGame(...) and force a real repaint through FGamePanel::onPaint() -> WXGameDisplay::draw() -> WXPlayerDisplay::drawFleets(); assert the process survives and !FMap::hasMap() still holds. (2) If constructing a fleet-owning player without an FMap is not achievable through public APIs (fleet/player construction may itself require or create an FMap), then the guard is instead proven by: calling WXPlayerDisplay::drawFleets(dc, player) directly on a wxMemoryDC with a mock/real FPlayer that returns a non-empty fleet list, with FMap::hasMap() forced false, and asserting it returns without dereferencing (no crash) -- a focused unit-style behavioral check of the guarded function. If NEITHER state is constructible through available APIs, state that explicitly in the tester report and explain that the guard is otherwise proven by inspection plus the pass-1 whole-chain repaint test (which proves FMap::hasMap() itself works and short-circuits the sibling WXMapDisplay path), noting the invariant that currently makes the fleet-bearing-no-map state unreachable in production.
- Do NOT rely on source-string/assertContains checks as the sole coverage (AGENTS.md Behavioral Verification mandate). Prefer a real drawFleets invocation with hasMap() false.
- Pass-1 artifacts are archived under artifacts/phase5-save-format-followups/FR-2/history/pass-1/; regenerate fresh live artifacts this pass (done).

Expected validation failures carried forward:
- None
