## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Added Milestone 8 Subtask 2 notes describing FBattleDisplay’s delegation through FBattleScreen/FTacticalGame and recorded the updated tactical validation/test focus.
- **`AGENTS.md`** — Updated repository guidance so Milestone 8 documentation reflects that FBattleDisplay now translates HUD events through the delegated tactical model seam while FBattleBoard remains legacy-owned for now.

### Summary
Updated milestone documentation to reflect the shipped Milestone 8 Subtask 2 behavior: FBattleDisplay no longer owns local fire-resolution logic, now delegates weapon/defense selection plus fire and mine-placement flow through FBattleScreen/FTacticalGame, and the tactical source-inspection tests now validate delegation-oriented behavior instead of removed `fireAllWeapons` source strings.

### Commit Message
`docs: document milestone8 subtask2 delegation`
