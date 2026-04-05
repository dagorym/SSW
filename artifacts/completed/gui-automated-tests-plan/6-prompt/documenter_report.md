## Documenter Report

### Files Updated
- **`AGENTS.md`** — Removed unresolved merge markers from the GUI testing section and aligned contributor guidance with the implemented live modal-launch coverage and canonical headless GUI command.

### Summary
Updated repository-wide contributor guidance to reflect the implemented Subtask 6 GUI modal-launch coverage. The AGENTS guide now explicitly documents the live launch-path coverage for `SelectCombatGUI`, `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI`, notes the `FBattleScreen` lifecycle assertions, and preserves the canonical headless validation command `cd tests/gui && make && xvfb-run -a ./GuiTests`.

### Commit Message
`Document GUI modal launch coverage`
