# Seeker Missile Fixes Round 6 Plan

## Output Artifact Path

`plans/seeker-missile-fixes-round6-plan.md`

Coordinator artifact root for this plan: `artifacts/seeker-missile-fixes-round6/`
with per-subtask subdirectories `SMRVI-01` and `SMRVI-02`.

## Feature Restatement

Final round of the tactical seeker-missile HUD cleanup. Three user-requested
items, decomposed into two implementation subtasks:

1. **(Reviewer follow-up / Round-5 NOTE 1) Eliminate the duplicated attack-phase
   instruction string.** The literal
   `"Select legal path hexes to deploy seeker missiles."` is written in two
   places: it is **drawn** by `drawAttackFire()` and it is **re-measured** by
   `draw()` to compute the attack-phase pending panel's left margin
   (`pendingLMargin`, the SMRV-01 fix). If the wording is edited in only one
   place, the computed margin drifts from the actual drawn line and the
   Round-5 overlap fix silently regresses. Extract the string into a single
   shared `FBattleDisplay` constant consumed by both sites. (The Round-5
   reviewer's NOTE 1 and its optional follow-up request are the same item; this
   subtask resolves both. Round-5 NOTE 2 was non-actionable documentation
   context — no work is required for it.)

2. **(Activation panel — combined) Replace the per-row heading/allowance text
   with the seeker's board position, and shift the "Activated seekers" column
   right so it clears the Done-button column.** During the seeker-activation
   phase (`PH_SEEKER_ACTIVATION`), `drawSeekerActivation()` renders each
   deactivatable row as `Deactivate seeker #N (heading H, allowance A)`. The
   heading/allowance values are clutter that provide no useful information to the
   player; replace the parenthetical with the seeker's board position formatted
   the same way the pre-game placement recall list shows it — `(X,Y)` from the
   seeker's hex — so a row reads `Deactivate seeker #N (X,Y)`. Separately, the
   "Activated seekers" column is still drawn at a **fixed** `lMargin = 310`, which
   continues to overlap the left column (the activation instruction lines and the
   "Seeker Activation Done" button). Shift the column right using a **dynamically
   computed** left margin — the same approach used for the attack-phase pending
   panel in Round-5 SMRV-01 — so it always clears the widest left-column content.
   These two changes are combined because both edit `drawSeekerActivation()` and
   both re-derive the same row click-regions (`m_seekerActivationRegions`).

## Confirmed Repository Facts

(All file/line references are from the current `sf2` working tree, HEAD
`fc53550`; line numbers are approximate and may shift as edits land. Geometry
constants: `BORDER = 5`, `ZOOM_SIZE = 30`,
`leftOffset = 2*BORDER + ZOOM_SIZE = 40`; `getActionPromptLineY(0) = 5`,
`(1) = 21`, `(2) = 37`.)

1. **The attack-phase instruction string is duplicated.**
   - Drawn in `drawAttackFire()` at `src/tactical/FBattleDisplay.cpp:1248`
     (`os.str("Select legal path hexes to deploy seeker missiles.");`).
   - Re-measured in `draw()` at `src/tactical/FBattleDisplay.cpp:731–734`:
     a local `wxString widestAttackLine("Select legal path hexes to deploy seeker missiles.")`
     whose `GetTextExtent(...).GetWidth()` feeds
     `pendingLMargin = leftOffset + attackTextW + 2*BORDER`, which is passed to
     `drawOffensiveSeekerPendingRows(dc, pendingLMargin, getActionPromptLineY(0), 10)`.
   - These two literals must stay identical for the SMRV-01 margin to match the
     drawn text; today nothing enforces that.

2. **`drawSeekerActivation()` (`src/tactical/FBattleDisplay.cpp:1688–1745`):**
   - Uses a fixed `const int lMargin = 310;` at `:1692`.
   - Left-column instruction lines are drawn at `leftOffset (=40)` at `:1707–1709`:
     `"Seeker activation phase."`,
     `"Click a seeker stack on the board to activate one seeker."`,
     `"Click a row below to deactivate an activated seeker."`
     (the middle line is the widest).
   - The list header `"Activated seekers:"` is drawn at `(lMargin, getActionPromptLineY(0))`
     at `:1713` (vertical anchor already corrected by Round-5 SMRV-02).
   - Each row text is built at `:1726–1728` as
     `"Deactivate seeker #" << seeker.seekerID << " (heading " << seeker.heading
     << ", allowance " << seeker.movementAllowance << ")"`.
   - The row's click region is computed from the **rendered text extent** at the
     **drawn margin** at `:1731–1733`
     (`m_seekerActivationRegions.push_back(wxRect(lMargin, y, tSize.GetWidth()+16, tSize.GetHeight()))`
     and `m_seekerActivationSeekerIDs.push_back(seeker.seekerID)`), so the click
     region automatically tracks whatever text and margin are used.

3. **The "Seeker Activation Done" button** is `m_buttonSeekerActivationDone`
   (`include/tactical/FBattleDisplay.h:124`), a sizer-managed `wxButton`
   (`src/tactical/FBattleDisplay.cpp:113`, added to `actionSizer` at `:129`); it is
   shown/positioned in the action-button row of the lower panel during activation
   (`:1745–1752`). `getActionButtonRowBottom()` (`:515`) reports the bottom of that
   row. The button occupies the left column beneath the instruction text.

4. **The pre-game placement recall list formats a seeker position as `(X,Y)`.**
   `drawPlaceSeekers()` at `src/tactical/FBattleDisplay.cpp:1640–1643` builds
   `"Recall: " << shipName << " (" << grp.hex.getX() << "," << grp.hex.getY() << ") x" << grp.count`.
   This is the `(X,Y)` position format to mirror.

5. **`FTacticalSeekerMissileState`** carries `seekerID`, `ownerID`, `hex`,
   `heading`, and `movementAllowance` (`include/tactical/FTacticalGame.h`). The
   activation list is sourced from
   `m_parent->getActiveSeekersByMovingPlayerThisPhase()` at
   `src/tactical/FBattleDisplay.cpp:1719`, and each element's `hex` is the seeker's
   board position.

6. **Round-5 SMRV-01 precedent for a dynamically computed margin** lives in
   `draw()` (`:727–734`): measure the widest left-column line via
   `GetTextExtent(...)` and set the panel margin to
   `leftOffset + width + 2*BORDER`. SMRVI-02 applies the same idea to the
   activation panel, additionally accounting for the Done button.

## Resolved Design Decisions

- **D1 (decomposition granularity):** Items "remove heading/allowance" and "shift
  the activation panel right" are **combined into one subtask** (SMRVI-02) because
  both edit `drawSeekerActivation()` and both re-derive
  `m_seekerActivationRegions`. The reviewer-follow-up shared-constant work remains
  its own subtask (SMRVI-01). **Decided by the user.**
- **D2 (replace, not just remove):** The per-row parenthetical is **replaced**
  with the seeker's board position `(X,Y)` (from `seeker.hex`), formatted like the
  pre-game placement recall list, rather than removed entirely. **Decided by the
  user.**
- **D3 (activation margin approach):** The activation column uses a
  **dynamically computed** left margin (mirroring SMRV-01), derived from the
  widest left-column content so it always clears the instruction text and the
  "Seeker Activation Done" button — not a fixed larger value. **Decided by the
  user.**
- **D4 (anti-drift consistency):** Because SMRVI-02 introduces its own
  measure-the-widest-left-line computation, the margin computation MUST measure
  the exact instruction string(s) that are actually drawn (ideally the same
  string object/constant), so it cannot reintroduce the drift problem SMRVI-01
  fixes. **Planner decision (consistency with D3 and SMRVI-01).**

## Assumptions

- The two FBattleDisplay subtasks are **not** parallelizable: both edit
  `src/tactical/FBattleDisplay.cpp` and `include/tactical/FBattleDisplay.h`.
  Executed serially.
- No model/state (`FTacticalGame`) change is required; this is purely
  GUI-render/text/layout work in the wx display layer.
- The shared-constant scope for SMRVI-01 is the one attack-phase instruction
  string only; generalizing the activation instruction strings into members is
  out of scope (SMRVI-02 only needs to measure the line it draws).
- GUI behavioral tests run under `xvfb` (`tests/gui` suite); the tactical suite
  runs headless.

## Likely Files To Modify

- `src/tactical/FBattleDisplay.cpp` (both subtasks)
- `include/tactical/FBattleDisplay.h` (both subtasks — shared constant
  declaration and/or docblock updates)

(Tester-owned test files are listed in each subtask's Tester guidance and are not
implementer-owned.)

## Subtasks

### SMRVI-01 — Extract The Attack-Phase Instruction String Into A Shared Constant

Security review: not required.

Replace the two duplicated literals of
`"Select legal path hexes to deploy seeker missiles."` (drawn in
`drawAttackFire()` at `FBattleDisplay.cpp:1248`; re-measured in `draw()` at
`FBattleDisplay.cpp:731–734`) with a single shared `FBattleDisplay` constant
referenced by both the draw site and the margin-measurement site. Pure
robustness refactor: no visual or behavioral change. The attack-phase pending
panel must continue to clear the left instruction text exactly as it does today.

### SMRVI-02 — Activation Panel: Show Seeker Position And Shift The Column Right

Security review: not required.

Two combined changes in `drawSeekerActivation()`
(`src/tactical/FBattleDisplay.cpp:1688–1745`):

1. **Row text:** Replace the `(heading H, allowance A)` parenthetical (`:1726–1728`)
   with the seeker's board position `(X,Y)` from `seeker.hex`, formatted the same
   way `drawPlaceSeekers()` shows placed-seeker positions (`:1642`). Each row
   becomes `Deactivate seeker #N (X,Y)`.
2. **Column margin:** Replace the fixed `lMargin = 310` (`:1692`) with a
   dynamically computed left margin (mirroring the SMRV-01 attack-phase
   approach) so the "Activated seekers" column clears the widest left-column
   content during `PH_SEEKER_ACTIVATION` — the longest activation instruction
   line ("Click a seeker stack on the board to activate one seeker.") **and** the
   right edge of the "Seeker Activation Done" button — and therefore no longer
   overlaps the Done-button column. The margin computation must measure the exact
   instruction string actually drawn (per D4) so it cannot drift.

Recompute each row's click region (`m_seekerActivationRegions` /
`m_seekerActivationSeekerIDs`) from the new row text and the new margin so that
clicking a row still deactivates exactly the intended seeker, and ensure rows are
not clipped and do not overlap the left column, the "Seeker Activation Done"
button, or the ship-status display.

## Dependency Ordering

- **Serial, in this order:** SMRVI-01 → SMRVI-02.
- **No subtask is parallelizable** (both edit the same two FBattleDisplay files;
  SMRVI-01 may touch `draw()`/`drawAttackFire()`/the header while SMRVI-02 edits
  `drawSeekerActivation()`/the header — overlapping files require serial
  execution).
- SMRVI-01 first because it is the smallest, isolated robustness refactor and the
  outstanding reviewer follow-up; SMRVI-02 then builds on a clean base. There is
  no functional dependency between them, only a shared-file (serialization)
  dependency.
- No subtask requires specialist Security review.

## Overall Acceptance Criteria

1. The attack-phase instruction string exists as a single shared `FBattleDisplay`
   constant; both the `drawAttackFire()` draw site and the `draw()`
   pending-margin measurement reference that one constant (no remaining duplicate
   literal). (SMRVI-01)
2. The attack-phase pending-seeker panel still renders to the right of the left
   instruction text with no overlap and the recall click-regions still work —
   i.e. SMRVI-01 is behavior-preserving. (SMRVI-01)
3. During `PH_SEEKER_ACTIVATION`, each deactivatable row shows the seeker's board
   position as `(X,Y)` (matching the placement recall format) and no longer shows
   `heading`/`allowance`. (SMRVI-02)
4. During `PH_SEEKER_ACTIVATION`, the "Activated seekers" column's left edge is to
   the right of the widest left-column content (the longest activation
   instruction line and the "Seeker Activation Done" button's right edge), so the
   panel no longer overlaps the Done-button column; the margin is dynamically
   computed (not the old fixed 310). (SMRVI-02)
5. Clicking an activation row still deactivates exactly the intended seeker after
   the text and margin changes; rows are not clipped or drawn behind the Done
   button or ship-status display. (SMRVI-02)
6. The protected document `doc/rules/tactical_operations_manual.md` is not
   edited. (Both)
7. The tactical test suite passes; new/updated GUI behavioral tests pass under
   xvfb; no new regressions are introduced relative to the documented
   pre-existing GUI failures. (Both)

## Documentation Impact

- `include/tactical/FBattleDisplay.h`: docblocks for the new shared constant
  (SMRVI-01) and for `drawSeekerActivation()` (SMRVI-02 — updated row content and
  computed-margin behavior); `@date Last Modified` / `@author` updated per
  AGENTS.md.
- `doc/test-contracts.md`: add/extend entries for the new SMRVI behavioral tests
  (attack-string single-source; activation-row position text; activation-column
  computed margin) and any source-contract supplements.
- `AGENTS.md`: if the HUD-layout convention bullet enumerates which lower-panel
  phases use a computed (vs fixed) right-shift margin, note that the
  activation-phase "Activated seekers" column now uses a dynamically computed
  margin like the attack-phase pending panel.
- No user-facing guide change expected (internal tactical HUD text/layout only).

## Implementer Prompts

### SMRVI-01 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Eliminate the duplicated attack-phase instruction string literal
`"Select legal path hexes to deploy seeker missiles."` by introducing a single
shared `FBattleDisplay` constant and referencing it from both sites. Today the
literal is (a) drawn by `drawAttackFire()` at
`src/tactical/FBattleDisplay.cpp:1248` and (b) re-measured in `draw()` at
`src/tactical/FBattleDisplay.cpp:731–734`, where
`wxString widestAttackLine("Select legal path hexes to deploy seeker missiles.")`
feeds `pendingLMargin = leftOffset + GetTextExtent(widestAttackLine).GetWidth() + 2*BORDER`
(the Round-5 SMRV-01 overlap fix). Define one constant (e.g. a
`static const`/`constexpr` member or static class string on `FBattleDisplay`,
following existing conventions in the header) holding that exact text, and use it
in BOTH the draw call and the margin measurement so the two can never diverge.
Do not change the visible wording, the draw position of the instruction text, or
the pending-panel layout — this is a behavior-preserving refactor. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- The attack-phase instruction string is defined exactly once as a shared
  `FBattleDisplay` constant; both `drawAttackFire()` (the drawn text) and `draw()`
  (the `pendingLMargin` measurement) reference that single constant, with no
  remaining duplicate string literal.
- The attack-phase ("Pending seeker deployments") panel still renders to the right
  of the left instruction text with no overlap, and the pending recall
  click-regions still behave as before (no visual or behavioral change).
- The visible instruction wording and its draw location are unchanged.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; run
  `cd tests/gui && xvfb-run -a ./GuiTests` (a display/xvfb is required).

Tester test-file location guidance:
- The behavior-preserving guarantee is covered by the existing live test
  `testOffensiveSeekerPendingListRegionVisibilityAndRecall` in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` + `WXGuiTestHarness`):
  confirm it still passes and, if helpful, assert that the pending panel's left
  edge equals `leftOffset + GetTextExtent(<the shared constant>).GetWidth() + 2*BORDER`
  — i.e. the margin tracks the constant. Add a supplementary source-contract
  assertion in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*` that the
  instruction literal appears exactly once (defined via the shared constant and
  referenced by both the draw and measurement sites), but per AGENTS.md the
  behavioral test is the primary coverage and the source-contract test only
  supplements it.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round6/SMRVI-01/` using the active implementer
role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are
committed.

### SMRVI-02 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Rework the seeker-activation phase "Activated seekers" panel in
`drawSeekerActivation()` (`src/tactical/FBattleDisplay.cpp:1688–1745`) with two
combined changes:

1. Per-row text — replace the `(heading H, allowance A)` parenthetical built at
   `src/tactical/FBattleDisplay.cpp:1726–1728` with the seeker's board position
   `(X,Y)` taken from the row's `seeker.hex` (i.e. `seeker.hex.getX()`,
   `seeker.hex.getY()`), formatted the same way the pre-game placement recall list
   shows a seeker position in `drawPlaceSeekers()` at
   `src/tactical/FBattleDisplay.cpp:1642` (`(" << hex.getX() << "," << hex.getY() << ")"`).
   Each row should read `Deactivate seeker #N (X,Y)`. Drop the heading and
   allowance values entirely.

2. Column margin — replace the fixed `const int lMargin = 310;` at
   `src/tactical/FBattleDisplay.cpp:1692` with a dynamically computed left margin,
   mirroring the Round-5 SMRV-01 attack-phase approach in `draw()`
   (`:731–734`). Compute the margin from the widest left-column content drawn
   during `PH_SEEKER_ACTIVATION` so the "Activated seekers" column clears it: the
   longest activation instruction line (the lines drawn at `leftOffset` at
   `:1707–1709`, widest being
   "Click a seeker stack on the board to activate one seeker.") AND the right edge
   of the "Seeker Activation Done" button (`m_buttonSeekerActivationDone`). A
   reasonable form is
   `lMargin = leftOffset + max(widestInstructionTextWidth, doneButtonRightExtent) + 2*BORDER`
   (use the button's actual right edge/width via its position+size, or its best
   size, whichever is reliable at draw time; if the button geometry is not yet
   valid at draw time, clear the widest instruction line and the button's left
   origin plus its best width). The margin computation MUST measure the exact
   instruction string(s) actually drawn at `:1707–1709` (do not hardcode a
   separate copy that can drift). The header "Activated seekers:" (`:1713`) and all
   rows use this computed margin.

After both changes, recompute each row's click region
(`m_seekerActivationRegions` and the parallel `m_seekerActivationSeekerIDs`) from
the new row text extent and the new margin (the existing code at `:1731–1733`
already derives the region from the drawn text and margin — keep that property so
it tracks automatically). Ensure rows are not clipped and do not overlap the left
column, the "Seeker Activation Done" button, or the ship-status display. Do not
change the underlying activation/deactivation model logic. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `PH_SEEKER_ACTIVATION`, each deactivatable row displays the seeker's
  board position as `(X,Y)` (matching the `drawPlaceSeekers` recall format) and no
  longer shows `heading` or `allowance`.
- During `PH_SEEKER_ACTIVATION`, the "Activated seekers" column's left edge is to
  the right of the widest left-column content (longest activation instruction line
  and the "Seeker Activation Done" button's right edge), computed dynamically (not
  the old fixed 310), so the panel no longer overlaps the Done-button column; the
  vertical anchor remains the top of the bottom panel (`getActionPromptLineY(0)`,
  from SMRV-02).
- Clicking an activation row still deactivates exactly the intended seeker; rows
  are not clipped and do not overlap the left column, the Done button, or the
  ship-status display.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; run
  `cd tests/gui && xvfb-run -a ./GuiTests` (a display/xvfb is required for the
  live render/anchor/click tests).

Tester test-file location guidance:
- Live render / row-text / margin / click-region behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` + `WXGuiTestHarness`),
  consistent with the existing `testSeekerActivationAnchorIsAtActionPromptLineY`
  pattern. Required behavioral coverage (each must discriminate the change — fail
  against the pre-change code and pass after):
  (a) seed an activated seeker at a known hex, render `PH_SEEKER_ACTIVATION`, and
  assert the row text contains the position `(X,Y)` and does NOT contain
  "heading"/"allowance";
  (b) assert the activation column's region left edge is to the right of the
  widest left-column content (instruction line extent and the Done button's right
  edge) and strictly greater than the old fixed 310;
  (c) simulate a click on a row's region and assert the intended seeker is
  deactivated (active-seeker set decrements).
  Source-contract coverage for the helper may be added in
  `tests/tactical/FTacticalBattleDisplayFireFlowTest.*` as a supplement only; per
  AGENTS.md the behavioral tests are mandatory and source-contract tests may not
  substitute for them.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round6/SMRVI-02/` using the active implementer
role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are
committed.
