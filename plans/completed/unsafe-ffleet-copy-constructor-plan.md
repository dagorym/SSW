# Plan: Unsafe `FFleet` Copy Constructor

## Scope
Prompt 1 from `plans/code_cleanup.md` only.

## Confirmed Repository Facts
- `FFleet` stores owning raw `FVehicle*` pointers in `m_ships`.
- `FFleet::~FFleet()` deletes every ship in `m_ships`.
- `FFleet::FFleet(const FFleet &)` currently shallow-copies the ship pointers.
- `src/gui/TransferShipsGUI.cpp:370` uses `new FFleet(*m_fleet)` in active GUI flow.
- The current design already exposes `removeShip()` / `addShip()` style ownership transfer behavior.

## Assumptions
- Fleet ownership is intended to be exclusive: one `FVehicle*` should not be owned by multiple fleets.
- The shallow copy is not a desired public API and should be removed rather than preserved.
- Deep-copying ships would require broader `FVehicle` cloning semantics than this prompt needs.

## Files to Modify
- `include/strategic/FFleet.h`
- `src/strategic/FFleet.cpp`
- `src/gui/TransferShipsGUI.cpp`
- `tests/strategic/FFleetTest.cpp`

## Recommended Direction
Disable implicit `FFleet` copying and refactor the GUI to build transfer fleets explicitly. This preserves the existing ownership model, removes double-delete risk, and avoids introducing broad clone infrastructure for `FVehicle`.

## Subtask 1: Remove Implicit `FFleet` Copy Support
Make accidental copying impossible so the double-delete path cannot recur.

### Acceptance Criteria
- `FFleet` no longer allows shallow copy construction or assignment.
- The destructor still owns and deletes its own ships, but copy-based aliasing is blocked.
- Any accidental compilation path that tries to copy a fleet fails clearly at compile time.
- Existing code that needs fleet duplication is forced onto explicit transfer logic instead.

### Implementer Agent Prompt
You are the implementer agent for Subtask 1: remove implicit `FFleet` copy support.

Allowed files:
- `include/strategic/FFleet.h`
- `src/strategic/FFleet.cpp`

Task:
Update `FFleet` so it cannot be copied implicitly. Prefer the narrowest change that makes ownership unambiguous and prevents shallow-copy aliasing of `m_ships`. Keep the destructor ownership behavior intact. Add a brief comment in the header or implementation explaining that fleets own their ships exclusively and therefore cannot be copied safely.

Acceptance criteria:
- Copy construction and assignment are disabled or otherwise rejected by the type system.
- The code no longer admits the unsafe shallow-copy ownership model.
- The change is documented in a short ownership comment.

## Subtask 2: Refactor TransferShipsGUI And Add Regression Coverage
Replace the active GUI copy path with explicit fleet creation and verify the ownership behavior stays safe.

### Acceptance Criteria
- `src/gui/TransferShipsGUI.cpp` no longer uses `new FFleet(*m_fleet)`.
- The new fleet is created explicitly and populated through explicit ship transfer steps.
- Fleet metadata needed by the UI flow is preserved without reintroducing shared ownership.
- `tests/strategic/FFleetTest.cpp` covers the updated ownership contract or at least guards against the old copy-based behavior.

### Implementer Agent Prompt
You are the implementer agent for Subtask 2: refactor `TransferShipsGUI` and add regression coverage.

Allowed files:
- `src/gui/TransferShipsGUI.cpp`
- `tests/strategic/FFleetTest.cpp`

Task:
Replace the GUI flow that currently clones a fleet with explicit creation and transfer logic that respects exclusive ship ownership. Add or update tests so the repository protects the new contract and does not regress back to shared `FVehicle*` ownership.

Acceptance criteria:
- The GUI path builds without using `new FFleet(*m_fleet)`.
- Fleet transfer behavior still works through explicit ownership movement.
- Test coverage reflects the non-copyable fleet contract or the explicit transfer workflow.

## Dependency Ordering
1. Planner-level design decision is already made in this plan: use non-copyable fleets rather than deep copy.
2. Subtask 1 depends only on that decision and can be implemented immediately.
3. Subtask 2 depends on Subtask 1 because the GUI refactor needs the final `FFleet` ownership contract.

## Parallelism
- No meaningful parallel work is available before Subtask 1 completes.
- After Subtask 1 is merged, the GUI refactor and test updates can be coordinated, but they are still sequentially dependent on the final `FFleet` API.

## Output Artifact
- `plans/unsafe-ffleet-copy-constructor-plan.md`
