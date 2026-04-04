# Milestone 7 Context Notes

## Key Repository Facts

- `FBattleScreen` still owns tactical runtime state directly.
- `FTacticalGame` already owns equivalent additive tactical state and mechanics helpers.
- `WXTacticalUI` exists but is not yet wired into the live tactical runtime.
- `FBattleBoard` and `FBattleDisplay` still depend on the `FBattleScreen` public API, so Milestone 7 should preserve that shell surface.

## Planning Emphasis

- Minimize risk by moving canonical state ownership first and preserving the shell API.
- Keep modal dialog lifetime and close-path behavior on `FBattleScreen`.
- Leave rendering/input extraction to Milestone 8.

## Expected Follow-on Risk Areas

- API drift between `FBattleScreen` and `FTacticalGame` during the handoff.
- Existing source-inspection tests that pin pre-delegation behavior.
- Ownership boundaries for `FTacticalGame`, `WXTacticalUI`, and the existing tactical child widgets.
