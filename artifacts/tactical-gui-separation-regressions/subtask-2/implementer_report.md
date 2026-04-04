Implementation Summary
- Fixed move-route selection indexing in `src/tactical/FTacticalGame.cpp` so clicked highlighted hexes are included in the committed route endpoint.
- Added runtime tactical regression fixture `FTacticalMoveRouteSelectionTest` covering:
  - first highlighted forward extension,
  - later highlighted forward extension to exact clicked endpoint,
  - backtracking trim to an earlier clicked path hex.
- No `FBattleBoard.cpp` changes were required; renderer contract remained aligned.

Files Changed
- src/tactical/FTacticalGame.cpp
- tests/tactical/FTacticalMoveRouteSelectionTest.h
- tests/tactical/FTacticalMoveRouteSelectionTest.cpp

Validation Commands Run
1) `cd tests/tactical && make && ./TacticalTests`
   - Result: PASS (OK (75 tests))
2) `g++ -Iinclude -Itests/tactical ... -c tests/tactical/FTacticalMoveRouteSelectionTest.cpp ...`
   - Result: PASS (compiles cleanly)
3) Standalone runtime execution of new fixture (compiled runner + test and executed `./tests/tactical/FTacticalMoveRouteSelectionStandalone`)
   - Result: PASS (OK (3 tests))

Acceptance Criteria Mapping
- First highlighted movement hex advances by exactly one: covered by `testFirstHighlightedHexExtendsRouteByOne`.
- Later highlighted hex lands on exact clicked hex: covered by `testLaterHighlightedHexBecomesRouteEndpoint`.
- Earlier path hex trims to exact clicked hex: covered by `testSelectingEarlierPathHexTrimsRouteToClickedHex`.
- Movement counters and heading consistency: asserted in all three tests (`nMoved`, path length, heading/finalHeading consistency).
- Renderer adjustment only if needed: not needed after model fix.
