Implementation summary
- Added tactical live GUI coverage scaffolding under tests/gui via TacticalGuiLiveTest and runner wiring.
- Added/extended GUI harness modal helpers and tactical adapter auto-dismiss seam for deterministic message modal closure in test automation.
- Added live parent-backed WXTacticalUI coverage for redraw/message/damage summary/winner and non-empty ICM path guard behavior.

Files changed
- include/gui/WXTacticalUI.h
- src/gui/WXTacticalUI.cpp
- tests/gui/GuiTests.cpp
- tests/gui/Makefile
- tests/gui/WXGuiTestHarness.h
- tests/gui/WXGuiTestHarness.cpp
- tests/gui/TacticalGuiLiveTest.h
- tests/gui/TacticalGuiLiveTest.cpp

Validation commands run
- cd tests/gui && make -j2 >/dev/null && ./GuiTests

Validation outcome
- PASS: GUI tests runner completed (11 tests)
