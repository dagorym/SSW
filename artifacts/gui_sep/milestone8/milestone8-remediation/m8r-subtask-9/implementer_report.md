# Implementer Report - Milestone 8 Remediation Subtask 9

## Summary
Updated `doc/DesignNotes.md` to correct Milestone 8 historical framing so it clearly distinguishes:
1. originally shipped Milestone 8 behavior,
2. later remediation for two blocking runtime bugs, and
3. the accepted non-blocking manual GUI playthrough limitation.

Also corrected Subtask 8 wording to avoid overstating that `FTacticalMineDamageFlowTest` directly exercises the full `FBattleDisplay -> FBattleScreen` wx move-done seam.

## Files Changed (content commit)
- `doc/DesignNotes.md`

## Documentation adjustments made
- Rewrote Subtask 2 fire-phase bullet to describe shipped state without claiming remediation-level cleanup lifecycle was already present.
- Replaced narrative claiming shipped move-done callback already routed through `FTacticalGame::completeMovePhase()` with explicit statement of the two blocking shipped defects:
  - fire cleanup ordering bug,
  - move-finalization bypass via `setPhase(PH_FINALIZE_MOVE)`.
- Added explicit separation between those blocking defects and the accepted non-blocking manual GUI scenario playthrough validation limitation.
- Updated Subtask 8 coverage description so:
  - `FTacticalBattleScreenDelegationTest` is identified as wx callback delegation seam coverage,
  - `FTacticalMineDamageFlowTest` is identified as direct canonical model seam outcome coverage through `FTacticalGame::completeMovePhase()`.

## Validation
- No build/test commands were run for this subtask because changes are documentation-only and no code/test files were modified.

## Commits
- Content commit: `1bc646a`
- Artifact commit: created after this report
