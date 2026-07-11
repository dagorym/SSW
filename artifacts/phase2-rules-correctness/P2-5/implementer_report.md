# Implementer Report

Status:
- success

Task summary:
- Fix the FPlayer::m_destroyed memory leak (reviewer follow-up F2): establish FPlayer as the sole owner of destroyed ships and free them in ~FPlayer(); document the ownership contract; create doc/deferred-tasks.md tracking the deferred save-format items (H2, F2-serialization). m_destroyed is intentionally NOT serialized.

Changed files:
- src/strategic/FPlayer.cpp
- include/strategic/FPlayer.h
- doc/deferred-tasks.md (new)

Validation commands run:
- make (repo root) -- clean build, exit 0
- cd tests && make && ./SSWTests -- built clean; 217 run, 9 pre-existing failures (all FGameHeaderDependencyTest, environment/worktree source-path resolution), 0 new failures

Validation outcome:
- PASS with pre-existing failures documented. Repo builds clean. SSWTests: Run 217, Failures 9, Errors 0. All 9 failures are FrontierTests::FGameHeaderDependencyTest source-inspection tests that fail identically on the unmodified baseline (verified by stashing the FPlayer changes and re-running: same 9 failures). They are unrelated to this change (they inspect FGame header/source structure and cannot locate readable candidate file paths in the worktree). FPlayerTest passes. The flaky FFleetTest::testDecTransitTime (documented RNG flake, targeted by P2-7) is not counted among the 9 and did not fail in the recorded run.

Implementation/code commit hash:
- 69d300b7e178e1ee2e811e4424b5914b79ee4f4a

Artifacts written:
- artifacts/phase2-rules-correctness/P2-5/implementer_report.md
- artifacts/phase2-rules-correctness/P2-5/tester_prompt.txt
- artifacts/phase2-rules-correctness/P2-5/implementer_result.json

Implementation context:
- src/strategic/FPlayer.cpp ~FPlayer(): added a third cleanup block that deletes every FVehicle* in m_destroyed and clears the vector, mirroring the existing m_fleets and m_unattached blocks. A comment documents why this cannot double-delete.
- ALIASING / DOUBLE-DELETE VERIFICATION (required by the plan's 2c assumption): I grepped every addDestroyedShip() call site. There are exactly three: src/strategic/FGame.cpp:1210 (cleanUpShips) passes fleet->removeShip(id); src/gui/BattleResultsGUI.cpp:480 passes f->removeShip(...) inline; and :580 passes m_curFleet->removeShip(...) inline. In every case the pointer handed to addDestroyedShip() is the direct return value of an FFleet::removeShip() call. FFleet::removeShip() (src/strategic/FFleet.cpp:71) erases the ship from m_ships and returns it WITHOUT deleting. Therefore, once a ship is in m_destroyed it is no longer referenced by any fleet's m_ships, nor by FPlayer::m_unattached (ships never move between m_unattached and m_destroyed). No aliasing exists; freeing m_destroyed in ~FPlayer() cannot double-delete a ship a surviving fleet still holds.
- include/strategic/FPlayer.h: added a full Doxygen block to the ~FPlayer() declaration; expanded the addDestroyedShip() Doxygen with the ownership-transfer contract and the not-serialized note; expanded the m_destroyed member comment; updated the class-level @author/@date and description. @author additions use 'Claude Sonnet 5 (medium)' per CLAUDE.md format; Last Modified dates set to Jul 11, 2026.
- save()/load() were NOT modified. m_destroyed remains unserialized by design (deferred item F2-serialization).
- doc/deferred-tasks.md is new: a standing deferred-items list with H2 (persist active defense across save/load; origin C3 + Phase-2 decision; deferred = save-format break; target = save-format hardening phase / roadmap Next #5) and F2-serialization (serialize FPlayer::m_destroyed for the future Replacements rule; origin reviewer F2; deferred = save-format break; same target phase).
- TESTER FOCUS: the test must FAIL against the UNFIXED destructor (i.e. before the m_destroyed delete block) -- e.g. an instrumented/counting FVehicle subclass or a static destruction counter proving destroyed ships are freed exactly once on FPlayer destruction, AND that a ship still held by a surviving fleet is freed exactly once (not double-freed). Behavioral, not source-inspection (per CLAUDE.md).

Expected validation failures carried forward:
- 9 x FrontierTests::FGameHeaderDependencyTest::* -- PRE-EXISTING and unrelated to this change. Verified by stashing the FPlayer edits and re-running SSWTests: the same 9 failures occur on the unmodified baseline. Root cause: these source-inspection tests cannot find readable candidate file paths in this worktree layout. Not a regression from P2-5.
- FrontierTests::FFleetTest::testDecTransitTime -- documented pre-existing RNG flake (plan item F1/P2-7's target). Not caused by P2-5 and not part of this subtask's scope.
