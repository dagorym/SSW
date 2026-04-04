Plan step status:
1. Completed: extended `WXGuiTestHarness` with top-level window observation helpers (`getTopLevelWindows`, `findTopLevelWindow`, `waitForTopLevelWindow`, `findModalDialog`, `waitForModalDialog`) and orphan cleanup enforcement (`cleanupOrphanTopLevels`).
2. Completed: added harness regression coverage in `GuiHarnessTest` for top-level observation, modal detection, and cleanup behavior.
3. Completed: updated tactical and BattleSim live GUI tests to avoid implicit shown-window stack lifetime cleanup by using explicit dialog/frame destruction order and event pumping.
4. Completed: validated with existing headless GUI suite command.

Files changed:
- tests/gui/WXGuiTestHarness.h
- tests/gui/WXGuiTestHarness.cpp
- tests/gui/GuiHarnessTest.h
- tests/gui/GuiHarnessTest.cpp
- tests/gui/TacticalGuiLiveTest.cpp
- tests/gui/BattleSimGuiLiveTest.cpp

Validation commands run:
```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

Validation outcomes:
- `OK (22 tests)`
