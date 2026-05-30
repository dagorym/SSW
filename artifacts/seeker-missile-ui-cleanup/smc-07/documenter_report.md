# Documenter Report: SMC-07 Seeker Path Drawing and Icon Rotation

**Task:** SMC-07 - Draw seeker movement on the board (cyan stepped path, PH_MOVE scope, pen width 2) plus heading-rotated seeker icon. Covers SMC-06 prerequisite: movementPath field capture in FTacticalSeekerMissileState and getSeekerMissiles() delegation through FBattleScreen.

**Branch:** seeker-fix

**Documentation Commit:** b3dc88f

**Artifact Directory:** artifacts/seeker-missile-ui-cleanup/smc-07

---

## Documentation Scope

SMC-06 adds render-supporting `movementPath` state to `FTacticalSeekerMissileState` and populates it in `resolveActiveSeekersForMovingPlayer()`. SMC-07 uses that path data in `FBattleBoard::drawSeekerPaths()` (cyan, PH_MOVE only) and adds heading rotation to `drawSeekerMissiles()`.

---

## Documentation Changes Made

### include/tactical/FTacticalGame.h

**resolveActiveSeekersForMovingPlayer() Doxygen updated.**

The existing Doxygen accurately described seeker movement resolution mechanics but did not mention the SMC-06 movementPath population behavior. Added a paragraph documenting that:
- Moving seekers have their `movementPath` cleared and rebuilt from the start hex through each greedy step.
- Non-moving seekers (inactive or belonging to the non-moving player) have their `movementPath` cleared to prevent stale paths persisting across turns.
- The render-supporting path state is consumed by `FBattleBoard::drawSeekerPaths()` during `PH_MOVE`.

Author field updated to include `claude-sonnet-4-6 (standard)` and Last Modified updated to May 30, 2026. File-level `@author` field updated similarly.

**Documentation already complete (no changes needed):**
- `FTacticalSeekerMissileState` struct Doxygen already documented `movementPath` as render-supporting, non-persistent, wx-free state with notes about clearance semantics and the `resolveActiveSeekersForMovingPlayer` source.
- `getSeekerMissiles()` one-liner comment on `FBattleScreen.h` already noted that the return includes `movementPath` render state.

### include/tactical/FBattleBoard.h

**Documentation already complete (no changes needed):**
- `drawSeekerPaths()` Doxygen already described cyan pen, PH_MOVE scope, pen width 2, `movementPath.size() >= 2` filter, and stepped `dc.DrawLine()` draw call.
- `drawSeekerMissiles()` Doxygen already described heading rotation for active seekers in normal battle phases.

### doc/DesignNotes.md

**Added SMC-06 and SMC-07 entries** after the TSM-006 paragraph describing offensive-fire seeker deployment.

SMC-06 entry covers:
- The `movementPath` field added to `FTacticalSeekerMissileState` as render-supporting state.
- Clearance and rebuild semantics in `resolveActiveSeekersForMovingPlayer()`.
- Non-persistent, wx-free nature of the field.
- `FBattleScreen::getSeekerMissiles()` delegation providing the path data to the board layer.

SMC-07 entry covers:
- `drawSeekerPaths()` mechanics: iterates active seekers with `movementPath.size() >= 2`, cyan pen (`#00CCCC`, width 2), stepped `dc.DrawLine()` segments through each hex center.
- Scope limited to `PH_MOVE` so paths refresh each movement turn and do not persist.
- Heading rotation in `drawSeekerMissiles()`: `heading * pi/3` radians, consistent with six-direction ship icon rotation.

### AGENTS.md

**Extended the Tactical tests bullet** to mention the new SMC-07 test contracts:
- `testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen` source contract checks.
- Updated `testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules` now verifies the heading-rotated call form.
- New `testMovementPathPopulatedByResolveActiveSeekers` runtime model contract.

---

## Documentation Gaps Found and Resolved

| Gap | Location | Resolution |
|---|---|---|
| `resolveActiveSeekersForMovingPlayer()` Doxygen missing movementPath population notes | `include/tactical/FTacticalGame.h` | Added paragraph describing path clearance and rebuild for moving seekers, clearance-only for non-moving seekers, and `drawSeekerPaths()` consumer. |
| DesignNotes.md missing SMC-06/07 entries | `doc/DesignNotes.md` | Added two paragraphs after TSM-006 entry. |
| AGENTS.md tactical test bullet missing SMC-07 test contracts | `AGENTS.md` | Extended existing bullet with new test names and contracts. |

---

## Documentation Already Complete

- `FTacticalSeekerMissileState.movementPath` field had accurate struct-level Doxygen.
- `drawSeekerPaths()` header declaration had complete and accurate Doxygen.
- `drawSeekerMissiles()` header declaration had complete Doxygen including heading rotation.
- `getSeekerMissiles()` one-liner was accurate.

---

## Test Outcomes (from Tester)

Tactical tests: 189/189 pass (0 failures, 0 errors). 2 new tests added. GUI pre-existing failures (8) are unrelated to SMC-07.

---

## Status: PASS

All documentation tasks complete. Documentation commit: b3dc88f. Artifacts written.
