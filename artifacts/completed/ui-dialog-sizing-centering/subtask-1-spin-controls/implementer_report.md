Implementation Report — subtask-1-spin-controls

Plan step status:
1. Preflight scope check — complete (goal, allowed files, acceptance criteria, and validation commands captured).
2. Implement incrementally — complete.
3. Validate after each change — complete.
4. Completion gate checks — complete.

Summary of changes:
- Replaced hard-coded tactical speed spin control width with wxDefaultSize and set min size from GetBestSize().
- Replaced hard-coded ICM allocation spin control width with wxDefaultSize, set min size from GetBestSize(), and kept sizer-driven placement.
- Extended live GUI regression coverage to assert positive runtime geometry for dynamic ICM spin controls after row selection.
- Extended tactical source-contract regression to enforce speed spin min-size contract and reject legacy narrow hard-coded width.

Files changed:
- src/tactical/FBattleDisplay.cpp
- src/gui/ICMSelectionGUI.cpp
- tests/gui/TacticalGuiLiveTest.cpp
- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp

Validation commands run:
- cd tests/gui && make && xvfb-run -a ./GuiTests (failed: xvfb-run not installed in environment)
- cd tests/tactical && make && ./TacticalTests (pass: OK 92 tests)
- cd tests/gui && make (pass)
- cd tests/gui && ./GuiTests (pass: OK 27 tests)
- cd tests/tactical && make && ./TacticalTests (pass: OK 92 tests)

Validation outcome:
- Final validation pass on both targeted suites (GuiTests and TacticalTests).
- No remaining known failures tied to this implementation.

Implementation/code commit:
- 1670f967f886f79ecdb1594d32c153e5c0d2a90c
