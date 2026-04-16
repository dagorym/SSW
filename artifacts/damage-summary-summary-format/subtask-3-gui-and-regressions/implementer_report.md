Implementer report — damage-summary-summary-format subtask 3

Plan step status
- Preflight scope check: complete (used Subtask 3 prompt as source of truth).
- Incremental implementation: complete.
- Validation: tactical command passed; GUI command blocked by missing xvfb-run binary in environment.
- Commit handling: complete (implementation commit created; artifact commit pending in this report stage).

Files changed
- src/gui/TacticalDamageSummaryGUI.cpp
- tests/tactical/FTacticalDamageSummaryGUITest.cpp
- tests/tactical/FTacticalCombatReportTest.cpp

Summary of implementation
- Updated TacticalDamageSummaryGUI ship-rollup rendering to place a blank line between ship blocks for cleaner multi-line header-plus-bullets display.
- Updated FTacticalDamageSummaryGUITest coverage to assert ship header + bullet-line format and explicit ADF/MR point-loss bullets (ADF (-2), MR (-1)) in dialog summary data.
- Strengthened FTacticalCombatReportTest mixed-effects summary case to include grouped defense-hit bullets alongside hull/weapon/electrical/ADF/MR bullets and assert the multiline contract stays intact.

Validation commands run
1) cd tests/tactical && make && ./TacticalTests
- Result: PASS (OK: 92 tests).

2) cd tests/gui && make && xvfb-run -a ./GuiTests
- Result: FAIL in this environment.
- Failure: `xvfb-run: command not found`.
- Notes: attempted remediations (`sudo apt-get install xvfb` and `apt-get install xvfb`) were not possible due missing sudo credentials and package-manager permission lock.

3) Additional compile-safety check for changed tactical test sources
- Command: cd tests/tactical && make FTacticalDamageSummaryGUITest.o FTacticalCombatReportTest.o
- Result: PASS.

Validation outcome
- Implementation changes compile and tactical validation passes.
- Required GUI runtime validation could not execute because the required headless display wrapper is unavailable in this environment.
