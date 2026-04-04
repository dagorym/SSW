# Milestone 10 Final Validation and Cleanup Plan

## Feature Restatement
Perform the Milestone 10 closeout for the UI/game-logic separation effort by removing any remaining wx dependencies from non-GUI modules, enforcing non-wx build rules for model libraries, adding mock-UI regression coverage for strategic and tactical model flows, validating a clean repo build and full test pass, and updating project guidance to reflect the final dependency and interface rules.

## Confirmed Repository Facts
- The requested repo areas already expose non-wx UI seams at [include/strategic/IStrategicUI.h](/home/tstephen/repos/SSW/include/strategic/IStrategicUI.h) and [include/tactical/ITacticalUI.h](/home/tstephen/repos/SSW/include/tactical/ITacticalUI.h).
- `FGame` already supports `FGame::create(IStrategicUI * ui)` and `init(wxWindow * w)` ignores the window parameter in the current implementation at [include/strategic/FGame.h](/home/tstephen/repos/SSW/include/strategic/FGame.h) and [src/strategic/FGame.cpp](/home/tstephen/repos/SSW/src/strategic/FGame.cpp).
- `FTacticalGame` already supports `installUI(ITacticalUI * ui)`, `setupFleets(...)`, `resolveCurrentFirePhase()`, `isCombatOver()`, `hasWinner()`, and `getWinnerID()` at [include/tactical/FTacticalGame.h](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h).
- Current scan results show remaining wx includes in tactical runtime files:
  - [include/tactical/FBattleScreen.h](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h)
  - [include/tactical/FBattleDisplay.h](/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h)
  - [include/tactical/FBattleBoard.h](/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h)
  - [src/tactical/FBattleBoard.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp)
  - [src/tactical/FBattleScreen.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp)
  - [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp)
- Current scan results show all six non-GUI module Makefiles still include `wx-config` flags:
  - [src/core/Makefile](/home/tstephen/repos/SSW/src/core/Makefile)
  - [src/strategic/Makefile](/home/tstephen/repos/SSW/src/strategic/Makefile)
  - [src/ships/Makefile](/home/tstephen/repos/SSW/src/ships/Makefile)
  - [src/weapons/Makefile](/home/tstephen/repos/SSW/src/weapons/Makefile)
  - [src/defenses/Makefile](/home/tstephen/repos/SSW/src/defenses/Makefile)
  - [src/tactical/Makefile](/home/tstephen/repos/SSW/src/tactical/Makefile)
- Existing strategic and tactical test suites are already organized for CppUnit registration through:
  - [tests/strategic/Makefile](/home/tstephen/repos/SSW/tests/strategic/Makefile)
  - [tests/tactical/Makefile](/home/tstephen/repos/SSW/tests/tactical/Makefile)
  - [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp)
  - [tests/tactical/TacticalTests.cpp](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)

## Assumptions
- The tactical `FBattle*` files listed above remain part of the tactical module by design, but for this milestone they must stop directly including wx headers from `src/tactical` and `include/tactical`; if that is impossible for one of them, the implementer should treat that as a design bug and move the wx-facing surface behind gui-owned headers/source rather than weakening the milestone requirement.
- The new tactical winner test can use a lightweight harness ship and weapon similar to existing test harnesses instead of requiring production scenario data.
- The full repo test pass may still rely on wx for GUI-module tests and the top-level runner, but Milestone 10 does not require a fully automated GUI system beyond current unit/integration coverage.

## Files To Modify
- Confirmed likely code files for wx-include cleanup:
  - [include/tactical/FBattleScreen.h](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h)
  - [include/tactical/FBattleDisplay.h](/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h)
  - [include/tactical/FBattleBoard.h](/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h)
  - [src/tactical/FBattleBoard.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp)
  - [src/tactical/FBattleScreen.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp)
  - [src/tactical/FBattleDisplay.cpp](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp)
- Confirmed Makefiles to modify:
  - [src/core/Makefile](/home/tstephen/repos/SSW/src/core/Makefile)
  - [src/strategic/Makefile](/home/tstephen/repos/SSW/src/strategic/Makefile)
  - [src/ships/Makefile](/home/tstephen/repos/SSW/src/ships/Makefile)
  - [src/weapons/Makefile](/home/tstephen/repos/SSW/src/weapons/Makefile)
  - [src/defenses/Makefile](/home/tstephen/repos/SSW/src/defenses/Makefile)
  - [src/tactical/Makefile](/home/tstephen/repos/SSW/src/tactical/Makefile)
- Confirmed strategic test integration files:
  - [tests/strategic/Makefile](/home/tstephen/repos/SSW/tests/strategic/Makefile)
  - [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp)
  - New likely files: `tests/strategic/FGameMockStrategicUITest.h`, `tests/strategic/FGameMockStrategicUITest.cpp`
- Confirmed tactical test integration files:
  - [tests/tactical/Makefile](/home/tstephen/repos/SSW/tests/tactical/Makefile)
  - [tests/tactical/TacticalTests.cpp](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)
  - New likely files: `tests/tactical/FTacticalGameMockUITest.h`, `tests/tactical/FTacticalGameMockUITest.cpp`
- Confirmed documentation file:
  - [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md)

## Overall Documentation Impact
- Update [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md) to make the Milestone 10 dependency rule explicit: `core`, `strategic`, `ships`, `weapons`, `defenses`, and model-owned `tactical` code must not require `wx-config` compiler or linker flags.
- Document the intended ownership boundary for `IStrategicUI` and `ITacticalUI`, including that strategic/tactical logic may depend on those interfaces but wx-backed implementations belong in gui.
- Add an explicit note that no fully automated end-to-end GUI testing system exists or is required for milestone acceptance; unit and build validation are the expected automated coverage.

## Subtasks

### Subtask 1: Remove remaining wx include leaks from non-GUI model/module surfaces
Description:
Audit every `#include.*wx` hit under the requested source/header roots and eliminate each one by moving wx-only declarations/definitions out of non-GUI headers and sources or by replacing includes with narrower forward declarations or gui-owned boundaries where feasible.

Acceptance Criteria:
- A repo scan of `src/core`, `src/strategic`, `src/ships`, `src/weapons`, `src/defenses`, `src/tactical`, and matching `include/` directories returns zero `#include.*wx` hits except intentionally commented-out legacy lines that are removed or left non-active only if policy allows.
- Any currently flagged tactical `FBattle*` files no longer directly include wx headers from the searched locations.
- Non-GUI strategic, tactical model, and shared headers still compile after the include cleanup.
- No new direct gui dependency is introduced into `core`, `strategic`, `ships`, `weapons`, or `defenses`.

Documentation Impact:
- No standalone documentation file is required from this subtask, but [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md) will need final wording updated to reflect the enforced non-wx boundary once cleanup is complete.

### Subtask 2: Remove `wx-config` flags from non-GUI module Makefiles
Description:
Update the six model/module Makefiles so they no longer add `wx-config --cxxflags` or `wx-config --libs`, while preserving any include/library requirements still needed by those modules after the code cleanup.

Acceptance Criteria:
- [src/core/Makefile](/home/tstephen/repos/SSW/src/core/Makefile), [src/strategic/Makefile](/home/tstephen/repos/SSW/src/strategic/Makefile), [src/ships/Makefile](/home/tstephen/repos/SSW/src/ships/Makefile), [src/weapons/Makefile](/home/tstephen/repos/SSW/src/weapons/Makefile), [src/defenses/Makefile](/home/tstephen/repos/SSW/src/defenses/Makefile), and [src/tactical/Makefile](/home/tstephen/repos/SSW/src/tactical/Makefile) contain no active `wx-config --cxxflags` or `wx-config --libs` usage.
- Those module libraries still build successfully as part of the root build.
- Any Makefile comments or dependency notes that materially misstate the new rule are corrected.

Documentation Impact:
- [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md) should capture this rule explicitly in the module dependency/build guidance.

### Subtask 3: Add a strategic mock-UI regression test for `FGame`
Description:
Add a new CppUnit strategic test that constructs `FGame` through `FGame::create(IStrategicUI*)` with a mock `IStrategicUI`, runs initialization without a real GUI window, and verifies expected game state plus strategic UI callback behavior relevant to startup.

Acceptance Criteria:
- A new strategic test fixture exists under `tests/strategic/`.
- The test uses a mock/stub `IStrategicUI` implementation and does not instantiate real wx dialogs or windows.
- The test runs `FGame::init(NULL)` or equivalent non-window startup path against the mock UI.
- The test verifies at minimum:
  - initialization succeeds,
  - expected player/game state is populated,
  - UI-driven startup interactions are satisfied through the mock instead of real GUI,
  - no real GUI spawning is required.
- The new test is added to [tests/strategic/Makefile](/home/tstephen/repos/SSW/tests/strategic/Makefile) and [tests/SSWTests.cpp](/home/tstephen/repos/SSW/tests/SSWTests.cpp).

Documentation Impact:
- No new user-facing docs expected.
- [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md) should mention `IStrategicUI` as the required strategic dialog/notification seam and that strategic tests can mock it.

### Subtask 4: Add a tactical mock-UI winner regression test for `FTacticalGame`
Description:
Add a new CppUnit tactical test that installs a mock `ITacticalUI`, creates a minimal two-ship battle, drives the model to a battle conclusion without real GUI objects, and verifies the winner outcome.

Acceptance Criteria:
- A new tactical test fixture exists under `tests/tactical/`.
- The test uses a mock/stub `ITacticalUI` implementation and does not instantiate wx UI objects.
- The test sets up a deterministic two-ship combat using either lightweight harness ships/weapons or existing test harness patterns.
- The test drives `FTacticalGame` to combat completion and verifies:
  - the correct side wins,
  - `hasWinner()` / `getWinnerID()` or equivalent model state reflects the expected winner,
  - UI notifications, if exercised, go through the mock `ITacticalUI`.
- The new test is wired into [tests/tactical/Makefile](/home/tstephen/repos/SSW/tests/tactical/Makefile) and [tests/tactical/TacticalTests.cpp](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp).

Documentation Impact:
- No new user-facing docs expected.
- [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md) should mention `ITacticalUI` as the tactical redraw/dialog/winner seam and that milestone validation relies on model-level tests rather than automated full-GUI playback.

### Subtask 5: Update `AGENTS.md` for final dependency and interface rules
Description:
Revise project guidance to reflect the final post-separation rules for module dependencies, non-wx model builds, and the responsibilities of `IStrategicUI` and `ITacticalUI`.

Acceptance Criteria:
- [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md) explicitly states that `core`, `strategic`, `ships`, `weapons`, `defenses`, and non-gui `tactical` model code must not depend on wx includes or `wx-config` build/link flags.
- `AGENTS.md` documents that gui implementations such as `WXStrategicUI` and `WXTacticalUI` own wx-backed behavior while model code uses interface seams.
- `AGENTS.md` includes the requested note that a fully automated GUI test system does not exist and is not required to pass Milestone 10.

Documentation Impact:
- This subtask is the documentation update.

### Subtask 6: Execute final clean-build and full-test validation
Description:
Run the required root clean build and the full `tests/SSWTests` suite after code and test updates, then capture the validation outcome and any residual non-blocking notes.

Acceptance Criteria:
- `make all_clean && make` succeeds from the repo root.
- `cd tests && make && ./SSWTests` succeeds.
- The final verification report or handoff notes record that the build and full test suite passed.
- Any failures encountered during validation are fixed before milestone signoff rather than deferred.

Documentation Impact:
- No persistent documentation changes expected unless validation uncovers a project instruction mismatch that belongs in [AGENTS.md](/home/tstephen/repos/SSW/AGENTS.md).

## Dependency Ordering
1. Subtask 1 must happen before Subtask 2 if include cleanup changes compile requirements for the model modules.
2. Subtask 2 must complete before Subtask 6 because the clean-build check is one of its acceptance criteria.
3. Subtask 3 depends on the current `IStrategicUI`/`FGame` seam only; it can run in parallel with Subtask 1 or Subtask 2 unless startup fixes are needed.
4. Subtask 4 depends on the current `ITacticalUI`/`FTacticalGame` seam only; it can run in parallel with Subtask 1 or Subtask 2 unless tactical cleanup changes fixture setup.
5. Subtask 5 should happen after Subtasks 1-4 so the documentation reflects the final implemented rules, not the intended ones.
6. Subtask 6 is last and depends on Subtasks 1-5 being complete.

## Parallelization Notes
- Subtasks 3 and 4 are parallelizable once the implementer confirms the interface seams remain stable.
- Subtask 5 can be drafted late while final validation is running, but it should not be finalized until the implemented dependency rules are confirmed by Subtask 6.

## Implementer Agent Prompts

### Prompt for Subtask 1
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h`
- `/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h`
- `/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h`
- `/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp`
- `/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp`
- `/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp`
- Any directly required helper headers/sources under `/home/tstephen/repos/SSW/include/tactical`, `/home/tstephen/repos/SSW/src/tactical`, or `/home/tstephen/repos/SSW/include/gui`, `/home/tstephen/repos/SSW/src/gui` if moving wx-facing declarations is necessary.

Task:
Eliminate every active `#include.*wx` occurrence under the Milestone 10 audit scope. Treat each hit as a bug. Preserve tactical runtime behavior while moving or isolating wx-only dependencies behind gui-owned code or forward declarations as needed.

Acceptance criteria:
- The audited header/source roots return zero active `#include.*wx` hits.
- Tactical model/shared code still compiles.
- No new wx dependency is introduced into non-GUI modules.

### Prompt for Subtask 2
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/src/core/Makefile`
- `/home/tstephen/repos/SSW/src/strategic/Makefile`
- `/home/tstephen/repos/SSW/src/ships/Makefile`
- `/home/tstephen/repos/SSW/src/weapons/Makefile`
- `/home/tstephen/repos/SSW/src/defenses/Makefile`
- `/home/tstephen/repos/SSW/src/tactical/Makefile`

Task:
Remove active `wx-config --cxxflags` and `wx-config --libs` usage from the six non-GUI module Makefiles, keeping the model libraries buildable under the new boundary rules.

Acceptance criteria:
- None of the six Makefiles contain active `wx-config --cxxflags` or `wx-config --libs`.
- The root build still succeeds after the Makefile changes.

### Prompt for Subtask 3
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/strategic/Makefile`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`
- `/home/tstephen/repos/SSW/tests/strategic/FGameMockStrategicUITest.h`
- `/home/tstephen/repos/SSW/tests/strategic/FGameMockStrategicUITest.cpp`
- Existing nearby strategic test files only if minor registration or shared-fixture alignment is required.

Task:
Add a new strategic CppUnit test that constructs `FGame` with a mock `IStrategicUI`, runs the initialization sequence without spawning any real GUI, and verifies the resulting game state and UI-seam behavior.

Acceptance criteria:
- The test uses a mock `IStrategicUI`, not real wx dialogs/windows.
- The test verifies successful initialization and expected strategic state.
- The test is wired into the strategic Makefile and the top-level `SSWTests` runner.

### Prompt for Subtask 4
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalGameMockUITest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalGameMockUITest.cpp`
- Existing nearby tactical test files only if minor shared-fixture alignment is required.

Task:
Add a new tactical CppUnit test that installs a mock `ITacticalUI`, runs a deterministic two-ship `FTacticalGame` combat to conclusion, and verifies the correct winner without any real GUI objects.

Acceptance criteria:
- The test uses a mock `ITacticalUI`.
- The combat setup is deterministic and runs entirely through model/test harnesses.
- The correct winner is asserted from `FTacticalGame` state and any UI callback interaction goes through the mock.
- The test is wired into the tactical Makefile and tactical runner.

### Prompt for Subtask 5
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW/AGENTS.md`

Task:
Update `AGENTS.md` to document the final Milestone 10 module dependency rules and the `IStrategicUI` / `ITacticalUI` boundaries, including the note that a fully automated GUI test system does not exist and is not required for milestone acceptance.

Acceptance criteria:
- `AGENTS.md` clearly states the non-wx build/include rule for non-GUI modules.
- `AGENTS.md` describes `IStrategicUI` and `ITacticalUI` responsibilities and gui ownership of wx-backed implementations.
- `AGENTS.md` explicitly notes the absence of a fully automated GUI test system and that this does not block Milestone 10.

### Prompt for Subtask 6
You are the implementer agent.

Allowed files:
- `/home/tstephen/repos/SSW`

Task:
Run the required final validation commands after the Milestone 10 cleanup and tests are implemented, and fix any issues necessary to achieve a clean pass.

Acceptance criteria:
- `make all_clean && make` passes from `/home/tstephen/repos/SSW`.
- `cd /home/tstephen/repos/SSW/tests && make && ./SSWTests` passes.
- Final handoff includes the validation result and notes any residual non-blocking limitations only if they do not violate the milestone requirements.

## Output Artifact Path
- [plans/milestone10-final-validation-cleanup-plan.md](/home/tstephen/repos/SSW/plans/milestone10-final-validation-cleanup-plan.md)
