# Documenter Report

## Files Updated
- `doc/DesignNotes.md` — documented the stopped-ship tactical movement contract so the design notes now match the shipped zero-speed facing-selection behavior and its preserved `MR == 0` / non-stopped constraints.

## Summary
Reviewed `plans/stopped-ship-free-rotation-plan.md` against the implemented and tested SSW-ZSR-001 diff using comparison-base assumption `73ef28b` (`master` / `stopped-ship-free-rotation-plan`). Updated `doc/DesignNotes.md` to record that eligible ships entering `PH_MOVE` at `speed == 0` can choose any legal adjacent facing before displacement, can finish movement in place while preserving `speed == 0`, and still keep existing non-stopped routing and `MR == 0` steering restrictions intact.

## Commit Message
`docs: note stopped-ship tactical facing flow`
