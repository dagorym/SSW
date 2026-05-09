# Damage Fallback Hull Plan

## Feature Restatement
Bring tactical advanced-damage resolution back into line with the Tactical Operations Manual so that non-hull hits convert to normal weapon hull damage when they target a system that cannot take fresh damage, with the user-specified special handling for `Disastrous Fire`.

## Confirmed Repository Facts
- The advanced damage table is implemented in [`src/ships/FVehicle.cpp`](/home/tstephen/repos/SSW/src/ships/FVehicle.cpp:404) in `FVehicle::advancedDamage(...)`.
- Weapon-hit and defense-hit entries already fall back to hull damage when no undamaged matching component is found.
- ADF, MR, power-system, combat-control, navigation, electrical-fire, and DCR entries currently mutate state directly without converting repeated/undamageable hits to hull.
- `Disastrous Fire` is implemented as a bundled branch in the same method and currently reapplies all bundled effects unconditionally.
- Attack-result and combat-report plumbing already carry hull-damage and non-hull effects forward from `FTacticalDamageResolution`; no separate reporting layer appears to own the rules bug.

## Assumptions
- No product-facing documentation change is required because the repository rules doc already describes the intended behavior.
- No combat-report schema change is required if `FVehicle::advancedDamage(...)` emits the correct hull-vs-non-hull effects after the rules fix.
- The most relevant tester-owned coverage lives in `tests/ships/FVehicleTest.cpp`, `tests/ships/FTacticalAttackIntegrationTest.cpp`, and possibly `tests/tactical/FTacticalCombatReportTest.cpp` for indirect summary/regression confirmation.

## Files To Modify
- Likely production files:
  - `src/ships/FVehicle.cpp`
  - `include/ships/FVehicle.h` if helper declarations are needed for cleaner damageability/fallback handling
- Likely tester follow-up files:
  - `tests/ships/FVehicleTest.cpp`
  - `tests/ships/FTacticalAttackIntegrationTest.cpp`
  - `tests/tactical/FTacticalCombatReportTest.cpp` if any report-level regression case is needed

## Overall Documentation Impact
No documentation update is expected. The rules text in `doc/rules/tactical_operations_manual.md` already states the intended behavior; downstream documentation review should only confirm that no implementation note or artifact doc needs to mention the bug fix.

## Subtasks

### SSW-DFH-001: Enforce Hull Fallback For Standard Non-Hull Damage Entries
Update advanced-damage resolution so that non-hull results outside `Disastrous Fire` convert to normal weapon hull damage whenever the targeted system cannot take a fresh hit under the rules.

Acceptance Criteria:
- ADF results (`-1 ADF`, `1/2 ADF`, `all ADF`) still apply normally while current ADF is above `0`, even if ADF was previously damaged.
- MR results (`-1 MR`, `all MR`) still apply normally while current MR is above `0`, even if MR was previously damaged.
- ADF results convert to normal weapon hull damage when current ADF is already `0`.
- MR results convert to normal weapon hull damage when current MR is already `0`.
- Repeated hits on already-active single-state systems convert to normal weapon hull damage instead of reasserting the same state:
  - power short circuit
  - combat control
  - navigation
  - electrical fire
  - damage control
- Existing weapon-hit and defense-hit fallback behavior remains intact.
- The emitted `FTacticalDamageResolution` / attack-result data for converted hits reflects hull damage rather than a zero-effect non-hull event.

Documentation Impact:
No documentation update expected; downstream doc review should only confirm that no implementation notes require revision.

### SSW-DFH-002: Implement Special Reapplication Rules For `Disastrous Fire`
Adjust the `Disastrous Fire` branch so it reapplies only the still-applicable bundled effects and converts to hull damage only when the bundle has nothing left to newly damage.

Acceptance Criteria:
- `Disastrous Fire` reapplies its bundled effects to any listed subsystem that is currently functional or undamaged.
- `Disastrous Fire` restarts the fire if the ship is not currently on fire.
- `Disastrous Fire` does not convert to hull damage merely because some bundled effects were already damaged; it still applies any remaining eligible bundled effects.
- `Disastrous Fire` converts to normal weapon hull damage only when all of its listed subsystem effects are already in the damaged state and the ship is already on fire.
- When `Disastrous Fire` partially reapplies bundled effects, the emitted damage-resolution data contains only the effects that actually changed state and does not add a hull-damage event.
- When `Disastrous Fire` fully falls back to hull, the emitted damage-resolution data reports hull damage rather than a bundle of zero-effect non-hull events.

Documentation Impact:
No documentation update expected; downstream doc review should confirm the implementation now matches the existing rules text and user clarification.

## Dependency Ordering
1. `SSW-DFH-001` must complete first because it establishes the reusable decision logic for when a standard non-hull result is still damageable versus when it must fall back to hull.
2. `SSW-DFH-002` follows `SSW-DFH-001` because the `Disastrous Fire` branch should reuse the same damageability/fallback conventions and avoid duplicating inconsistent logic.

Parallelization Guidance:
These subtasks should be executed sequentially. They both touch `src/ships/FVehicle.cpp`, rely on shared rules interpretation, and would otherwise risk conflicting helper structure and fallback semantics.

## Implementer Agent Prompts

### Implementer Prompt: SSW-DFH-001
You are the implementer agent

Allowed files:
- `src/ships/FVehicle.cpp`
- `include/ships/FVehicle.h` only if helper declarations are required to keep the damageability/fallback logic coherent

Task:
Update standard advanced-damage resolution in `FVehicle::advancedDamage(...)` so non-hull entries outside the `Disastrous Fire` branch convert to normal weapon hull damage whenever the targeted system cannot take a fresh hit under the tactical rules. Preserve cumulative behavior for ADF/MR while their current values remain above zero, and preserve existing weapon-hit/defense-hit component-selection behavior.

Acceptance criteria:
- ADF and MR entries continue to apply as cumulative point loss while current values remain above zero.
- ADF entries convert to normal weapon hull damage when current ADF is already zero.
- MR entries convert to normal weapon hull damage when current MR is already zero.
- Repeated hits on already-damaged single-state systems (`power short circuit`, `combat control`, `navigation`, `electrical fire`, `damage control`) convert to normal weapon hull damage instead of reasserting the same state.
- Weapon-hit and defense-hit fallback behavior remains unchanged for ships that still have an eligible undamaged component.
- Damage-resolution output for converted hits records hull damage, not a zero-change non-hull effect.

Validation guidance:
- Smallest likely validation scope: `cd tests/ships && make && ./ShipsTests`
- Additional likely targeted validation if attack-result plumbing is affected: `cd tests/ships && make && ./ShipsTests`
- If ship-runner naming or scope differs locally, treat `tests/ships` as the required validation area and note the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/ships/FVehicleTest.cpp`
  - `tests/ships/FTacticalAttackIntegrationTest.cpp`

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/damage-fallback-hull`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: SSW-DFH-002
You are the implementer agent

Allowed files:
- `src/ships/FVehicle.cpp`
- `include/ships/FVehicle.h` only if helper declarations are required to support `Disastrous Fire` bundle application cleanly

Task:
Update the `Disastrous Fire` branch in `FVehicle::advancedDamage(...)` to follow the clarified bundle semantics: reapply its listed subsystem effects only where they can still newly take effect, restart the fire if it is no longer active, and convert the result to normal weapon hull damage only when the ship is already on fire and every listed subsystem effect is already in its damaged state.

Acceptance criteria:
- `Disastrous Fire` reapplies ADF, MR, DCR, combat-control, and fire effects only where those effects can still newly change state.
- `Disastrous Fire` restarts the fire when the ship is not currently on fire.
- `Disastrous Fire` does not fall back to hull while any bundled subsystem effect can still be newly applied.
- `Disastrous Fire` falls back to normal weapon hull damage only when no bundled effect can newly apply and the ship is already on fire.
- Partial reapplication reports only the effects that actually changed state.
- Full fallback reports hull damage instead of zero-effect bundled status events.

Validation guidance:
- Smallest likely validation scope: `cd tests/ships && make && ./ShipsTests`
- Additional likely validation if downstream tactical summaries depend on the emitted resolution shape: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If exact runner commands differ locally, treat `tests/ships` plus the tactical standalone runner as the intended validation scope and note the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/ships/FVehicleTest.cpp`
  - `tests/ships/FTacticalAttackIntegrationTest.cpp`
  - `tests/tactical/FTacticalCombatReportTest.cpp` if summary-level indirect behavior needs explicit regression coverage

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/damage-fallback-hull`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
- `plans/damage-fallback-hull-plan.md`
