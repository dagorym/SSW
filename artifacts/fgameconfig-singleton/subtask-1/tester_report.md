# Tester Report — Subtask 1: FGameConfig Singleton Lifecycle

## Summary
- **Tests run:** 159
- **Failures:** 1 (pre-existing, out of scope)
- **New tests added:** 3
- **All new tests:** PASS

## Acceptance Criteria Results

| # | Criterion | Result |
|---|-----------|--------|
| AC1 | `getGameConfig()` lazily initialises singleton when unset | ✅ PASS |
| AC2 | Singleton lifecycle consistent without unsafe external deletion | ✅ PASS |
| AC3 | Public static `reset()` exists and is documented as test-only | ✅ PASS |
| AC4 | Production callers observe same base-path behavior | ✅ PASS |

## Tests Written

### testGetGameConfigLazyCreatesSingleton
Calls `getGameConfig()` after `reset()` without prior `create()` and asserts the returned config has a non-empty base path. Verifies AC1.

### testResetRecreatesSingletonAndMaintainsContract
Verifies that before reset, `create()` and `getGameConfig()` return the same instance. After `reset()`, verifies the singleton is functional and self-consistent. Uses behavioral assertions rather than pointer-address comparison (which is unreliable after delete). Verifies AC2/AC3.

### testBasePathConsistentForProductionCallers
Calls `create()` then `getGameConfig()` and asserts both return the same base path string. Verifies AC4.

## Fixture Lifecycle Updates
- `setUp()` now calls `FGameConfig::reset()` before `create()` for a clean slate.
- `tearDown()` now uses `FGameConfig::reset()` instead of raw `delete`.
- `testSerialize()` uses `reset()` instead of manual `delete` mid-test.

## Pre-existing Failure (out of scope)
- **testConstructor** fails with hardcoded path assertion: `basePath == "/home/tstephen/Development/SSW/"`
- This is not introduced by Subtask 1; it is addressed by Subtask 2.

## Branch / Commit
- Branch: weapon-tests-tester-20260324
- Commit: 09264aef0b6875bf67805110463a61f5f5028b21
