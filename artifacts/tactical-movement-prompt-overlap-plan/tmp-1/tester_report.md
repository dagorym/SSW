# Tester Report: FBattleScreen layout policy validation

## Scope
Validated the FBattleScreen layout update on branch `layout-update-tester-20260516` for:
- default tactical-screen size (`1200x900`),
- lower-panel baseline allocation (`120px`),
- larger lower-panel request handling,
- map-height minimum floor (`60%`),
- no tactical-state/rules flow changes.

## Assumptions
- Used provided commands as the smallest relevant validation path:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests`
  - `cd tests/gui && make && xvfb-run -a ./GuiTests`
- Pre-existing source-token failures in `FBattleDisplay.cpp` are baseline and unrelated to this FBattleScreen-only implementation, as stated in task context and reflected in current runs.

## Test changes
Modified files:
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.h`
- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`

Added coverage:
- Source-contract checks for FBattleScreen default constructor size (`1200x900`).
- Source-contract checks for layout constants (`120` baseline, `60%` map floor).
- Source-contract checks that `applyLayoutPolicy()` tracks larger display requests, applies baseline, and clamps to preserve map floor.
- Source-contract checks that `onSize(...)` only reapplies layout policy.
- Live GUI runtime check that FBattleScreen default size is `1200x900`, baseline lower-row starts at `120`, larger lower-row requests are honored when possible, and oversized requests are clamped to keep a 60% map floor.

## Command results
1. `cd tests && make tactical-tests && ./tactical/TacticalTests` — **failed**
   - Run: 147, Failures: 3, Errors: 0.
   - All failures are pre-existing source-token expectations in `FTacticalBattleDisplayFireFlowTest` targeting `src/tactical/FBattleDisplay.cpp` (movement-prompt helper tokens).
2. `cd tests/gui && make && xvfb-run -a ./GuiTests` — **failed**
   - Run: 34, Failures: 1, Errors: 0.
   - Failure is pre-existing `TacticalGuiLiveTest` source-token expectation in `src/tactical/FBattleDisplay.cpp`.

## Acceptance criteria status
1. `FBattleScreen` defaults to `1200x900`: **validated** (new tactical source-contract + live GUI size assertion).
2. Parent layout starts lower panel at `120px` when possible: **validated**.
3. Parent layout honors larger lower-panel request when needed: **validated**.
4. Map keeps at least `60%` of client height: **validated**.
5. No tactical rules or battle-state behavior changed: **validated** (layout-only policy assertions plus no new tactical-flow failures).

## Outcome
- New layout-policy acceptance coverage is passing.
- Remaining suite failures are unchanged pre-existing baseline failures outside this FBattleScreen diff.

## Commits
- Test changes commit: `001c853f9833a3b85db701087a956798e00af9cd`

## Cleanup
- No temporary non-handoff byproducts were created.
