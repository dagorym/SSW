# Station Placement Duplication Plan

## Feature Restatement
Prevent tactical station setup from creating multiple map occupants when the user clicks a second hex before choosing the station's orbital direction. Station setup should leave exactly one station instance on the tactical map and keep the placement flow in a valid pre-orbit-selection state.

## Confirmed Repository Facts
- Station placement is handled in [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp:1338) by `FTacticalGame::placeStation(const FPoint & hex)`.
- `placeStation(...)` currently pushes `m_station` directly into `m_hexData[hex].ships`, records `m_shipPos`, and advances to `PH_SET_SPEED`.
- The tactical model already tracks selected placement hex state through `m_shipPos` and uses a two-step placement pattern for ships via `m_setRotation`, `setShipPlacementHeading(...)`, and `setShipPlacementHeadingByHex(...)`.
- Existing targeted tactical regression coverage already exists in:
  - [`tests/tactical/FTacticalStationOrbitalMovementTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalStationOrbitalMovementTest.cpp:1) for station placement/orbit flow
  - [`tests/tactical/FTacticalShipPlacementOrientationTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalShipPlacementOrientationTest.cpp:1) for two-step placement/orientation behavior
- The display prompt for station placement currently lives in [`src/tactical/FBattleDisplay.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp:260), but the duplicate-placement defect is rooted in model behavior rather than prompt text.

## Assumptions
- The intended UX is to allow the player to change the pending station hex before finalizing orbital direction, not to permanently commit the first clicked adjacent hex.
- The most conservative implementation is to keep only one station occupant in `m_hexData` by removing any prior pending station placement before recording a new one, rather than introducing a broader station-specific setup state machine.
- No user-facing documentation update is required because this is a correctness fix to existing tactical setup behavior rather than a rules change.

## Files To Modify
- Likely production files:
  - `src/tactical/FTacticalGame.cpp`
  - `include/tactical/FTacticalGame.h` only if a small helper declaration is needed to keep pending-station placement cleanup explicit
- Likely tester follow-up files:
  - `tests/tactical/FTacticalStationOrbitalMovementTest.cpp`
  - `tests/tactical/FTacticalShipPlacementOrientationTest.cpp` only if the implementer reuses the generic placement-orientation seam

## Overall Documentation Impact
No documentation update is expected. Downstream documentation review should only confirm that no design note or user guide text describes the buggy duplicate-placement behavior.

## Subtasks

### SSW-SPD-001: Normalize Pending Station Placement To A Single Map Occupant
Update tactical station setup so repeated pre-orbit placement clicks cannot leave more than one station instance on the map while preserving the existing station setup flow into orbital direction selection and battle-start initialization.

Acceptance Criteria:
- Clicking a valid adjacent station hex during `BS_SetupStation` places the station at that hex and records it as the current pending station position.
- Clicking a different valid adjacent station hex before orbital direction is finalized moves the pending station placement instead of leaving a second station instance behind.
- After any sequence of pre-orbit station placement clicks, exactly one copy of `m_station` exists across all tactical hex occupant lists.
- Invalid station placement clicks still fail without adding or moving a station occupant.
- Existing station setup flow still supports downstream orbit initialization and movement bookkeeping once placement is finalized.
- Non-station ship placement behavior remains unchanged.

Documentation Impact:
No documentation update expected; this should remain an implementation-only correction.

## Dependency Ordering
1. `SSW-SPD-001` is the only required implementation subtask.

Parallelization Guidance:
No parallelization. The fix is a single tactical-model change with overlapping validation context.

## Implementer Agent Prompts

### Implementer Prompt: SSW-SPD-001
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h` only if a helper declaration is needed for explicit pending-station cleanup

Implement the tactical station-placement fix so that station setup never leaves multiple copies of the same station on the tactical map when the user clicks a second adjacent hex before choosing orbital direction. Preserve the existing station setup flow and keep the change narrowly scoped to the model seam responsible for placement state.

Acceptance criteria:
- A valid station placement click during `BS_SetupStation` records exactly one pending station occupant on the map.
- A second valid adjacent placement click before orbital direction finalization relocates that pending station occupant instead of creating a duplicate.
- Invalid clicks do not create a station occupant or corrupt the pending station position.
- Station orbit initialization still works from the final selected hex after the fix.
- Non-station ship placement and rotation flow are unchanged.

Validation guidance:
- Smallest likely validation scope: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If a more targeted runner is preferable locally, treat `tests/tactical` as the required validation area and note the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalStationOrbitalMovementTest.cpp`
  - `tests/tactical/FTacticalShipPlacementOrientationTest.cpp` if shared placement-orientation behavior is involved

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/station-placement-duplication`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
- `plans/station-placement-duplication-plan.md`
