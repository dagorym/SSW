Implemented live wx modal-flow coverage for battle-screen launch chains:

- Added `BattleSimGuiLiveTest` and integrated it into `tests/gui/GuiTests.cpp` + `tests/gui/Makefile`.
- Extended `StrategicGuiLiveTest` with `SelectCombatGUI` launch coverage that opens `FBattleScreen` and asserts deterministic teardown.
- Added `FBattleScreen` lifecycle counters (constructed/destroyed/live) used by GUI tests to verify ownership/lifetime behavior.
- Added test seams (`finishDialog`, `finalizeStartBattle`) for deterministic, headless modal orchestration in `SelectCombatGUI` and `ScenarioEditorGUI`.
- Hardened GUI harness modal auto-dismiss behavior for nested modal flows and suppressed wx assert popups in headless runs.
- Updated BattleSim scenario/editor fleet icon setup to keep launch flows stable in live rendering paths.
- Updated `doc/DesignNotes.md` with the canonical headless GUI command and modal-launch coverage summary.

Validation run:

```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

Result: `OK (19 tests)`.
