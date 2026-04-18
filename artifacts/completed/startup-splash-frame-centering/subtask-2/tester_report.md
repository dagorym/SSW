Tester Agent Report — startup-splash-frame-centering / subtask-2

Requested test scope
- Validate startup placement fixes for SSW and BattleSim splash/frame centering and launch ordering.
- Confirm old fixed SSW startup position removal.
- Confirm splash style preserves expected splash taskbar/top-level semantics.

Assumptions
- Ran the existing repository GUI suite command as `cd tests/gui && make && ./GuiTests` because `xvfb-run` is unavailable in this environment and `./GuiTests` executes successfully headlessly here.

Test changes
- Modified existing GUI live tests (no new test files):
  - tests/gui/StrategicGuiLiveTest.cpp
  - tests/gui/BattleSimGuiLiveTest.cpp
- Added/expanded assertions to validate:
  - frame centering on display for `FMainFrame` live creation,
  - startup helper centers frame before show,
  - splash uses `wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP`,
  - splash is created before frame creation/show,
  - `FApp.cpp` no longer uses legacy `wxPoint(50,50)`,
  - `FMainFrame.cpp` constructor centers on screen.

Commands run
1) Baseline build: `cd tests/gui && make`
2) Baseline suite: `cd tests/gui && ./GuiTests`
3) Post-change validation: `cd tests/gui && make && ./GuiTests`

Execution results
- Total tests written: 0 (existing tests updated)
- Test fixtures updated: 2
- GUI suite pass count: 29/29
- Failed: 0

Acceptance-criteria validation
1. SSW splash centers + initial FMainFrame opens centered: PASS (source-contract assertions + live frame centering assertion).
2. BattleSim splash centers + initial BattleSimFrame opens centered: PASS (source-contract assertions + existing live frame centering assertion).
3. Splash/frame coexist and splash remains above while visible: PASS (startup helper order checks and `wxSTAY_ON_TOP` style assertion).
4. FMainFrame no longer uses old fixed startup position: PASS (`wxDefaultPosition` present; `wxPoint(50,50)` absent in `src/FApp.cpp`).
5. Splash style preserves expected behavior for top-level/taskbar handling: PASS (`wxFRAME_NO_TASKBAR` + `wxSTAY_ON_TOP` asserted; legacy reduced style string rejected).
6. Deterministic startup placement for both apps: PASS (shared helper ordering + centering assertions verified in strategic and BattleSim coverage).

Commit status
- Test changes committed: YES
- Test commit hash: 4e7ebf46534c9a1a71892af1476951d1a6640975

Cleanup
- No temporary non-handoff byproducts were created by this tester workflow.
