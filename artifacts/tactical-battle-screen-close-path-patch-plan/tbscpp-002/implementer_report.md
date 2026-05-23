# Implementer Report

Status:
- completed

Task summary:
- Patch GUI harness close-oracle behavior and tactical close-path regressions so tests prove tactical windows are hidden or absent after menu/title close requests.

Changed files:
- tests/gui/WXGuiTestHarness.h
- tests/gui/WXGuiTestHarness.cpp
- tests/gui/TacticalGuiLiveTest.cpp
- tests/gui/BattleSimGuiLiveTest.h
- tests/gui/BattleSimGuiLiveTest.cpp
- tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp

Validation commands run:
- cd tests && make tactical-tests && ./tactical/TacticalTests
- cd tests/gui && make
- xvfb-run -a tests/gui/GuiTests

Validation outcome:
- Targeted close-path regressions now pass with stronger close assertions. Suite-level validation still has pre-existing unrelated failures: tactical runner 1 source-token failure in FTacticalGameMechanicsTest; GUI runner 6 existing source-token failures in StrategicGuiLiveTest/TacticalGuiLiveTest.

Implementation/code commit hash:
- 850cd44

Artifacts written:
- artifacts/tactical-battle-screen-close-path-patch-plan/tbscpp-002/implementer_report.md
- artifacts/tactical-battle-screen-close-path-patch-plan/tbscpp-002/tester_prompt.txt
- artifacts/tactical-battle-screen-close-path-patch-plan/tbscpp-002/implementer_result.json

Implementation context:
- WXGuiTestHarness now keeps close-oracle success tied to hidden/absent windows instead of IsBeingDeleted alone.
- Close waits run an idle pass while polling so pending-delete top-level cleanup can complete during close observation.
- Orphan cleanup now includes pending-delete windows, hides shown survivors, and requests close without forcing manual operator closure.
- Tactical live tests now verify non-modal menu and title-bar close vectors leave FBattleScreen hidden/absent with settled live-instance counters.
- BattleSim live coverage now includes both modal menu-quit and modal title-bar-close vectors and verifies modal caller unwind with no live FBattleScreen remaining.
- Source-contract tactical tests were updated to enforce FBattleScreen-owned close guard expectations, Close(true) menu dispatch, and no event.Skip/model-close-guard lock-in.

Expected validation failures carried forward:
- tests/tactical/TacticalTests: FrontierTests::FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults still expects removed token 'm_closeInProgress = false;' in FTacticalGame.cpp.
- tests/gui/GuiTests: existing source-token failures in StrategicGuiLiveTest (static-box parent, WXStrategicUI source guard, first-show sizing contract file-read checks).
- tests/gui/GuiTests: existing source-token failures in TacticalGuiLiveTest (movement prompt text token, narrow-width margin token, ICM spinner style token).
