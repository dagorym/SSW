# FGameConfig Singleton And Test Fragility Plan

## Confirmed Repository Facts
- `src/core/FGameConfig.cpp` implements `FGameConfig::create()` and `FGameConfig::getGameConfig()` using a raw singleton pointer.
- `getGameConfig()` currently dereferences `m_config` without checking whether the singleton exists.
- `tests/core/FGameConfigTest.cpp` manually deletes the singleton in `tearDown()` and recreates it in `testSerialize()`.
- `tests/core/FGameConfigTest.cpp` hardcodes `/home/tstephen/Development/SSW/` in `testConstructor()`.
- The same config object is used broadly for resource path loading in production code, so changes to its lifecycle affect multiple modules.

## Assumptions
- The production fix should make singleton access safe or explicitly defined instead of relying on tests to manage raw deletion.
- The test should verify portable behavior for `getBasePath()` rather than a workspace-specific absolute path.
- If explicit singleton reset support is introduced, it should exist only as much as needed for tests and should not force unrelated call sites to change.

## Files To Modify
- `include/core/FGameConfig.h`
- `src/core/FGameConfig.cpp`
- `tests/core/FGameConfigTest.cpp`
- `tests/core/FGameConfigTest.h` if the test fixture needs new helper state or cases
- possibly `tests/core/Makefile` if a new core test source file is introduced

## Subtasks

### 1. Harden the singleton lifecycle in production code
- Decide whether `getGameConfig()` should create the singleton on demand, assert/guard with a clear error, or expose a documented reset/reset-like pathway for controlled teardown.
- Make the singleton lifecycle internally consistent so production code does not depend on manual `delete` behavior.
- Keep the change focused on `FGameConfig` rather than spreading lifecycle policy into callers.

Acceptance criteria:
- `getGameConfig()` no longer relies on undefined behavior when called before initialization.
- The singleton lifecycle has one clearly documented path for creation and retrieval.
- Production callers continue to obtain the same base-path behavior after the change.

### 2. Replace the brittle test expectations with portable assertions
- Update `testConstructor()` so it validates the returned base path in a portable way, such as checking that it is non-empty and has the expected repository-relative suffix or resource-root shape.
- Remove the manual singleton deletion from test teardown unless it is explicitly supported by the singleton API.
- Keep serialization coverage intact without depending on a hardcoded workspace path.

Acceptance criteria:
- The test no longer expects `/home/tstephen/Development/SSW/`.
- The fixture no longer manually tears down the singleton unless that teardown is part of the production API contract.
- `testSerialize()` still exercises save/load behavior without relying on environment-specific state.

### 3. Add focused regression coverage for singleton access safety
- Add test coverage for the singleton access pattern chosen in Subtask 1, including the pre-initialization case if the production contract is meant to support it.
- Verify that repeated create/get calls behave consistently and do not depend on hidden test-order state.
- Keep the regression narrowly scoped to `FGameConfig` lifecycle behavior.

Acceptance criteria:
- The regression test covers the chosen singleton access policy explicitly.
- The test suite proves the lifecycle is deterministic across repeated calls.
- Any supported reset behavior is exercised only through the public API, not raw pointer deletion.

## Dependency Ordering
1. Subtask 1 must happen first because it defines the singleton contract.
2. Subtask 2 depends on Subtask 1 and should be updated to match the chosen contract.
3. Subtask 3 depends on Subtask 1 and can run alongside Subtask 2 once the contract is fixed.

## Implementer Agent Prompts

### Prompt 1
You are the implementer agent.

Files you are allowed to change: `include/core/FGameConfig.h` and `src/core/FGameConfig.cpp`.

Task: Harden the `FGameConfig` singleton lifecycle so access is defined and production code does not rely on unsafe raw deletion or undefined access before initialization.

Acceptance criteria:
- `getGameConfig()` has a well-defined behavior when the singleton has not yet been created.
- The singleton lifecycle is internally consistent and does not depend on manual deletion outside the class contract.
- Existing production callers still obtain the same base-path behavior after the change.

### Prompt 2
You are the implementer agent.

Files you are allowed to change: `tests/core/FGameConfigTest.cpp` and `tests/core/FGameConfigTest.h`.

Task: Replace the brittle `FGameConfig` test assertions with portable checks and remove direct singleton deletion from the test fixture unless the production API explicitly supports it.

Acceptance criteria:
- `testConstructor()` no longer hardcodes a machine-specific absolute path.
- The fixture lifecycle follows the supported singleton contract rather than deleting the singleton pointer directly.
- `testSerialize()` still validates save/load behavior under the updated lifecycle rules.

### Prompt 3
You are the implementer agent.

Files you are allowed to change: `tests/core/FGameConfigTest.cpp`, `tests/core/FGameConfigTest.h`, and `tests/core/Makefile` if a new test source file is needed.

Task: Add regression coverage for the chosen `FGameConfig` access policy so repeated create/get usage and any supported reset behavior are verified explicitly.

Acceptance criteria:
- The regression covers the production lifecycle contract from Subtask 1.
- The test suite is deterministic across repeated singleton calls.
- The core test build includes the new coverage if a new test file is added.

## Output Artifact
- Written plan file: `plans/fgameconfig-singleton-and-test-fragility-plan.md`
