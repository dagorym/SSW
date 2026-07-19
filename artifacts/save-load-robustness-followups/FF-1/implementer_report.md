# Implementer Report

Status:
- success

Task summary:
- FF-1 (reviewer follow-up / SF-located-object-ids): reject out-of-range fleet located-object reference IDs at LOAD TIME (root-cause), not with gui guards. A corrupt/malicious save can carry a fleet whose location (system) ID or jump-route ID does not resolve to any loaded FSystem/FJumpRoute. Previously the load accepted it, and the gui fleet-draw path later dereferenced the NULL result of FMap::getSystem(id)/getJumpRoute(id) and crashed. `FGame::load()` now validates, after each player loads, every one of that player's fleets against the already-loaded `m_universe`: the location (system) ID must resolve via `FMap::getSystem(id)` unless it is the documented `0` "not yet in a system" sentinel, and the jump-route ID must resolve via `FMap::getJumpRoute(id)` unless it is the `FFleet::NO_ROUTE` sentinel. The first unresolved reference aborts the load through the existing `reportLoadError(...)` -> `IStrategicUI::showMessage(...)` aggregate-abort path (the freshly-allocated FPlayer is deleted and `load()` returns nonzero), so no half-built game is committed as the live singleton. No gui file was modified.

Changed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h

Validation commands run:
- make -C src/strategic (module build)
- make -C . check from the worktree root (builds all + runs SSWTests, TacticalTests, GuiTests under xvfb-run)

Validation outcome:
- src/strategic builds clean with no new warnings. `make check` from the worktree root completed with exit code 0 (the check target stops and propagates a non-zero exit at the first failing suite, so exit 0 means all three suites passed): SSWTests, TacticalTests, and GuiTests (OK, 85 tests) all green.
- Note: the first `make check` run failed exactly one source-inspection test, `FGameHeaderDependencyTest::testSourceDoesNotUseDisplayPaintingHelpers`, because the initial implementation comment in FGame.cpp contained the literal token `WXPlayerDisplay` (that test asserts FGame.cpp source never contains `WXMapDisplay`/`WXPlayerDisplay`/`WXGameDisplay`). The comment was reworded to "gui fleet-draw path" with no gui class token; the second `make check` run is fully green. No production behavior was affected by that fix.
- No behavioral test for the new validation was authored by this run; behavioral coverage is the Tester's responsibility (tests/strategic/FGameSaveFormatTest.{h,cpp}).

Implementation/code commit hash:
- 375ff9b0

Artifacts written:
- artifacts/save-load-robustness-followups/FF-1/implementer_report.md
- artifacts/save-load-robustness-followups/FF-1/tester_prompt.txt
- artifacts/save-load-robustness-followups/FF-1/implementer_result.json

Implementation context:
- Fix location: `src/strategic/FGame.cpp`, `FGame::load()`, the per-player loop (previously ~lines 717-726). After `p->load(is)` succeeds, a new validation pass iterates `p->getFleetList()` BEFORE the existing "place fleets into their systems" pass. For each fleet: `if (location != 0 && m_universe->getSystem(location) == NULL) { delete p; reportLoadError(...); return 1; }` and `if (jumpRoute != FFleet::NO_ROUTE && m_universe->getJumpRoute(jumpRoute) == NULL) { delete p; reportLoadError(...); return 1; }`. The validation pass is deliberately separate from and precedes the addFleet pass, so a rejected save never wires a fleet into a system before aborting.
- Sentinel confirmation (per task instruction, confirmed from source rather than assumed): `FFleet::NO_ROUTE` and `FFleet::NO_DESTINATION` are both `static_cast<unsigned int>(-1)` (include/strategic/FFleet.h:41-42). Fleet "no system" location is `0`: `FFleet::FFleet()` sets `m_location = 0` (src/strategic/FFleet.cpp:24), `FGame::createSFNova()` deliberately leaves Strike Force Nova unplaced (no `setLocation`), and the sole existing gui guard already keys off `getLocation() != 0` (src/gui/WXPlayerDisplay.cpp:99). The jump-route sentinel is `FFleet::NO_ROUTE`, matching the same file's `getJumpRoute() == FFleet::NO_ROUTE` guard (WXPlayerDisplay.cpp:59) and the H4 note that jump-route ID `0` is a VALID distinct route (so `0` must NOT be treated as "no route" here).
- Cleanup/ownership on the abort path: at the point of each nonzero check the just-`new`ed FPlayer `p` is NOT yet in `m_players` (the `m_players.push_back(p)` is the last statement of the loop body, reached only when both checks pass), so `delete p` cannot double-free or leave a dangling `m_players` entry. `~FPlayer()` deletes its owned fleets (which delete their owned ships), so no leak. No fleet was added to any FSystem before the abort (the addFleet pass runs only after validation passes), so no dangling system->fleet pointer is left behind.
- Why the location check uses `!= 0` and the route check uses `!= FFleet::NO_ROUTE` (asymmetric): location `0` is the fleet's "not in a system" sentinel and is a legitimate valid state (e.g. unplaced Strike Force Nova), so it must be exempt from resolution. Route `0` is a real, distinct jump-route ID (H4), so only the distinguished `NO_ROUTE` sentinel is exempt.
- Doxygen: `FGame::load()`'s block in include/strategic/FGame.h was extended to document the new FF-1 fleet located-object-ID validation and bumped `@date Last Modified` to Jul 19, 2026 (Claude Sonnet 5 (medium) already present in the `@author` list). src/strategic/FGame.cpp file-header `@date Last Modified` bumped to Jul 19, 2026. FFleet.cpp/FFleet.h and FMap.h were not modified (validation was done inline in FGame::load(), the plan's explicitly-allowed alternative), so no Doxygen change was needed there.

Expected validation failures carried forward:
- None
