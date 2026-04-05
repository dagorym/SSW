## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added the new GUI harness top-level discovery and orphan-cleanup guidance, plus the explicit destroy-and-pump teardown expectation for live GUI fixtures.
- **`doc/DesignNotes.md`** — Recorded the shipped `WXGuiTestHarness` observation/cleanup APIs, the tactical and BattleSim live-test cleanup change, and the updated `GuiTests` result.

### Summary
Documented the new GUI harness top-level observation and zero-orphan cleanup behavior in the existing contributor guidance and design notes, including the expectation that affected live GUI tests explicitly destroy shown parent frames or dialogs and pump events before teardown completes.

### Commit Message
`Document GUI harness cleanup expectations`
