Tester Agent Report — startup-splash-frame-centering/subtask-1

## Scope Restatement
Validate that startup splash/frame creation was extracted into `createStartupSplashAndFrame(...)` and both `FApp::OnInit()` and `FBattleSimApp::OnInit()` delegate to it while preserving splash/frame startup behavior.

## Test Changes
- Modified `tests/gui/StrategicGuiLiveTest.cpp` (extended existing source-contract test).
- Modified `tests/gui/BattleSimGuiLiveTest.cpp` (extended existing source-contract test).
- No implementation files changed.

## Commands Run
1. `cd tests/gui && make && (xvfb-run -a ./GuiTests || ./GuiTests)`

## Structured Results
- Total tests written: 0 new tests
- Existing tests modified: 2
- Tests passed: 29
- Tests failed: 0

## Acceptance Criteria Validation
1. SSW startup path uses extracted seam: **Met**
   - Verified `src/FApp.cpp` delegates to `createStartupSplashAndFrame(...)` and no longer contains inline splash loading/creation tokens.
2. BattleSim startup path uses extracted seam: **Met**
   - Verified `src/FBattleSimApp.cpp` delegates to `createStartupSplashAndFrame(...)` and no longer contains inline splash loading/creation tokens.
3. Both `OnInit()` methods delegate instead of duplicating inline startup logic: **Met**
   - Source-contract assertions added in strategic and BattleSim GUI tests.
4. Extracted seam preserves concurrent splash/frame startup behavior: **Met**
   - Verified helper contract in `include/gui/WXStartupLaunch.h` keeps timeout splash creation plus frame show/top-window flow and call ordering.

## Failures
- None.

## Commit Status
- Test changes commit: `752f085f168e658312ce16ba1bd36b94967b410d`
- Artifact files will be committed separately per two-commit workflow.

## Cleanup
- No temporary non-handoff byproducts created.
