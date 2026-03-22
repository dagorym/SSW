# FFleet Destination Sentinel Mismatch Plan

## Confirmed Repository Facts
- `include/strategic/FFleet.h` stores `m_destination` and `m_jumpRouteID` as `unsigned int`.
- The header uses `-1` defaults for `setLocation(... dest = -1, ... route = -1)`, which is inconsistent with unsigned storage.
- `src/strategic/FFleet.cpp` sets `m_destination = -1` when a jump completes and resets `m_jumpRouteID` to `0`.
- `tests/strategic/FFleetTest.cpp` compares `getDestination()` and `getJumpRoute()` against `-1` in several places.
- Callers that read destination or route IDs exist in `src/strategic/FGame.cpp`, `src/gui/SystemDialogGUI.cpp`, and `src/gui/WXPlayerDisplay.cpp`.

## Assumptions
- The “unset” state should be explicit and consistent for both destination and jump route, either by switching to signed IDs or by introducing named sentinels for the unsigned IDs.
- Destination and jump-route handling should be normalized together so the fleet contract does not split into two different conventions.
- The serialization format should remain compatible unless the chosen fix clearly requires a versioned change.

## Files To Modify
- `include/strategic/FFleet.h`
- `src/strategic/FFleet.cpp`
- `tests/strategic/FFleetTest.cpp`
- likely `src/strategic/FGame.cpp`
- likely `src/gui/SystemDialogGUI.cpp`
- likely `src/gui/WXPlayerDisplay.cpp`
- possibly `src/gui/ViewFleetGUI.cpp` if the chosen contract requires an additional guard path there

## Subtasks

### 1. Normalize the FFleet sentinel contract
- Decide on one explicit representation for “no destination” and “no jump route”.
- Apply that contract consistently to the FFleet data members, constructor defaults, accessors, mutators, transit completion, and cancel-jump behavior.
- Keep save/load behavior aligned with the chosen representation so fleets round-trip cleanly.

Acceptance criteria:
- The fleet class no longer mixes unsigned storage with raw `-1` sentinel assignments.
- Destination and jump-route state transitions use the same explicit convention from construction through transit completion.
- Serialization and deserialization preserve the chosen sentinel semantics.

### 2. Update callers that depend on destination or route IDs
- Audit the known destination readers and route readers so they handle the explicit no-value state correctly.
- Remove any raw `-1` comparisons or unsafe assumptions about always-valid destination/route IDs in the affected call sites.
- Keep the UI and strategic flow consistent when a fleet is not in transit and when it is mid-jump.

Acceptance criteria:
- The affected callers compile against the updated FFleet contract without signed/unsigned mismatch warnings.
- Code paths that dereference destination or route IDs no longer assume validity when the fleet is not in transit.
- Any necessary guard logic is localized to the smallest practical set of caller files.

### 3. Repair and extend FFleet regression coverage
- Update `tests/strategic/FFleetTest.cpp` so the constructor, `setLocation`, `decTransitTime`, and `cancelJump` assertions match the explicit sentinel contract.
- Add coverage for both destination and jump-route sentinel behavior, not just one field.
- Verify the tests capture the expected transitional behavior when a fleet starts a jump, completes a jump, and cancels a jump.

Acceptance criteria:
- The strategic test suite reflects the new contract instead of asserting raw `-1` against unsigned values.
- The regression tests cover constructor state, transit state, arrival state, and cancel-jump state.
- The test build is free of the sentinel-related signed/unsigned comparison warnings in the touched coverage.

## Dependency Ordering
1. Subtask 1 must happen first because it defines the contract that everything else follows.
2. Subtask 2 depends on Subtask 1 and can be worked in parallel with Subtask 3 once the contract is fixed.
3. Subtask 3 depends on Subtask 1 and should be validated against the final caller behavior from Subtask 2.

## Implementer Agent Prompts

### Prompt 1
You are the implementer agent.

Files you are allowed to change: `include/strategic/FFleet.h` and `src/strategic/FFleet.cpp`.

Task: Normalize the FFleet sentinel contract so destination and jump-route state use one explicit convention instead of mixing unsigned storage with raw `-1` assignments.

Acceptance criteria:
- The fleet class uses a consistent explicit no-destination/no-route representation.
- Constructor defaults, `setLocation`, `decTransitTime`, `cancelJump`, and save/load all agree on that representation.
- The chosen contract preserves correct behavior for fleets in transit and fleets already at their destination.

### Prompt 2
You are the implementer agent.

Files you are allowed to change: `src/strategic/FGame.cpp`, `src/gui/SystemDialogGUI.cpp`, `src/gui/WXPlayerDisplay.cpp`, and `src/gui/ViewFleetGUI.cpp` if needed for the chosen contract.

Task: Update destination and jump-route call sites so they respect the explicit FFleet sentinel contract and do not assume those IDs are always valid.

Acceptance criteria:
- Caller code no longer relies on raw `-1` comparisons against unsigned fleet IDs.
- Code paths that use destination or jump-route IDs are safe when a fleet is not in transit.
- The touched caller files compile without sentinel-related signed/unsigned warnings.

### Prompt 3
You are the implementer agent.

Files you are allowed to change: `tests/strategic/FFleetTest.cpp`.

Task: Repair and extend the FFleet tests so they verify the explicit destination and jump-route sentinel contract across construction, transit, arrival, and cancel-jump behavior.

Acceptance criteria:
- The tests assert the updated contract instead of comparing unsigned values to `-1`.
- Destination and jump-route behavior are both covered.
- The tactical/strategic test build passes cleanly with the updated assertions.

## Output Artifact
- Written plan file: `plans/ffleet-destination-sentinel-mismatch-plan.md`
