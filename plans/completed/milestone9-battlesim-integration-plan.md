# Milestone 9 Plan: BattleSim Integration

## Feature Restatement

Update the `BattleSim` executable so its tactical battle flow is confirmed to work with the Milestone 5-8 tactical separation work, where `FBattleScreen` owns `FTacticalGame` and installs `WXTacticalUI`.

## Confirmed Repository Facts

- `FBattleScreen` now constructs `FTacticalGame` and `WXTacticalUI` internally in [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp).
- BattleSim tactical launchers in [`src/battleSim/ScenarioDialog.cpp`](/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp) and [`src/battleSim/ScenarioEditorGUI.cpp`](/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp) currently create stack `FBattleScreen` dialogs and call `ShowModal()`.
- Existing tactical source-inspection coverage in [`tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp) already asserts that BattleSim scenario launchers use stack-owned modal `FBattleScreen` instances.
- `BattleSim` is built from the repo root through [`src/Makefile`](/home/tstephen/repos/SSW/src/Makefile), which produces both `SSW` and `BattleSim`.
- The milestone design notes in [`plans/ui-model-separation-design.md`](/home/tstephen/repos/SSW/plans/ui-model-separation-design.md) explicitly describe Milestone 9 as low-risk verification plus any remaining BattleSim wiring cleanup.

## Assumptions

- No elective direct-`FTacticalGame` BattleSim refactor will be performed unless inspection or runtime validation reveals a concrete integration problem that the current `FBattleScreen` path cannot solve cleanly.
- Runtime validation will be treated as part of milestone completion even if production code changes are not required.

## Files To Modify

Confirmed likely files:

- [`tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp)
- [`tests/tactical/FTacticalBattleScreenElectricalFireTest.h`](/home/tstephen/repos/SSW/tests/tactical/FTacticalBattleScreenElectricalFireTest.h)
- [`tests/tactical/Makefile`](/home/tstephen/repos/SSW/tests/tactical/Makefile)
- [`tests/tactical/TacticalTests.cpp`](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)

Conditionally likely production files if a bypass or integration defect is found:

- [`src/battleSim/ScenarioDialog.cpp`](/home/tstephen/repos/SSW/src/battleSim/ScenarioDialog.cpp)
- [`src/battleSim/ScenarioEditorGUI.cpp`](/home/tstephen/repos/SSW/src/battleSim/ScenarioEditorGUI.cpp)
- [`src/battleSim/LocalGameDialog.cpp`](/home/tstephen/repos/SSW/src/battleSim/LocalGameDialog.cpp)
- [`src/battleSim/BattleSimFrame.cpp`](/home/tstephen/repos/SSW/src/battleSim/BattleSimFrame.cpp)
- [`src/FBattleSimApp.cpp`](/home/tstephen/repos/SSW/src/FBattleSimApp.cpp)
- [`src/BattleSim.cpp`](/home/tstephen/repos/SSW/src/BattleSim.cpp)
- [`include/tactical/FBattleScreen.h`](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h)
- [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp)

Documentation-only if architecture notes materially change:

- [`plans/ui-model-separation-design.md`](/home/tstephen/repos/SSW/plans/ui-model-separation-design.md)

## Overall Documentation Impact

No user-facing documentation update is expected if BattleSim already works through the existing `FBattleScreen` constructor path. If Milestone 9 requires a new explicit BattleSim integration seam or a materially different ownership model, update the internal architecture notes in [`plans/ui-model-separation-design.md`](/home/tstephen/repos/SSW/plans/ui-model-separation-design.md) to reflect the final runtime path.

## Subtasks

### Subtask 1: Tighten automated coverage around BattleSim tactical launch paths

Inspect every BattleSim launcher that can reach tactical combat and add or extend source-inspection tests so the milestone is guarded by automation instead of manual memory. The goal is to lock in that BattleSim launchers continue to create `FBattleScreen` through the constructor path that owns `FTacticalGame` and installs `WXTacticalUI`, and that modal lifetime stays owned by the caller.

Acceptance Criteria:

- Tactical tests identify the BattleSim files that launch `FBattleScreen`.
- Automated coverage fails if a BattleSim launcher switches from stack/modal ownership to an unsafe or bypassing construction pattern.
- Automated coverage fails if `FBattleScreen` stops owning/installing the tactical model/UI pair in its standard constructor.
- `cd tests/tactical && make && ./TacticalTests` passes after the test updates.

Documentation Impact:

- None expected.

### Subtask 2: Normalize any BattleSim launch path that bypasses the Milestone 7 constructor contract

Use the audit and tests from Subtask 1 to determine whether any BattleSim code path reaches tactical combat without relying on the `FBattleScreen` constructor that creates `FTacticalGame` and `WXTacticalUI`. If such a path exists, make the smallest production change necessary so every BattleSim tactical entry point uses the supported constructor/lifetime flow. If no bypass exists, leave production code unchanged.

Acceptance Criteria:

- Every BattleSim tactical entry point reaches combat through an `FBattleScreen` construction path that leaves the dialog with a valid installed tactical UI.
- No BattleSim path depends on external callers manually constructing or installing `WXTacticalUI`.
- Any production changes remain localized to BattleSim launchers or a narrowly-scoped `FBattleScreen` API adjustment.
- Existing tactical tests and any new BattleSim launch-path tests pass after the change.

Documentation Impact:

- None if this is a no-op after audit.
- If a new explicit BattleSim-specific tactical construction seam is introduced, update internal architecture notes in [`plans/ui-model-separation-design.md`](/home/tstephen/repos/SSW/plans/ui-model-separation-design.md).

### Subtask 3: Build and runtime-validate both executables

Perform the milestone verification work: build `BattleSim`, run a full battle to completion through a real BattleSim scenario flow, confirm the observed behavior matches the pre-refactor expectations, then confirm `SSW` also still builds and launches correctly after the Milestone 9 changes. This subtask is required even if Subtask 2 is a no-op.

Acceptance Criteria:

- `make` from the repo root completes successfully and produces both `SSW` and `BattleSim`.
- A full BattleSim scenario battle is run to completion through the GUI flow and does not fail because of missing tactical UI wiring.
- Runtime observations confirm BattleSim behavior remains consistent with the prior `FBattleScreen`-driven experience.
- `SSW` is rebuilt and smoke-run successfully after the BattleSim verification pass.
- Any runtime-only findings that do not require code changes are captured clearly in the implementation handoff or completion notes.

Documentation Impact:

- None expected.

## Dependency Ordering

1. Subtask 1 must happen first because it establishes the BattleSim launch-path inventory and the regression checks that define the safe constructor contract.
2. Subtask 2 depends on Subtask 1. If Subtask 1 proves there is no bypass, Subtask 2 becomes an explicit no-op with no production edits.
3. Subtask 3 depends on completion of Subtask 2, or on the Subtask 2 no-op decision, so runtime validation is performed against the final intended code path.

Parallelization notes:

- Test-file edits inside Subtask 1 can be parallelized across separate tactical test files if the write scopes stay disjoint.
- Runtime validation in Subtask 3 should not begin until the final launch-path code and tests are settled.

## Implementer Agent Prompts

### Implementer Prompt 1

You are the implementer agent.

Files you are allowed to change:

- `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.h`
- `tests/tactical/Makefile`
- `tests/tactical/TacticalTests.cpp`
- Other new or existing files under `tests/tactical/` only if they are required for narrowly-scoped BattleSim launch-path coverage

Task:

Audit the BattleSim tactical entry points and strengthen automated tactical test coverage so the repo enforces the Milestone 9 contract. Focus on source-inspection or similarly lightweight coverage that proves BattleSim launches tactical combat through the standard `FBattleScreen` constructor/lifetime path and that `FBattleScreen` continues to own/install `FTacticalGame` and `WXTacticalUI`.

Acceptance Criteria:

- Tactical tests identify the relevant BattleSim tactical launcher files.
- Tests fail if BattleSim launchers stop using the supported modal `FBattleScreen` construction/lifetime pattern.
- Tests fail if `FBattleScreen` stops creating `FTacticalGame` and installing `WXTacticalUI` in its constructor path.
- `cd tests/tactical && make && ./TacticalTests` passes.

### Implementer Prompt 2

You are the implementer agent.

Files you are allowed to change:

- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`
- `src/battleSim/LocalGameDialog.cpp`
- `src/battleSim/BattleSimFrame.cpp`
- `src/FBattleSimApp.cpp`
- `src/BattleSim.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `plans/ui-model-separation-design.md` only if the final architecture contract materially changes

Task:

If the BattleSim audit reveals any path that bypasses the Milestone 7 tactical constructor contract, make the smallest production change necessary so every BattleSim tactical launch path ends up with a valid `FBattleScreen` owning `FTacticalGame` and `WXTacticalUI`. Do not perform a broad direct-`FTacticalGame` BattleSim refactor unless the existing `FBattleScreen` integration cannot be made correct cleanly.

Acceptance Criteria:

- Every BattleSim tactical entry point uses a supported `FBattleScreen` construction path with a valid installed tactical UI.
- No caller-side manual `WXTacticalUI` setup is required for BattleSim.
- Production changes are localized and do not broaden tactical architecture scope beyond Milestone 9.
- All affected tactical tests pass after the production change.
- If no bypass exists, document that the subtask is a no-op and leave production code unchanged.

### Implementer Prompt 3

You are the implementer agent.

Files you are allowed to change:

- No production file changes are expected for this subtask.
- If a runtime defect is uncovered that must be fixed immediately, restrict edits to the files already allowed in Implementer Prompt 2 and record the reason for expanding scope.

Task:

Run the milestone verification sequence against the final code: build from the repo root, confirm both `SSW` and `BattleSim` build successfully, run a full BattleSim battle to completion through the GUI flow, and smoke-run `SSW`. Capture whether BattleSim behavior remains consistent with the pre-refactor experience and identify any runtime-only findings.

Acceptance Criteria:

- `make` from the repo root succeeds and produces both executables.
- A full BattleSim battle is exercised to completion without tactical UI wiring failures.
- `SSW` also builds and smoke-runs successfully after the Milestone 9 work.
- Verification results are captured clearly enough for milestone signoff.

## Output Artifact

Written plan path:

- [`plans/milestone9-battlesim-integration-plan.md`](/home/tstephen/repos/SSW/plans/milestone9-battlesim-integration-plan.md)
