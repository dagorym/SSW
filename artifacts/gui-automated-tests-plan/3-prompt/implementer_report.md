Implementation summary
- Added live strategic GUI dialog coverage focused on model/state outcomes for UPF/Sathar assignment, transfer workflow, combat selection dialogs, and battle results editing.
- Extended GUI harness with modal action support for deterministic dialog return-code assertions.
- Hardened dialog internals required by the new live coverage:
  - Fixed iterator erasure safety and defensive fleet update handling in TransferShipsGUI.
  - Fixed wx sizer assertions in BattleResultsGUI for wx3.3 runtime compatibility.

Files changed
- tests/gui/StrategicGuiLiveTest.h
- tests/gui/StrategicGuiLiveTest.cpp
- tests/gui/WXGuiTestHarness.h
- tests/gui/WXGuiTestHarness.cpp
- src/gui/TransferShipsGUI.cpp
- src/gui/BattleResultsGUI.cpp

Validation commands run
- cd tests/gui && make clean >/dev/null && make && xvfb-run -a ./GuiTests (baseline)
- cd tests/gui && make clean >/dev/null && make && xvfb-run -a ./GuiTests (post-change iterations)
- cd tests/gui && make && xvfb-run -a ./GuiTests (final)

Validation outcome
- PASS: cd tests/gui && make && xvfb-run -a ./GuiTests
