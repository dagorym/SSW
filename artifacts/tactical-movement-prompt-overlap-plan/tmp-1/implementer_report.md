# Implementer Report

## Task
Introduce persistent `FBattleDisplay` lower-panel layout state for prompt/stats split, keep ship-stats split stable across tactical phases unless geometry invalidates it, and communicate requested display height from current layout state.

## Files Changed
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

## Implementation Summary
- Added explicit shared lower-panel layout state (`LowerPanelLayoutState`) tracking:
  - current prompt/stats split mode,
  - ship-stats anchor positions,
  - reserved prompt lines,
  - requested display height.
- Added prompt wrapping/reservation helpers:
  - `reserveActionPromptLines(...)`
  - `countWrappedActionPromptLines(...)`
  - `drawWrappedActionPrompt(...)`
- Updated movement prompt flow to reserve wrapped prompt lines and retain required reminder text.
- Updated defensive fire, offensive fire, and mine placement paths to reserve the action prompt block consistently.
- Updated `drawCurrentShipStats(...)` to apply and reuse shared layout state and request display height from that state.
- Layout transitions now occur only when current geometry can no longer sustain existing state.

## Validation
Baseline before edits:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` (failed baseline: 3 known source-token mismatches)
- `cd tests/gui && make && xvfb-run -a ./GuiTests` (failed baseline: 1 known source-token mismatch)

Post-change:
- `cd tests && make tactical-tests && ./tactical/TacticalTests` ✅ pass (`OK (147 tests)`)
- `cd tests/gui && make && xvfb-run -a ./GuiTests` ✅ pass (`OK (34 tests)`)

## Commit
- Implementation/code commit: `55ed2b6`
