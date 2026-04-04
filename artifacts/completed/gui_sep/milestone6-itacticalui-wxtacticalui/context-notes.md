# Milestone 6 Context Notes

## Confirmed Current State

- `FTacticalGame.h` already contains the Milestone 5 placeholder boundary: `class ITacticalUI;` and `ITacticalUI * m_ui;`.
- No concrete tactical UI boundary files exist yet under `include/tactical` or `include/gui`.
- `FBattleScreen.cpp` still owns the tactical modal UI calls:
  - `showTacticalDamageSummaryDialog()` creates `TacticalDamageSummaryGUI`
  - `fireICM()` creates `ICMSelectionGUI`
  - `declareWinner()` shows the winner message directly with `wxMessageBox`
- `src/gui/Makefile` already compiles `WXStrategicUI.o`, which is the closest in-repo adapter pattern to mirror for the tactical side.
- Tactical additive tests already exist under `tests/tactical`, including interface-boundary and source-shape checks for Milestone 5.

## Key Planning Constraints

- Milestone 6 is additive only: no rewiring of `FBattleScreen`, `FBattleBoard`, `FBattleDisplay`, or the BattleSim tactical entry flow.
- The tactical interface boundary must stay wx-free even though the concrete adapter is wx-backed.
- `VehicleList` and `ICMData` come from `ships/FVehicle.h`, so the tactical interface cannot rely only on forward declarations for those parameter types.
- Verification should focus on interface shape, adapter null-safety, GUI build wiring, and no-regression additive behavior.
