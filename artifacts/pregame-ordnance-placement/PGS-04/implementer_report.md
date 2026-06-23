# Implementer Report

Status:
- success

Task summary:
- PGS-04: Pre-game seeker undeploy list and mine removal source-independence fix. During BS_PlaceSeekers, render a centered lower-panel list of placed inactive seekers grouped by (hex, source ship) with recall-click routing. Fix mine removal to work regardless of currently selected source, restoring ammo to the placing ship.

Changed files:
- include/tactical/FTacticalGame.h
- src/tactical/FTacticalGame.cpp
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp

Validation commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests

Validation outcome:
- PASS — 216 tactical tests pass, no regressions

Implementation/code commit hash:
- 1a34316

Artifacts written:
- artifacts/pregame-ordnance-placement/PGS-04/implementer_report.md
- artifacts/pregame-ordnance-placement/PGS-04/tester_prompt.txt
- artifacts/pregame-ordnance-placement/PGS-04/implementer_result.json

Implementation context:
- FTacticalGame::getPlacedSeekerHexGroups() groups inactive m_seekerMissiles by (hex, source) during BS_PlaceSeekers — returns vector<FTacticalPreGameSeekerHexGroup>
- FTacticalGame::recallPlacedSeekerAtHexSource(hex, source) removes exactly one inactive seeker match, erases corresponding m_placedOrdnance record, calls restoreAmmoForSource(), rebuilds source list, re-selects
- placeOrdnanceAtHex() PGS-04 branch (before toggle path): during BS_PlaceMines, if mined hex clicked, searches m_placedOrdnance by hex alone for FWeapon::M type, erases record, erases from m_minedHexList, restores ammo to the stored placing source (not selected source)
- FBattleScreen forwards both new methods with reDraw() on change for recallPlacedSeekerAtHexSource()
- FBattleDisplay extended with 5 parallel primitive-type vectors (m_preGameSeekerRecallRegions, m_preGameSeekerRecallHexes, m_preGameSeekerRecallShipIDs, m_preGameSeekerRecallWeaponIndices, m_preGameSeekerRecallWeaponIDs) cleared in draw()
- drawPlaceSeekers() renders centered recall list at panelW/2 — below source-selection rows, one row per group: 'Recall: <shipName> (<x>,<y>) x<count>'
- onLeftUp() for BS_PlaceSeekers: calls checkPreGameSeekerRecallSelection() first; only falls through to checkShipSelection() if no recall row hit
- checkPreGameSeekerRecallSelection() reconstructs FTacticalOrdnanceSource from parallel vectors and calls m_parent->recallPlacedSeekerAtHexSource()

Expected validation failures carried forward:
- None
