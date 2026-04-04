## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Updated the BattleSim GUI coverage notes to describe the stronger launch-proof assertions, preserved `FBattleScreen` lifecycle accounting, and baseline-restoring shown-window cleanup now enforced by `BattleSimGuiLiveTest`.
- **`AGENTS.md`** — Refined the repository testing guidance so the GUI-test summary and live-fixture instructions mention the BattleSim launch-path presentation checks and final shown-top-level baseline requirement.

### Summary
Updated the existing BattleSim live-GUI documentation surfaces to match the implemented and tester-validated behavior: the tests now prove `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI` launch along the expected paths, keep `FBattleScreen` lifecycle assertions in place, and explicitly destroy, stabilize, and clean up shown top-level windows before confirming teardown returned to baseline.

### Commit Message
`Document BattleSim GUI launch coverage`
