# Implementer Report — tactical-ship-stats-lower-panel-layout-fixes / subtask-1

## Summary
Implemented a scoped lower-panel layout remediation in tactical display code. The update keeps split mode only when measured ship-stats content plus a safety margin can fit, falls back to stacked mode when split becomes too tight, and uses runtime action-button row geometry to keep stacked ship stats below the button row during resize/phase transitions.

## Modified Files
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

## Behavior Changes
- Added an explicit helper to compute current action-button row bottom (`getActionButtonRowBottom()`), with fallback and visible-button geometry handling.
- Tightened split eligibility in `ensureLowerPanelLayoutState(...)` to require content-fit plus a small layout padding reserve.
- Preserved split layout where fit is valid; otherwise force stacked layout.
- In `drawCurrentShipStats(...)`, added runtime fallback to stacked mode when split gets too tight at paint time and enforce stacked top placement below the live button row.
- Increased prompt-reservation floor for split layout (`ACTION_PROMPT_MIN_WIDTH` now 280) to avoid narrow-width clipping/regression.

## Validation
Executed and passed:
- `cd tests/tactical && make && ./TacticalTests` → `OK (153 tests)`
- `cd tests/gui && make && GDK_BACKEND=x11 xvfb-run -a ./GuiTests` → `OK (36 tests)`

## Notes
- No changes were made outside allowed implementation files.
- The verifier preamble warning about stale `doc/DesignNotes.md` totals was intentionally left for downstream remediation in the documented chain.
- Implementation/code commit hash: `535f305`.
