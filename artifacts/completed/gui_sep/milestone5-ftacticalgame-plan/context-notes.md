# Milestone 5 Context Notes

## Design Intent

Milestone 5 is explicitly additive. The new `FTacticalGame` class should compile and own tactical mechanics state, but the running tactical board remains based on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` until Milestones 7 and 8.

## Tactical State Owners Observed In Repository

- `FBattleScreen`
  - battle state and phase
  - active/moving player flags
  - selected ship and weapon
  - attacker/defender fleet and ship lists
  - movement-complete flag
  - tactical combat report state
  - ICM assignment state
  - setup, phase transitions, electrical fire, and winner flow
- `FBattleBoard`
  - `hexData` grid state
  - `turnData` per-ship movement state
  - movement path calculation
  - gravity-turn and mine interaction state
  - movement finalization and ship removal on the map
- `FBattleDisplay`
  - combat-phase execution in `fireAllWeapons()`
  - damage-summary dialog trigger after report construction

## Planning Constraints

- No wx inheritance or wx headers in `FTacticalGame`.
- `ITacticalUI` is not introduced until Milestone 6, so only a forward-declared pointer placeholder belongs in Milestone 5.
- Current tactical runtime behavior must not change in this milestone.

## Recommended Verification Floor

- Tactical static library builds with `FTacticalGame.o`.
- Optional additive smoke test only if it does not force premature UI or delegation design.
