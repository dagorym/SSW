## Implementer Report — TMPR-2

### Summary
Implemented TMPR-2 in tactical lower-panel layout sizing so move-phase prompt wrapping drives additional lower-panel height only when needed, and releases that extra height when width pressure is removed.

### Files changed
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

### What changed
1. **Prompt reservation now tracks full wrapped line demand**
   - Removed the previous 3-line cap in `reserveActionPromptLines(...)`.
   - Added helpers to compute prompt extra lines and dynamic spacer height.

2. **Button clearance now follows wrapped prompt depth**
   - Added a dynamic spacer item above action buttons (`m_actionButtonExtraSpacerItem`) and refresh logic.
   - Extra spacer scales by `(reservedPromptLines - ACTION_PROMPT_MAX_LINES) * ACTION_PROMPT_LINE_HEIGHT`.

3. **Requested display height now includes prompt-wrap clearance**
   - Kept baseline V2 requested-height seed.
   - Added prompt-wrap extra height and button-row bottom checks into `ensureLowerPanelLayoutState(...)`.
   - Preserved minimum floor behavior and existing stats-bottom sizing checks.

4. **Shrink-back behavior on width expansion**
   - Ensured right-split mode reclaims available width by resetting `shipStatsLeftMargin` to current `largestMarginWithStatsRoom` when split remains valid.
   - This allows wrapped prompt lines (and extra height/spacer) to contract when width grows.

5. **Deterministic wrap counting during resize reflow**
   - `refreshMovePromptReservation(...)` now sets the same move-prompt font before measuring wrapped lines, matching draw-time geometry.

### Acceptance criteria mapping
- Requested lower-panel height now reflects active wrapped prompt geometry (via uncapped reservation + dynamic extra height computation).
- Narrowing width can increase lower-panel requested height and button spacer to prevent prompt/button overlap.
- Wrapped prompt bottom clears the button band through explicit dynamic spacer and requested-height checks.
- Widening width contracts the extra height by recomputing wrapped lines with expanded split prompt width.
- `FBattleScreen` 60% map floor policy and fallback behavior were not changed.

### Validation
- `cd tests && make tactical-tests && ./tactical/TacticalTests` ✅
- `cd tests/gui && make && xvfb-run -a ./GuiTests` ✅

### Notes for Tester
Focus on resize transition behavior in move phase:
1. Start wide (baseline height).
2. Narrow width until prompts wrap deeper (expect increased lower-panel request and no prompt/button overlap).
3. Re-widen (expect reduction toward baseline request and side-by-side split when valid).
