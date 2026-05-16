## Documenter Report

### Files Updated
- **`doc/UsersGuide.md`** — Updated the tactical combat screen description to match the shipped `FBattleScreen` layout policy: default `1200x900` sizing, a `120px` baseline lower panel when space allows, growth for larger tactical-control requests, and the `60%` minimum battlefield-height floor.

### Summary
Documentation review found a user-facing documentation gap in `doc/UsersGuide.md`: it still described the tactical battle screen as opening at `800x600` with a constant-height lower panel. I updated that section to reflect the tested `FBattleScreen` behavior and confirmed no tactical rules or battle-state documentation changes were needed. No separate developer-guidance update was required because this task only changed visible window sizing/layout behavior, not repository-wide contributor workflow guidance.

### Commit Message
`docs: update tactical battle screen layout guide`
