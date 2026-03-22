# Code Cleanup Review

Date: 2026-03-22
Repository root: `SSW/`

## Scope

This review covered:
- Repository instructions and build layout
- A clean application build from the repository root
- A clean test build from `tests/`
- A runtime test pass with `./SSWTests`
- Targeted inspection of warning sites and failed test paths

## Executive Summary

The repository builds successfully, but there are several correctness and maintainability issues that should be addressed before treating the codebase as clean:
- One live code path uses an unsafe `FFleet` copy constructor that can double-delete ships.
- The test runner returns success when tests fail.
- Tactical battery range computation appears to clamp bounds incorrectly.
- `FFleet` uses unsigned fields with `-1` sentinel semantics, which leaks into tests and warnings.
- `FGameConfig` singleton usage and tests are brittle.
- The base `FWeapon` tests are inconsistent with current `FWeapon` behavior.

There are also lower-priority build hygiene issues:
- Several unused local variables in tactical GUI code
- Repeated `ar` warnings caused by obsolete `u` usage
- Coverage flags enabled in normal builds, producing `libgcov` noise during test execution
- Test-time image loading warnings likely caused by missing wx image handler initialization

## Findings

### 1. Unsafe `FFleet` Copy Constructor

Severity: High

References:
- `src/strategic/FFleet.cpp:43`
- `src/strategic/FFleet.cpp:70`
- `src/gui/TransferShipsGUI.cpp:370`

Details:
- `FFleet::FFleet(const FFleet &)` shallow-copies `m_ships`.
- `FFleet::~FFleet()` deletes every ship in `m_ships`.
- The same ship pointers can therefore be owned by multiple `FFleet` instances.
- The copy constructor is used in fleet-splitting UI logic, so this is not dead code.

Impact:
- Double delete / heap corruption risk
- Shared mutable ship state across fleets
- Unclear ownership semantics

Recommended fix:
- Either implement a deep copy of ships, or delete the copy constructor and introduce an explicit clone routine with well-defined ownership.

### 2. Test Runner Exit Code Is Inverted

Severity: High

References:
- `tests/SSWTests.cpp:101`
- `tests/SSWTests.cpp:102`

Details:
- `runner.run()` returns `true` on success and `false` on failure.
- The current code returns that bool directly as the process exit code.
- In practice, failed tests return `0`, which is interpreted as success by shells and CI.

Observed behavior:
- `./SSWTests` reported 3 failures.
- The process still exited with code `0`.

Recommended fix:
- Return `wasSuccessful ? 0 : 1;`

### 3. Tactical Battery Range Bounds Are Clamped Incorrectly

Severity: High

References:
- `src/tactical/FBattleBoard.cpp:1010`
- `src/tactical/FBattleBoard.cpp:1014`

Details:
- `xMax` is set to `m_nCol` when `xMax < m_nCol`.
- `yMax` is set to `m_nRow` when `yMax < m_nRow`.
- This expands the search area to the board edge instead of capping it.
- The loops then iterate into off-map coordinates.

Impact:
- Incorrect targeting/range overlays
- Potential invalid tactical positions in range lists

Recommended fix:
- Clamp upper bounds only when they exceed the valid maximum.
- Likely use `m_nCol - 1` and `m_nRow - 1` as the inclusive maxima if indices are zero-based.

### 4. `FFleet` Destination API Uses Unsigned Values with `-1` Sentinel Semantics

Severity: Medium

References:
- `include/strategic/FFleet.h:302`
- `include/strategic/FFleet.h:304`
- `src/strategic/FFleet.cpp:137`
- `tests/strategic/FFleetTest.cpp:70`
- `tests/strategic/FFleetTest.cpp:157`

Details:
- `m_destination` is exposed as unsigned.
- The implementation assigns `-1` to indicate "no destination".
- Tests compare the unsigned result to `-1`, producing signed/unsigned warnings.

Impact:
- Ambiguous API contract
- Warning noise
- Sentinel values hidden behind unsigned wraparound

Recommended fix:
- Use a signed type for destination and route identifiers if they need sentinel values, or
- Keep them unsigned and define a named sentinel constant such as `kNoDestination`.

### 5. `FGameConfig` Singleton and Tests Are Brittle

Severity: Medium

References:
- `src/core/FGameConfig.cpp:24`
- `src/core/FGameConfig.cpp:31`
- `tests/core/FGameConfigTest.cpp:21`
- `tests/core/FGameConfigTest.cpp:27`

Details:
- `getGameConfig()` dereferences `m_config` without validating it.
- The test manually deletes the singleton in teardown.
- The test also hardcodes a machine-specific absolute path.

Observed failure:
- `FGameConfigTest::testConstructor` currently fails because it expects `/home/tstephen/Development/SSW/`.

Impact:
- Environment-specific test failure
- Fragile singleton lifecycle
- Undefined behavior if `getGameConfig()` is called before `create()`

Recommended fix:
- Make singleton access safer and consistent.
- Change the test to assert portable behavior, such as path validity or path suffix expectations.
- Avoid manually deleting the singleton in tests unless the singleton API explicitly supports reset semantics.

### 6. Base `FWeapon` Tests Are Out of Sync with Current Implementation

Severity: Medium

References:
- `tests/weapons/FWeaponTest.cpp:34`
- `tests/weapons/FWeaponTest.cpp:109`
- `tests/weapons/FWeaponTest.cpp:132`
- `src/weapons/FWeapon.cpp:191`

Details:
- The constructor test asserts that a default `FWeapon` has range `0`.
- Later tests expect `setTarget(v, 3, true)` to succeed on the same default object.
- `FWeapon::setTarget()` correctly rejects targets beyond `m_range`.

Observed failures:
- `FWeaponTest::testSetTarget`
- `FWeaponTest::testFireAtTarget`

Impact:
- False-negative test failures
- Confusion about base-class semantics

Recommended fix:
- Update these tests to use a concrete weapon type with non-zero range, or
- Revisit whether the base `FWeapon` class should permit target assignment for test scaffolding.

## Build And Test Results

### Clean Build

Commands run:
- `make clean`
- `make all`

Result:
- Build succeeded for `SSW` and `BattleSim`

Warnings observed during build:
- Unused locals in tactical code
- `ar: 'u' modifier ignored since 'D' is the default`

### Test Build

Command run:
- `make` from `tests/`

Result:
- Test build succeeded

Compiler warning observed:
- Signed/unsigned comparison warnings in `FFleetTest.cpp`

### Test Run

Command run:
- `./SSWTests`

Reported result:
- Run: 141
- Failures: 3
- Errors: 0

Failing tests:
- `FrontierTests::FGameConfigTest::testConstructor`
- `FrontierTests::FWeaponTest::testSetTarget`
- `FrontierTests::FWeaponTest::testFireAtTarget`

Additional runtime noise:
- Repeated wx image load warnings for valid PNG assets
- `libgcov` checksum mismatch messages

## Warning Cleanup Recommendations

### Unused Local Variables

References:
- `src/tactical/FBattleBoard.cpp:958`
- `src/tactical/FBattleScreen.cpp:90`
- `src/tactical/FBattleDisplay.cpp:710`

Recommendation:
- Remove unused `wxColour` locals or use them if they were intended for future drawing logic.

### Obsolete `ar rcsu` Usage

Example references:
- `src/gui/Makefile:52`
- `src/tactical/Makefile:45`
- `tests/tactical/Makefile:29`

Recommendation:
- Replace `ar rcsu` with `ar rcs`.

### Coverage Flags Enabled in Normal Builds

Example references:
- `src/Makefile:1`
- `src/gui/Makefile:9`
- `src/tactical/Makefile:9`

Details:
- Normal builds currently compile with `-fprofile-arcs -ftest-coverage`.
- This contributes to `libgcov` checksum mismatch noise when binaries and stale `.gcda` files do not align.

Recommendation:
- Gate coverage instrumentation behind a dedicated make variable or target instead of enabling it unconditionally.

### Missing wx Image Handler Initialization in Tests

References:
- `src/FApp.cpp:24`
- `tests/SSWTests.cpp:54`

Details:
- Application startup initializes image handlers.
- The test runner does not appear to do the same.
- The test run emitted many PNG load warnings even though the image files are valid.

Recommendation:
- Initialize wx image handlers in the test bootstrap if image loading is expected during tests.

### Warning Policy

Current warning posture:
- The project broadly uses `-Wall -Woverloaded-virtual`

Recommendation:
- After current warnings are cleaned up, add `-Wextra` in a controlled pass.
- Evaluate `-Wconversion` separately after sentinel and signedness cleanup.
- Avoid turning on `-Werror` until the warning baseline is stable.

## Suggested Cleanup Order

1. Fix the test runner exit code so failures are actionable in automation.
2. Fix the unsafe `FFleet` copy behavior.
3. Correct tactical battery range bounds logic.
4. Normalize `FFleet` destination and route sentinel handling.
5. Repair the broken tests (`FGameConfigTest`, `FWeaponTest`) so the suite reflects current behavior.
6. Remove build warning noise and separate coverage instrumentation from normal builds.
7. Improve test bootstrap behavior for wx image loading.

## Planner Agent Prompts

Use the following issue-focused prompts with the Planner agent.

### Prompt 1: Unsafe `FFleet` Copy Constructor

```text
Plan a fix for an unsafe fleet-copying issue in the SSW repository.

Issue:
- `FFleet::FFleet(const FFleet &)` in `src/strategic/FFleet.cpp:43` shallow-copies `m_ships`
- `FFleet::~FFleet()` in `src/strategic/FFleet.cpp:70` deletes every ship in `m_ships`
- `src/gui/TransferShipsGUI.cpp:370` uses `new FFleet(*m_fleet)` in active GUI logic

Problem:
- Multiple `FFleet` instances can own the same `FVehicle*`
- This creates double-delete risk and shared mutable state

Focus areas:
- Decide whether to deep-copy ships, remove copy support, or replace it with explicit clone logic
- Identify other call sites that depend on fleet copying
```

### Prompt 2: Test Runner Exit Code Is Inverted

```text
Plan a fix for a test infrastructure issue in the SSW repository.

Issue:
- `tests/SSWTests.cpp:101-102` stores the result of `runner.run()` and returns that bool directly
- In practice, test failures currently produce process exit code 0

Observed behavior:
- `./SSWTests` reported 3 failures but the process exit code still indicated success
```

### Prompt 3: Tactical Battery Range Clamp Bug

```text
Plan a fix for a tactical gameplay logic issue in the SSW repository.

Issue:
- `src/tactical/FBattleBoard.cpp:1010-1015` clamps `xMax` and `yMax` backwards in `computeBatteryRange`
- The code appears to expand range checks to board edges instead of capping to valid indices

Potential impact:
- Incorrect targeting overlays
- Off-map positions entering tactical range computations

Focus areas:
- Confirm intended inclusive/exclusive board bounds from nearby board code
- Include edge and corner range cases in the verification plan
```

### Prompt 4: `FFleet` Destination Sentinel Mismatch

```text
Plan a fix for an API consistency issue in the SSW repository.

Issue:
- `include/strategic/FFleet.h:302-304` exposes destination as unsigned
- `src/strategic/FFleet.cpp:137` assigns `-1` as a sentinel value
- `tests/strategic/FFleetTest.cpp:70,157` compare the returned value to `-1`

Problem:
- The API contract is inconsistent
- Signed/unsigned warnings are produced
- Sentinel meaning is obscured behind wraparound behavior

Focus areas:
- Decide whether to use signed IDs or a named unsigned sentinel
- Identify other route or location IDs that should be normalized at the same time
```

### Prompt 5: `FGameConfig` Singleton And Test Fragility

```text
Plan a fix for a configuration and test reliability issue in the SSW repository.

Issue:
- `src/core/FGameConfig.cpp:24-31` uses a singleton with unsafe access patterns
- `getGameConfig()` dereferences `m_config` without validating it
- `tests/core/FGameConfigTest.cpp:21` manually deletes the singleton in teardown
- `tests/core/FGameConfigTest.cpp:27` hardcodes a machine-specific absolute path

Observed result:
- `FGameConfigTest::testConstructor` currently fails in this workspace because the expected path is environment-specific

Focus areas:
- Distinguish production fixes from test-only cleanup
- Decide whether explicit singleton reset support is warranted for tests
```

### Prompt 6: Base `FWeapon` Tests Are Out Of Sync

```text
Plan a fix for a unit test correctness issue in the SSW repository.

Issue:
- `tests/weapons/FWeaponTest.cpp:34` asserts that a default `FWeapon` has range 0
- `tests/weapons/FWeaponTest.cpp:109,132` expect that same default object to accept a target at range 3 and consume ammo after firing
- `src/weapons/FWeapon.cpp:191` correctly rejects targets beyond `m_range`

Observed result:
- `FWeaponTest::testSetTarget` and `FWeaponTest::testFireAtTarget` currently fail

Focus areas:
- Decide whether the tests should use a concrete weapon type or whether base `FWeapon` semantics should change
- Include any nearby weapon-test cleanup that is tightly coupled to this decision
```

### Prompt 7: Build Warning Cleanup And Makefile Hygiene

```text
Plan cleanup work for build warnings and Makefile hygiene in the SSW repository.

Issues to cover:

1. Unused local variables
- `src/tactical/FBattleBoard.cpp:958-959`
- `src/tactical/FBattleScreen.cpp:89-90`
- `src/tactical/FBattleDisplay.cpp:707-711`

2. Obsolete archive command flags
- Multiple Makefiles use `ar rcsu`, which emits warnings because `u` is ignored by current GNU ar

3. Coverage flags enabled in normal builds
- Several Makefiles always include `-fprofile-arcs -ftest-coverage`
- Test runs emit `libgcov` checksum mismatch noise

4. Test bootstrap image-loading noise
- Application startup initializes wx image handlers in `src/FApp.cpp:24`
- `tests/SSWTests.cpp` does not appear to initialize image handlers
- Test runs emit many image format/load warnings even though icon files are valid PNGs

Focus areas:
- Separate correctness-related cleanup from pure build hygiene
- Decide whether this should be one cleanup pass or several smaller ones
```
