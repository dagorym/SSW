Verifier Report

Scope reviewed:
- Combined `gui_sep...HEAD` changes for `FBattleDisplay`, tactical delegation tests, and Milestone 8 Subtask 2 documentation.
- Implementation files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`
- Test files: `tests/tactical/FTacticalBattleDisplayFireFlowTest.h`, `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- Documentation files: `doc/DesignNotes.md`, `AGENTS.md`

Acceptance criteria / plan reference:
- Documenter handoff prompt for Milestone 8 Subtask 2 delegation of `FBattleDisplay` fire/setup behavior through `FBattleScreen` -> `FTacticalGame`
- Shared verifier workflow in `/home/tstephen/repos/agents/agents/verifier.md`

Convention files considered:
- `/home/tstephen/worktrees/SSW-m8/gui_sep-m8-subtask2-verifier-20260401/AGENTS.md`
- `/home/tstephen/repos/agents/AGENTS_LOOKUP.md`
- `/home/tstephen/repos/agents/agents/verifier.md`

Findings

BLOCKING
- `src/tactical/FBattleDisplay.cpp:541-555`, `src/tactical/FTacticalGame.cpp:445-478`, `src/tactical/FBattleScreen.cpp:463-469` - Fire-phase delegation now clears destroyed ships twice, which drops the destroyed-ship ID list before the wx map cleanup runs.
  `FTacticalGame::fireAllWeapons()` already calls `clearDestroyedShips()` and records the destroyed ship IDs in `m_lastDestroyedShipIDs`. `FBattleDisplay` then calls `FBattleScreen::clearDestroyedShips()`, but that wrapper immediately calls `FTacticalGame::clearDestroyedShips()` again. The second model call starts by clearing `m_lastDestroyedShipIDs`, so `FBattleScreen` reaches `m_map->removeShipFromGame(*itr)` with an empty ID list and leaves freshly destroyed ships on the battle board/UI. This is a functional regression in shipped combat resolution, not just a test issue.

WARNING
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp:113-119`, `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp:129-135` - The updated tactical delegation tests encode the duplicate cleanup sequence instead of validating destroyed-ship synchronization.
  Both fire-phase tests require `m_parent->clearDestroyedShips();` after `resolveCurrentFirePhase()`, but they never check whether destroyed ships are actually removed from the wx map/runtime state. That allowed the regression above to pass while the test suite still reported `OK (51 tests)`.

Test sufficiency assessment:
- The tactical suite still passes (`cd tests/tactical && make -s && ./TacticalTests` -> `OK (51 tests)`), but coverage is not sufficient for the fire-phase delegation seam because it only inspects source strings. It misses the runtime contract between model-side destroyed-ship bookkeeping and `FBattleScreen` map cleanup, and one source-inspection assertion now reinforces the broken call order.

Documentation accuracy assessment:
- `doc/DesignNotes.md` and `AGENTS.md` correctly describe the intended delegation shape for Milestone 8 Subtask 2, but they do not match the currently shipped runtime behavior in one important respect: destroyed-ship cleanup after delegated fire resolution is not functioning correctly because the display/screen/model sequence is inconsistent.

Verdict:
- FAIL
