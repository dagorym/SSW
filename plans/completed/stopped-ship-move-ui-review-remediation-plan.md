# Stopped Ship Move UI Review Remediation Plan

## Feature Restatement
Correct the stopped-ship move preview so the immediately visible PH_MOVE preview state includes the ship's current-heading forward route in addition to the alternate starting facings already shown. The remediation must realign the model-owned preview contract, the regression expectations, and the affected documentation with the governing feature requirement that every legal starting facing is surfaced.

## Confirmed Repository Facts
- The reviewer identified one blocking feature gap: `FTacticalGame::rebuildStoppedShipPreviewRoutes()` skips `heading == turnData->curHeading`, so the stopped-ship preview metadata never includes the current-heading forward route even though that route remains legal through the existing movement buckets (`artifacts/stopped-ship-move-ui/reviewer_report.md` and `src/tactical/FTacticalGame.cpp`).
- `FBattleBoard::drawRoute()` already renders every route returned by `getStoppedShipPreviewRoutes()` during the stopped-ship PH_MOVE preview state, so the current renderer path can surface the missing option once the preview-route data is corrected (`src/tactical/FBattleBoard.cpp`).
- The runtime regression suite currently locks in the incomplete behavior by asserting exactly five preview routes and by requiring every preview heading to differ from the original heading (`tests/tactical/FTacticalMoveRouteSelectionTest.cpp`).
- The repository documentation now mirrors the incomplete interpretation: `doc/UsersGuide.md` describes the highlighted stopped-ship previews as the legal facings, and `doc/DesignNotes.md` describes the additive preview surface as covering only legal alternate headings.
- No separate defect was confirmed for preview-click continuation, MR==0 behavior, or the additive non-wx tactical model boundary; those surfaces should remain unchanged except where they need to acknowledge the restored current-heading preview.

## Assumptions And Design Direction
- Scope is limited to correcting the stopped-ship preview completeness gap identified by the reviewer. Strategic movement, setup UX, and `doc/rules/tactical_operations_manual.md` remain out of scope.
- The existing additive preview-route surface is still the right model contract; the remediation should restore completeness by including the current-heading forward route in that same surface instead of inventing a second renderer-only path.
- Existing preview-click handling should keep working after the fix. The main requirement is that the current-heading route becomes visible in the same preview state as the alternates, not that the stopped-ship interaction model changes again.
- Ships with `MR == 0` must continue to expose no stopped-ship free-rotation preview.
- Documentation updates are expected for repository-authored docs that currently describe the incomplete behavior.

## Likely Files To Modify
- Model preview generation:
  - `src/tactical/FTacticalGame.cpp`
  - `include/tactical/FTacticalGame.h` only if a small additive declaration or comment update is required
- Likely tester follow-up files:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`
- Likely documenter follow-up files:
  - `doc/UsersGuide.md`
  - `doc/DesignNotes.md`

## Documentation Impact
Documentation updates are expected. The remediation should correct `doc/UsersGuide.md` and `doc/DesignNotes.md` so they describe the stopped-ship preview state as including the current-heading forward route alongside the alternate legal facings, while continuing to leave `doc/rules/tactical_operations_manual.md` untouched.

## Subtasks

### SSW-SSMR-001: Restore The Current-Heading Route To The Stopped-Ship Preview Contract
Update the stopped-ship preview-route generation so the model-owned preview surface includes the ship's current-heading forward route whenever that route is legal in the existing zero-speed free-rotation case, instead of exposing only the alternate facings.

Acceptance Criteria:
- When a moving-side ship begins PH_MOVE at `speed == 0`, `nMoved == 0`, and `MR > 0`, `getStoppedShipPreviewRoutes()` includes the current-heading forward preview in addition to the alternate legal starting facings.
- The stopped-ship preview lookup surface continues to map preview-route hexes back to the legal starting heading set, including the restored current-heading route where applicable.
- The remediation does not change non-stopped movement highlight semantics or grant new preview options to ships with `MR == 0`.
- Existing stopped-ship preview-click continuation still resolves into the normal route-selection flow after the preview contract is corrected.
- The tactical model remains additive and non-wx.

Documentation Impact:
Repository documentation should be updated downstream to describe the corrected all-legal-facings preview behavior.

## Dependency Ordering
1. `SSW-SSMR-001` is the only implementation subtask and should land before downstream regression and documentation updates are finalized.

Parallelization Guidance:
No parallelization. The code, test expectation, and documentation corrections all depend on the corrected model-owned preview contract.

## Implementer Agent Prompts

### Implementer Prompt: SSW-SSMR-001
Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h` only if a small additive declaration or comment update is required to support the corrected preview contract

Correct the stopped-ship preview-route generation so the current-heading forward route is part of the same additive stopped-ship preview surface that already exposes the alternate starting facings. The reviewer found that the model currently skips `heading == turnData->curHeading`, which leaves one legal starting facing invisible in the PH_MOVE preview state even though the underlying movement flow still allows it. Preserve the existing non-wx model boundary, keep ordinary movement behavior unchanged, and do not expand the stopped-ship preview to ships with `MR == 0`.

Acceptance criteria:
- A stopped ship with `speed == 0`, `nMoved == 0`, and `MR > 0` exposes a preview route for its current heading as well as the alternate legal starting facings.
- The reverse preview-heading lookup remains consistent with the corrected preview-route set.
- Existing preview-click continuation and ordinary non-stopped movement behavior remain unchanged.
- The change does not add stopped-ship preview options for ships with `MR == 0`.

Validation guidance:
- Smallest likely validation scope after implementation: `cd tests && make tactical-tests && ./tactical/TacticalTests`
- If local iteration uses a narrower command, keep `tests/tactical` as the minimum required validation area and record the exact command used as an assumption.

Tester test-file location guidance:
- Expected tester-owned coverage is most likely in:
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `tests/tactical/FTacticalGameMechanicsTest.cpp`
  - `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
  - `tests/tactical/FTacticalModelSelectionHexClickSurfaceTest.cpp`

Documenter follow-up guidance:
- Expected documenter-owned updates are most likely in:
  - `doc/UsersGuide.md`
  - `doc/DesignNotes.md`

Artifact directory guidance:
- If orchestration context does not provide a path, use repository-root-relative `artifacts/stopped-ship-move-ui-review-remediation/subtask-1`

Execution instruction:
- Begin implementation immediately when the prompt contains the required blocking inputs, and do not stop after preflight restatement alone.

Completion gate:
- Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path
- `plans/stopped-ship-move-ui-review-remediation-plan.md`
