# Reviewer Report — milestone1-warning-fixes

## Feature plan reference
- plans/milestone1-warning-fixes-plan.md
- Reviewer artifact directory: artifacts/gui_sep/milestone1-warning-fixes

## Subtasks and artifacts reviewed
- T1 from warning-fixes-T1-verify at /home/tstephen/repos/SSW-T1-verify
  - Reviewed implementation: src/strategic/FPlayer.cpp
  - Reviewed artifacts: tester, documenter, and verifier reports/results under artifacts/gui_sep/milestone1-warning-fixes/T1/
- T2 from warning-fixes-T2-verify at /home/tstephen/repos/SSW-T2-verify
  - Reviewed implementation: include/strategic/FGame.h and src/strategic/FGame.cpp
  - Reviewed tests: tests/strategic/FGameHeaderDependencyTest.h and tests/strategic/FGameHeaderDependencyTest.cpp
  - Reviewed artifacts under artifacts/gui_sep/milestone1-warning-fixes/T2/
- T3 from warning-fixes-T3-verify at /home/tstephen/repos/SSW-T3-verify
  - Reviewed implementation: src/gui/WXIconCache.cpp and include/gui/WXIconCache.h
  - Reviewed tests: tests/core/WXIconCacheTest.h and tests/core/WXIconCacheTest.cpp
  - Reviewed artifacts under artifacts/gui_sep/milestone1-warning-fixes/T3/
- T4 from warning-fixes-T4-verify at /home/tstephen/repos/SSW-T4-verify
  - Reviewed tests: tests/ships/FVehicleTest.cpp and tests/strategic/FFleetTest.cpp
  - Reviewed artifacts under artifacts/gui_sep/milestone1-warning-fixes/T4/

## Overall feature completeness assessment
- T1: complete and consistent with the plan.
- T4: complete and consistent with the plan.
- T3: complete in isolation and its Doxygen update is accurate.
- T2: mostly complete in isolation, but the delivered T2 behavior is not consistent with T3 when the subtasks are combined.

Overall status: feature not complete. The intended milestone end state was a cleaned strategic/wx boundary plus consistent base-path icon loading. That combined behavior is not yet correct.

## Findings

### BLOCKING
1. T2 and T3 use incompatible path-resolution contracts for the turn-counter icons.
   - Plan references:
     - plans/milestone1-warning-fixes-plan.md:34-50
     - plans/milestone1-warning-fixes-plan.md:122-149
     - plans/milestone1-warning-fixes-plan.md:159-179
   - Evidence:
     - In warning-fixes-T2-verify, src/strategic/FGame.cpp:944-948 calls WXIconCache::instance().get(gc.getBasePath()+"icons/tenday.png") and WXIconCache::instance().get(gc.getBasePath()+"icons/day.png").
     - In warning-fixes-T3-verify, src/gui/WXIconCache.cpp:23-25 resolves fullPath = gc.getBasePath() + filename.
     - In warning-fixes-T2-verify, tests/strategic/FGameHeaderDependencyTest.cpp:55-63 asserts those base-path-prefixed calls textually, so the regression test locks in the mismatch.
   - Feature-level impact:
     - After merge, the turn-counter icon loads would become effectively basePath + basePath + icons/... so the day and tenday images are likely to fail to load. This means the delivered feature does not satisfy the combined T2 and T3 intent.

### WARNING
1. The T2 automated regression still does not enforce the wx-free compile contract directly.
   - Plan reference: plans/milestone1-warning-fixes-plan.md:140-149, especially acceptance criterion 2.
   - Evidence:
     - artifacts/gui_sep/milestone1-warning-fixes/T2/verifier_report.md:64-77 already records this as a verifier warning.
     - warning-fixes-T2-verify/tests/strategic/FGameHeaderDependencyTest.cpp:25-53 only performs source-text inspection.
   - Assessment:
     - This is a valid non-blocking follow-up. The manual compile check passed during verification, but the suite would not catch a future regression that breaks a non-wx translation unit while preserving the searched strings.

### NOTE
1. The reviewed evidence is per-subtask, not a single merged-branch verification run.
   - The final merged test count should increase beyond the per-branch counts, but no post-merge feature-level test artifact was provided.
   - This did not prevent review because the T2/T3 integration defect is visible from the delivered branch states.

## Missed functionality and edge cases
- No automated regression currently checks the combined FGame::drawTurnCounter() plus WXIconCache behavior after T3. That is the missed integration case that allowed the blocker through.
- No automated test currently enforces the plan's wx-free translation-unit compile requirement for FGame.h; it is only manually verified.
- T4's exact filename assertions are consistent with the short-filename contract expected by T3 and therefore reinforce, rather than weaken, the conclusion that T2 should not be passing base-path-prefixed names into WXIconCache.

## Planner-ready follow-up feature requests
1. Update FGame::drawTurnCounter() so it passes short relative filenames, icons/tenday.png and icons/day.png, into WXIconCache, and update the T2 regression test to validate that contract instead of validating base-path-prefixed arguments.
2. Add an automated regression that would fail if turn-counter image loading becomes double-prefixed after the T2 and T3 changes are merged.
3. Replace the T2 text-inspection header regression with a compile-based automated check for including strategic/FGame.h from a translation unit that does not include wx headers.
4. After integrating T1 through T4, run and record one feature-level merged build plus tests/SSWTests pass to confirm the final combined behavior and final test count.

## Final outcome
FAIL

Reason: the feature is not yet safe to consider complete because the T2 and T3 deliverables conflict at runtime and violate the intended combined base-path-loading design.

## Commit status
- Reviewer artifacts written: reviewer_report.md and reviewer_result.json
- Reviewer artifact commit: committed; see final response for commit hash
