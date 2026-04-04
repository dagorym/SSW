Implementation report: subtask-3-battlesim remediation

Plan step status:
- Read AGENTS.md, verifier findings, and implementer definition; confirmed cwd and branch.
- Tightened `tests/gui/BattleSimGuiLiveTest.cpp` launch assertions to verify concrete dialog classes:
  - `BattleSimFrame` path now proves `LocalGameDialog` modal launch with parent match.
  - `LocalGameDialog` predefined path now proves `ScenarioDialog` modal launch.
  - `LocalGameDialog` custom path now proves `ScenarioEditorGUI` modal launch.
- Preserved `FBattleScreen` lifecycle assertions and strengthened cleanup proof by asserting no shown top-level windows remain beyond each test’s baseline set.
- Ensured shown parent frames are explicitly hidden, destroyed, and event-pumped before test return.
- Ran canonical headless GUI validation and confirmed passing suite.

Files changed:
- tests/gui/BattleSimGuiLiveTest.cpp

Validation commands run:
- `cd tests/gui && make && xvfb-run -a ./GuiTests`

Validation outcomes:
- Pass (`OK (24 tests)`).

Implementation/code commit:
- `acb20c6a2f448b1ee35246b6a8c749287d29f12c`
