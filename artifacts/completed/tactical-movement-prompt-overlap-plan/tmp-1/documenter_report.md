## Documenter Report

### Scope
- Document the verified tactical lower-panel resize/reflow remediation for `FBattleScreen` and `FBattleDisplay`.
- Capture the shipped shrink-back behavior: after constrained-width growth, the lower panel can return to its earlier compact height once width pressure is removed.
- Keep the documentation aligned with the verified scope that this change stays within tactical layout behavior and does not alter tactical combat rules.

### Assumptions
- Used `70ddd886a047b110690656c52f3a3a00cc9eb2e1`, the immediate parent of `5c332730b252a1a07fb0309f691b1935ec1b2c8d`, as the comparison base because the task explicitly asked for the remediation delta against that parent.
- Reused the provided shared artifact directory `artifacts/tactical-movement-prompt-overlap-plan/tmp-1`.
- Reused the tester-validated command results because this run made documentation/artifact changes only.

### Files Updated
- **`doc/UsersGuide.md`** — Added the user-facing note that live tactical resize reflows immediately and that the lower panel can shrink back toward its compact baseline after width pressure is removed.
- **`doc/DesignNotes.md`** — Documented the implementation detail that resize reflow now recomputes lower-panel requested height from current geometry instead of retaining the largest prior height, and recorded the tester evidence that proves shrink-back on restore.

### Summary
I reviewed the current remediation delta from `70ddd886a047b110690656c52f3a3a00cc9eb2e1` to `5c332730b252a1a07fb0309f691b1935ec1b2c8d`, the updated tactical/gui regression tests, the existing plan hints, and the current tactical documentation. The user guide already covered immediate lower-panel reflow, but it did not state the newly verified shrink-back behavior after constrained-width expansion. The design notes also needed to capture the max-height-retention removal in `FBattleDisplay` and `FBattleScreen`, plus the live GUI proof that restoring width restores the earlier lower-panel height. No tactical rules document or contributor-guidance file needed changes because the remediation stayed localized to layout policy and did not change combat or fire-resolution behavior.

### Documentation Commit
- **Commit:** `edc27151ccaeb8e61537f93867bc0174b4927a73`
- **Message:** `docs: note tactical lower-panel shrink-back`

### Validation Context
- Reused the provided tester validation outcomes for this documentation-only pass:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (152 tests)`
  - `cd tests/gui && make && xvfb-run -a ./GuiTests` → `OK (35 tests)`
- No documentation-specific build or lint step exists in this repository.
