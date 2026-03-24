# Base FWeapon Tests Are Out Of Sync Plan

## Confirmed Repository Facts
- `src/weapons/FWeapon.cpp` constructs the base weapon with `m_range = 0`, `m_target = NULL`, and `m_targetRange = -1`.
- `FWeapon::setTarget()` only assigns a target when the requested range is within `m_range`.
- `FWeapon::fire()` clears the target and resets `m_targetRange` to `-1` after a resolved fire, and it refuses to fire when ammo is empty, no target is set, or the target range is invalid.
- `tests/weapons/FWeaponTest.cpp` currently expects the default `FWeapon` to accept a target at range 3 and fire successfully.
- The repository already has concrete weapon tests such as `FAssaultRocketTest`, `FLaserBatteryTest`, and `FWeaponFireResultTest`, so the base-class test is the outlier.

## Assumptions
- The base `FWeapon` class should remain a zero-range placeholder rather than being retrofitted to behave like a concrete weapon.
- The test fix should preserve the current `FWeapon` implementation and instead make the tests reflect actual base-class semantics.
- If a helper harness is needed for targeted behavior checks, it should stay inside the weapon test area and not change production code.

## Files To Modify
- `tests/weapons/FWeaponTest.cpp`
- `tests/weapons/FWeaponTest.h` if the fixture needs a helper weapon/harness type or renamed test cases
- `tests/weapons/Makefile` only if a new test source file is introduced
- possibly `tests/weapons/FWeaponFireResultTest.cpp` if a shared harness pattern is needed for regression coverage

## Subtasks

### 1. Reconcile the base weapon test setup with the real base-class contract
- Decide whether the test fixture should keep using `FWeapon` only for zero-range behavior checks and use a concrete weapon for target/fire behavior, or whether a small test harness subclass is needed to expose a controlled non-zero range.
- Keep the decision localized to the tests so the production `FWeapon` base semantics do not change.
- Make sure the chosen setup still exercises the constructor, target assignment, and fire-path behavior in a way that matches the class under test.

Acceptance criteria:
- The test setup no longer assumes a default `FWeapon` can target or fire at range 3.
- Base-class assertions reflect the actual zero-range placeholder behavior.
- Any non-zero-range behavior is tested with an appropriate concrete weapon or test harness instead of the raw base object.

### 2. Update the failing FWeapon assertions to match the implementation
- Rewrite `testSetTarget()` and `testFireAtTarget()` so they check the documented behavior of `setTarget()` and `fire()` given the chosen test setup.
- Preserve checks for ammo consumption, target clearing, and invalid-target behavior where they are actually supported by the weapon object under test.
- Keep the existing constructor and serialization expectations aligned with the base class’s default state.

Acceptance criteria:
- `testSetTarget()` passes using a weapon object whose range is compatible with the requested target range.
- `testFireAtTarget()` no longer expects the default base object to accept an out-of-range target.
- Constructor, serialization, and invalid-target assertions remain consistent with the actual implementation.

### 3. Add focused regression coverage for base-vs-concrete weapon behavior
- Add or adjust tests so the suite explicitly covers both the base `FWeapon` placeholder contract and a concrete weapon’s target/fire path.
- Include at least one assertion that documents the base weapon’s zero-range limitation so the old expectation does not regress.
- Keep the regression close to the existing weapon test fixture unless a new helper class materially improves clarity.

Acceptance criteria:
- The test suite distinguishes between the base class contract and concrete weapon behavior.
- The regression protects the zero-range base weapon behavior from being mistaken for a concrete weapon again.
- The weapon tests pass without changing production `FWeapon` semantics.

## Dependency Ordering
1. Subtask 1 must happen first because it defines the appropriate test subject for the failing cases.
2. Subtask 2 depends on Subtask 1 and updates the assertions to the real behavior.
3. Subtask 3 depends on Subtasks 1 and 2 and can be used to harden the distinction between base and concrete weapon behavior.

## Implementer Agent Prompts

### Prompt 1
You are the implementer agent.

Files you are allowed to change: `tests/weapons/FWeaponTest.cpp` and `tests/weapons/FWeaponTest.h`.

Task: Reconcile the base weapon test fixture with the real `FWeapon` contract so the failing range-related checks use an appropriate concrete weapon or a small test harness instead of assuming the raw base class has non-zero range.

Acceptance criteria:
- The test setup no longer assumes a default `FWeapon` can fire at range 3.
- Base-class assertions continue to validate the zero-range placeholder behavior.
- The chosen test structure keeps the production `FWeapon` implementation unchanged.

### Prompt 2
You are the implementer agent.

Files you are allowed to change: `tests/weapons/FWeaponTest.cpp`.

Task: Update the failing `FWeapon` assertions so they match the actual `setTarget()` and `fire()` behavior for the corrected test subject.

Acceptance criteria:
- `testSetTarget()` and `testFireAtTarget()` pass against the real implementation.
- Ammo depletion, target clearing, and invalid-target handling are asserted where applicable.
- The constructor and serialization checks remain aligned with the current base-class defaults.

### Prompt 3
You are the implementer agent.

Files you are allowed to change: `tests/weapons/FWeaponTest.cpp`, `tests/weapons/FWeaponTest.h`, and `tests/weapons/Makefile` if a new helper test source file is introduced.

Task: Add regression coverage that clearly distinguishes base `FWeapon` placeholder behavior from concrete weapon fire behavior so the old out-of-sync expectation does not recur.

Acceptance criteria:
- The regression documents the base weapon’s zero-range limitation.
- At least one concrete weapon path is covered for successful target and fire behavior.
- The weapon test build includes any new helper file if one is added.

## Output Artifact
- Written plan file: `plans/fweapon-base-tests-out-of-sync-plan.md`
