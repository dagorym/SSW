# Implementer Report

Status:
- success

Task summary:
- Fix Critical defect C1: FGame::cleanUpShips() mutated by-value copies of the fleet and ship vectors, so destroyed ships were never removed from real fleets, loss counters could inflate on re-scan, and empty fleets were never removed. Rewrote the cleanup loop to operate on the live FPlayer/FFleet containers via FFleet::removeShip(id) and FPlayer::removeFleet(id)/FSystem::removeFleet(id), collecting destroyed-ship IDs before mutation to avoid the erase-while-iterating skip bug, and deleting emptied, orphaned fleet objects.

Changed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h

Validation commands run:
- make -C src/core
- make -C src/strategic
- make (repo root)
- cd tests && make && ./SSWTests

Validation outcome:
- src/core and src/strategic (the two non-GUI modules touched by this change) build cleanly with the fix, no new warnings, and no errors in FGame.cpp/FGame.h. The repo-root `make` and `cd tests && make && ./SSWTests` commands specified in the task could NOT be executed to completion in this sandbox: no `wx-config` binary/wxWidgets installation is present anywhere on the machine (confirmed via `which wx-config` and a filesystem search; only unbuilt wxWidgets-3.3.2 source with wx-config.in templates exists). Every GUI-touching Makefile (src/gui, and every tests/* Makefile including tests/core, tests/strategic, tests/ships, tests/weapons, tests/tactical) unconditionally invokes `wx-config --cxxflags`/`--libs`, so this failure is pre-existing and environment-wide, not caused by or specific to this change -- it occurs identically on an unmodified checkout. This is a sandbox/tooling gap outside CRIT-1's allowed-file scope (src/strategic/FGame.cpp, include/strategic/FGame.h) and cannot be resolved without installing/building wxWidgets, which is out of scope for this surgical defect fix.

Implementation/code commit hash:
- 43fc466fab0be28e319504fd5fa24a8a03f536a5

Artifacts written:
- artifacts/phase1-critical-defects/CRIT-1/implementer_report.md
- artifacts/phase1-critical-defects/CRIT-1/tester_prompt.txt
- artifacts/phase1-critical-defects/CRIT-1/implementer_result.json

Implementation context:
- cleanUpShips() is a private FGame method; the only public entry point that reaches it is FGame::checkForCombat() (also private), which calls cleanUpShips() when a system currently has both a non-transit UPF fleet and a non-transit Sathar fleet present (combatOccurred flag). A behavioral test must drive cleanup through whatever public turn-processing path in FGame ultimately invokes checkForCombat() (e.g. ending a turn/round with opposing fleets co-located and a ship's HP set to <= 0 via FVehicle::setHP(int)), rather than calling cleanUpShips()/checkForCombat() directly.
- FFleet::removeShip(unsigned int id) (src/strategic/FFleet.cpp:71) returns the removed FVehicle* without deleting it; ownership passes to whoever calls it -- cleanUpShips() immediately hands that pointer to FPlayer::addDestroyedShip(ship), preserving today's destroyed-ship ownership contract (no new deletion of destroyed ships; that remains Phase 2 scope per the plan).
- FPlayer::removeFleet(unsigned int id) (src/strategic/FPlayer.cpp:162) and FSystem::removeFleet(unsigned int id) (src/strategic/FSystem.cpp:41) both erase-and-return without deleting; cleanUpShips() now calls both for any fleet left with 0 ships, then deletes the orphaned FFleet object itself, since no other live reference to a fleet was found in the codebase (searched for in-transit/secondary fleet tracking in FGame.cpp/FGame.h; fleets are only referenced from FPlayer::m_fleets and FSystem::m_fleets, both of which are cleared before the delete).
- The rewritten loop collects destroyed ship IDs in a first pass (read-only over the live const VehicleList& from FFleet::getShipList()) and removes them in a second pass by ID via FFleet::removeShip(id), avoiding the original code's erase-while-iterating skip bug (iterator invalidation plus a double sItr++ after erase). Surviving ships are still reload()ed in the same first pass, preserving the existing '@todo check for being in supply' comment on that line.
- Empty-fleet removal is deferred to after each player's fleet-list scan completes (fleets to remove are collected into a small std::vector<FFleet*>), so the outer fList iteration is never mutated mid-scan.
- The Doxygen header on FGame::cleanUpShips() in include/strategic/FGame.h was updated: description now documents the live-container behavior and the two-pass/ID-collection approach; @date Last Modified set to Jul 10, 2026; @author now reads 'Tom Stephens, Claude Sonnet 5 (medium)' (original author preserved, new author appended per repository convention).

Expected validation failures carried forward:
- `make` from repo root and `cd tests && make && ./SSWTests` cannot run to completion in this sandbox because `wx-config` is absent from the environment entirely (no wxWidgets install, no package, only unbuilt source with .in templates). This is a pre-existing, environment-wide gap affecting every GUI-linked Makefile in the repository (src/gui, and all of tests/core, tests/strategic, tests/ships, tests/weapons, tests/tactical), not something introduced by this change. It was verified that src/core and src/strategic -- the only two modules this change touches -- compile cleanly and independently of wx-config.
