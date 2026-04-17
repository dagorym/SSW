# Asset Path Resolution Plan

Requested agent: `planner`

Repository-local definition found: `no`  
Shared definition found: `yes`  
Definition path in use: `/home/tstephen/repos/agents/agents/planner.md`

## Feature Restatement

Fix image loading so assets such as `data/zoom.png` are resolved relative to the repository's top-level asset directories rather than the runtime working directory or test-binary location. The fix must remain portable and must not hardcode an absolute system path.

## Confirmed Repository Facts

- `src/tactical/FBattleDisplay.cpp` currently loads the zoom image with `gc.getBasePath()+"data/zoom.png"`.
- `src/core/FGameConfig.cpp` computes `m_basePath` from the executable location, which points at `<repo>/tests/` when test binaries are launched from the `tests/` tree.
- `src/gui/WXIconCache.cpp` already implements a `../` fallback for image loading, which shows the repository already needs repo-root-relative asset lookup when binaries live below the repo root.
- Additional direct asset loads still bypass `WXIconCache`, including:
  - `src/tactical/FBattleBoard.cpp`
  - `src/tactical/FBattleScreen.cpp`
  - `include/gui/WXStartupLaunch.h`
- Existing core tests already cover `FGameConfig` lifecycle behavior in `tests/core/FGameConfigTest.cpp`.

## Assumptions

- The intended runtime layout for local builds and tests remains the current repository layout where `data/` and `icons/` live directly under the repo root.
- A reusable resolver on `FGameConfig` is acceptable as the canonical seam for non-cached asset loads.
- `WXIconCache` should continue to work, but may be updated to use the shared resolver if that keeps path behavior consistent.

## Files To Modify

- `include/core/FGameConfig.h`
- `src/core/FGameConfig.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/WXStartupLaunch.h`
- `src/gui/WXIconCache.cpp`
- `tests/core/FGameConfigTest.h`
- `tests/core/FGameConfigTest.cpp`
- Likely one or more tactical/gui regression files if source-level or runtime assertions are needed to lock the migrated call sites:
  - `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - `tests/core/WXIconCacheTest.cpp`
  - `tests/gui/StrategicGuiLiveTest.cpp`
  - `tests/gui/BattleSimGuiLiveTest.cpp`

## Overall Documentation Impact

- `AGENTS.md` likely does not need a behavioral update unless implementation introduces a new explicit asset-resolution convention worth documenting for contributors.
- Inline code comments may be warranted only where the resolver intentionally falls back from executable-relative lookup to repo-root-relative lookup.

## Subtasks

### AP-01: Add canonical asset-path resolution to `FGameConfig`

Description:
- Extend `FGameConfig` with a reusable asset-resolution API that can take a repo-relative asset path such as `data/zoom.png` or `icons/MaskingScreen.png` and resolve it against the correct repo-root asset directory even when the executable lives under `tests/`.
- Keep the implementation portable and avoid any absolute machine-specific paths.
- Preserve existing `getBasePath()` behavior for callers that still depend on executable-relative paths, while giving production and test code a safer asset-specific resolution seam.

Acceptance Criteria:
- `FGameConfig` exposes a public resolver suitable for direct image-loading call sites.
- The resolver returns a usable path for assets under repo-root `data/` and `icons/` when the executable is launched from the main build tree and when it is launched from the `tests/` tree.
- The implementation does not hardcode this machine's absolute repository path.
- Existing `getBasePath()` lifecycle behavior remains intact for current callers and tests that depend on it.

Documentation Impact:
- No standalone documentation update expected.
- A brief code comment is appropriate if fallback ordering would otherwise be non-obvious.

### AP-02: Migrate direct image-loading call sites to the shared resolver

Description:
- Replace direct `getBasePath()+"..."` image-loading patterns at known non-cache call sites with the new `FGameConfig` asset resolver.
- Cover the current tactical failure (`data/zoom.png`) and the other direct image loads already identified so asset behavior is consistent across tactical and startup surfaces.
- Keep `WXIconCache` aligned with the shared resolver so cached and uncached image loads do not diverge.

Acceptance Criteria:
- `FBattleDisplay` loads `zoom.png` through the shared resolver rather than constructing a raw `getBasePath()+"data/zoom.png"` string.
- `FBattleBoard`, `FBattleScreen`, and `WXStartupLaunch` no longer depend on raw executable-relative asset concatenation for their direct image loads.
- `WXIconCache` uses the same underlying resolution policy, or is otherwise updated so cached icon loads and direct image loads remain behaviorally consistent.
- No image-loading call site introduced by this change relies on a hardcoded absolute system path.

Documentation Impact:
- No standalone documentation update expected.
- If code comments are added, they should explain why direct callers route through the shared resolver instead of repeating local fallback logic.

### AP-03: Add regression coverage for repo-root-relative asset resolution

Description:
- Add or update focused regression tests so asset resolution is locked at the shared seam and the tactical zoom-image regression cannot silently return.
- Prefer testing the resolver at the `FGameConfig` seam and only add tactical/gui source- or runtime-level assertions where they materially protect the migrated call sites.

Acceptance Criteria:
- Core regression coverage proves that the new `FGameConfig` resolver can produce a valid path to a known repo asset without assuming the executable already lives in the repo root.
- Regression coverage protects the tactical zoom-image path from reverting to direct `getBasePath()+"data/zoom.png"` concatenation.
- If `WXIconCache` changes, tests are updated so the cache contract remains consistent with the shared resolver behavior.
- Validation scope remains focused; no new broad end-to-end workflow is required to prove the fix.

Documentation Impact:
- No standalone documentation update expected.
- Test comments may briefly document the executable-location regression being locked down.

## Dependency Ordering

1. `AP-01` must complete first.
2. `AP-02` depends on `AP-01` because the migrated call sites need the new shared resolver.
3. `AP-03` depends on `AP-01` and `AP-02` so the tests can target the final resolver API and migrated call sites.

Parallelization guidance:
- Do not parallelize these subtasks.
- The resolver contract must be established before callers migrate, and the regression coverage should target the final contract rather than an intermediate implementation.

## Implementer Agent Prompts

### Implementer Prompt: AP-01

You are the implementer agent

Allowed files:
- `include/core/FGameConfig.h`
- `src/core/FGameConfig.cpp`

Task:
- Add a reusable asset-path resolution API to `FGameConfig` for repo-relative asset paths such as `data/zoom.png` and `icons/MaskingScreen.png`.
- The resolver must work when binaries are launched from the repo root build outputs and when test binaries are launched from the `tests/` tree.
- Keep the implementation portable and do not hardcode an absolute system path.
- Preserve existing `getBasePath()` lifecycle behavior for current callers.

Acceptance criteria:
- `FGameConfig` exposes a public resolver suitable for direct asset-loading callers.
- The resolver can return a usable path to repo-root assets even when the executable lives under `tests/`.
- The implementation does not rely on any machine-specific absolute path.
- Existing singleton and `getBasePath()` behavior remain intact.

Validation guidance:
- Smallest likely validation command: `cd tests/core && make && ./CoreTests`
- If the module-local runner name differs in this repo, use the smallest equivalent core test target and note that assumption in the handoff artifacts.

Expected Tester file locations:
- `tests/core/FGameConfigTest.h`
- `tests/core/FGameConfigTest.cpp`

Artifact directory guidance:
- Use `artifacts/asset-path-resolution` for handoff artifacts unless orchestration provides a different repository-root-relative artifact directory.

Execution instruction:
- Begin implementation immediately if the files above are sufficient and no blocking ambiguity remains. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: AP-02

You are the implementer agent

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleBoard.cpp`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/WXStartupLaunch.h`
- `src/gui/WXIconCache.cpp`
- `include/core/FGameConfig.h`
- `src/core/FGameConfig.cpp`

Task:
- Migrate direct image-loading call sites from raw `getBasePath()+"..."` concatenation to the shared `FGameConfig` asset resolver added in AP-01.
- Cover the tactical zoom image and the other currently known direct image loads so asset-path behavior is consistent across tactical and startup flows.
- Align `WXIconCache` with the shared resolution policy rather than leaving it on a separate fallback implementation.

Acceptance criteria:
- `FBattleDisplay` uses the shared resolver for `data/zoom.png`.
- `FBattleBoard`, `FBattleScreen`, and `WXStartupLaunch` no longer depend on raw executable-relative asset concatenation for direct image loads.
- `WXIconCache` follows the same underlying asset-resolution policy, or equivalent behavior is preserved through a clearly shared seam.
- No affected call site uses a hardcoded absolute machine path.

Validation guidance:
- Smallest likely validation commands:
- `cd tests/core && make && ./CoreTests`
- `cd tests/tactical && make && ./TacticalTests`
- If a display-backed GUI smoke check is practical in the current environment, the likely narrow command is `cd tests/gui && make && xvfb-run -a ./GuiTests`; if not practical, note that assumption explicitly.

Expected Tester file locations:
- `tests/core/WXIconCacheTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- Likely GUI live tests if startup image loading needs direct coverage:
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`

Artifact directory guidance:
- Use `artifacts/asset-path-resolution` for handoff artifacts unless orchestration provides a different repository-root-relative artifact directory.

Execution instruction:
- Begin implementation immediately if the files above are sufficient and no blocking ambiguity remains. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

### Implementer Prompt: AP-03

You are the implementer agent

Allowed files:
- `tests/core/FGameConfigTest.h`
- `tests/core/FGameConfigTest.cpp`
- `tests/core/WXIconCacheTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/BattleSimGuiLiveTest.cpp`
- `include/core/FGameConfig.h`
- `src/core/FGameConfig.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `src/gui/WXIconCache.cpp`

Task:
- Add focused regression coverage for the new asset resolver and for the migrated zoom-image path so the executable-location bug cannot regress.
- Prefer seam-level tests at `FGameConfig` and only add tactical/gui assertions where they materially protect against reverting to raw `getBasePath()+"..."` asset concatenation.

Acceptance criteria:
- Regression coverage proves the new `FGameConfig` asset resolver can locate a known repo asset without assuming the executable already lives at the repo root.
- Regression coverage protects the tactical zoom-image fix from reverting to raw `getBasePath()+"data/zoom.png"` concatenation.
- If `WXIconCache` changed, its tests remain aligned with the shared resolver behavior.
- The new coverage remains focused and does not depend on a new broad end-to-end harness.

Validation guidance:
- Smallest likely validation commands:
- `cd tests/core && make && ./CoreTests`
- `cd tests/tactical && make && ./TacticalTests`
- Add `cd tests/gui && make && xvfb-run -a ./GuiTests` only if the chosen regression coverage actually touches GUI live-test fixtures.

Expected Tester file locations:
- `tests/core/FGameConfigTest.h`
- `tests/core/FGameConfigTest.cpp`
- `tests/core/WXIconCacheTest.cpp`
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
- Possibly `tests/gui/StrategicGuiLiveTest.cpp`
- Possibly `tests/gui/BattleSimGuiLiveTest.cpp`

Artifact directory guidance:
- Use `artifacts/asset-path-resolution` for handoff artifacts unless orchestration provides a different repository-root-relative artifact directory.

Execution instruction:
- Begin implementation immediately if the files above are sufficient and no blocking ambiguity remains. Do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

- `plans/asset-path-resolution-plan.md`
