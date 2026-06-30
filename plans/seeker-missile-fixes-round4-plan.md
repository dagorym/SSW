# Seeker Missile Fixes Round 4 Plan

## Output Artifact Path

`plans/seeker-missile-fixes-round4-plan.md`

Coordinator artifact root for this plan: `artifacts/seeker-missile-fixes-round4/`
with per-subtask subdirectories `SMRIV-01` … `SMRIV-06`.

## Feature Restatement

Continuation of the tactical seeker-missile / pre-game ordnance work. This round
addresses lower-panel (HUD) layout problems, a panel-height shrink problem, a
seeker-removal-after-impact defect, and three non-blocking cleanup follow-ups
carried over from the previous plan's Reviewer report. Concretely:

1. **Mine-phase HUD layout.** During pre-game mine placement (`BS_PlaceMines`),
   the ship-source-selection rows currently render starting *below* the action
   button row (`getActionButtonRowBottom()`). They should instead be anchored to
   the **top of the bottom panel** and placed to the **right of the left column**
   (the instruction text and the "Done" button). The mine instruction text
   ("The defending player may now place mines before the attacker sets up their
   ships.") should wrap onto two lines.
2. **Panel shrink-back.** After the bottom panel auto-grows to fit overflowing
   content, it currently never shrinks again (height is ratcheted up
   monotonically). It should shrink back down when the extra space is no longer
   needed — specifically, it keeps the expanded height within a tactical phase
   and recomputes-to-fit when the phase changes.
3. **Seeker-phase HUD layout.** During pre-game seeker placement
   (`BS_PlaceSeekers`), both the ship-source-selection rows and the placed-seeker
   undeploy/recall list currently render starting below the button row (selection
   rows at the left margin, recall list horizontally centered). They should be
   anchored to the **top of the bottom panel** and arranged as **side-by-side
   columns** to the right of the left column: selection rows in a middle column,
   the placed-seeker recall list in a right column. The seeker instruction text
   should wrap onto two lines.
4. **Seeker removal after impact (defect).** When a seeker missile contacts a
   ship and its attack is resolved and any damage applied, the seeker must be
   removed from the board. Currently a seeker that impacts during the
   **seeker-activation phase** is *not* removed and remains active, attacking
   again on subsequent phases/turns.
5. **Attack-phase recall layout.** During the attack/offensive-fire phase
   (`PH_ATTACK_FIRE`), the offensive-seeker "unplace"/recall list currently
   renders starting below the button row. It should be placed to the **right of
   the "Done" button** and anchored to the **top of the bottom panel**, matching
   the pre-game treatment.

Plus three carried-over Reviewer follow-ups (cleanup/hardening only): remove a
now-unreachable branch, add one defensive guard, and correct a test
include-guard typo.

## Confirmed Repository Facts

(All file/line references are from the current `sf2` working tree; line numbers
are approximate and may shift as edits land.)

1. **Pre-game source rows are anchored below the button row.** In
   `src/tactical/FBattleDisplay.cpp`, `drawPlaceMines()` (~line 1413) draws the
   source-selection rows starting at `int y = getActionButtonRowBottom();`
   (~line 1430) at a hardcoded left margin `int lMargin = 310;` (~line 1425).
   `drawPlaceSeekers()` (~line 1490) mirrors this: source rows start at
   `getActionButtonRowBottom()` at `lMargin = 310`, and the placed-seeker recall
   list starts at `int cy = getActionButtonRowBottom();` at horizontal center
   `const int centerMargin = panelW / 2;` (~lines 1553–1554).
2. **Instruction text is a single line.** `drawPlaceMines()` draws
   `"The defending player may now place mines before the attacker sets up their
   ships."` at `(leftOffset, getActionPromptLineY(0))` (~line 1423) on one line.
   `drawPlaceSeekers()` draws the analogous seeker string at the same position
   (~line 1500) on one line. `leftOffset = 2*BORDER + ZOOM_SIZE = 40`.
3. **`getActionButtonRowBottom()`** (`src/tactical/FBattleDisplay.cpp` ~lines
   486–517) returns the bottom Y of the lowest shown action button (plus a
   `BORDER` gap), or a computed fallback when no button is shown. The Done
   buttons (`m_buttonMinePlacementDone`, `m_buttonSeekerPlacementDone`) are laid
   out in a horizontal sizer (`actionSizer`) at the left, below the instruction
   text region.
4. **Attack-phase offensive-seeker recall list is anchored below the button
   row.** In `draw()` (~lines 685–688), when `getPhase() == PH_ATTACK_FIRE`,
   `drawOffensiveSeekerPendingRows(dc, leftOffset, getActionButtonRowBottom() +
   BORDER, 10)` is called; the list draws at the left margin (`leftOffset = 40`)
   starting below the button row. Its click handler
   `checkOffensiveSeekerPendingSelection()` (~line 1775) is routed from
   `onLeftUp()` for `PH_ATTACK_FIRE`.
5. **The panel height ratchets up and never shrinks.**
   `m_lowerPanelLayoutState.requestedDisplayHeight` (struct ~lines 175–182) is
   initialized to 120 (~line 62). Draw helpers (`drawPlaceMines`,
   `drawPlaceSeekers`, `drawSeekerActivation`, `drawOffensiveSeekerPendingRows`)
   raise it when content overflows and call `applyRequestedDisplayHeight()`
   (~lines 595–612), which sets the panel min-size and triggers a parent
   `SendSizeEvent()` reflow (guarded by `m_inResizeReflow`).
   `ensureLowerPanelLayoutState()` (~lines 519–593) explicitly preserves the
   larger value: `if (m_lowerPanelLayoutState.requestedDisplayHeight >
   requestedHeight) requestedHeight = m_lowerPanelLayoutState.requestedDisplayHeight;`
   (~lines 589–591). There is **no** code path that reduces
   `requestedDisplayHeight`. (This max-preserve behavior was added by SMF-02 /
   PGS-02 to stop the stats-based layout pass from clobbering draw-helper
   expansion and re-clipping rows; any shrink change must not reintroduce that
   clipping regression.)
6. **Move-phase seeker impacts ARE removed.**
   `FTacticalGame::completeMovePhase()` (~lines 3185–3240) calls
   `checkForActiveSeekersOnPath()` per moving ship (records contacts into
   `m_pendingSeekerContactOutcomes`), then `applyMovementSeekerDamage()`
   (~line 3231). `applyMovementSeekerDamage()` (~lines 3071–3104) collects the
   detonating seeker IDs, resolves damage via
   `resolvePendingSeekerDetonationDamage()`, then **erases** each detonated
   seeker from `m_seekerMissiles` by ID (~lines 3093–3103).
7. **Activation-phase seeker impacts are NOT removed (root cause of defect 4).**
   `FTacticalGame::completeSeekerActivationPhase()` (~lines 456–468) calls
   `resolveActiveSeekersForMovingPlayer()` — which intentionally keeps an
   impacting seeker alive in the rebuilt list (SMF-06, so it stays visible during
   ICM/damage dialogs) — and then `resolvePendingSeekerDetonationDamage()`
   (~line 464). This applies damage but contains **no erase of the impacted
   seeker**, and no `applyMovementSeekerDamage()` (or equivalent erase) is called
   on this path. The impacted seeker therefore survives as an `active` seeker and
   re-contacts/attacks on the next phase. This is the confirmed root cause.
8. **`resolvePendingSeekerDetonationDamage()`** (~lines 1291–1375) builds
   temporary `FWeapon::SM` weapons against the contacted targets, runs ICM
   selection, fires them, shows the damage summary, and clears the tactical
   report. It does not modify `m_seekerMissiles` membership — removal is the
   caller's responsibility (only `applyMovementSeekerDamage()` does it today).
9. **Reviewer follow-ups** (from
   `artifacts/pregame-ordnance-placement/reviewer_report.md`, outcome PASS, NOTE
   items): (a) a now-unreachable `else if (getState() == BS_PlaceSeekers)` branch
   in `FTacticalGame::placeOrdnanceAtHex()` (~line 3351), dead since PGS-03's
   early return takes all `BS_PlaceSeekers` SM placements; (b) a low-risk
   mine-removal fallthrough at `placeOrdnanceAtHex()` (~lines 3306–3358) where a
   hex present in `m_minedHexList` with no matching `FWeapon::M` placed-ordnance
   record is not erased; (c) an include-guard typo
   `FTACTICALPREAGAMEORDNANCETEST_H_` (PREAGAME → PREGAME) in
   `tests/tactical/FTacticalPreGameOrdnanceTest.h:15`.
10. `doc/rules/tactical_operations_manual.md` is a protected external artifact
    and must not be edited.

## Resolved Design Decisions

1. **(D1) Pre-game placement anchor.** All pre-game placement row/list groups
   anchor their top to the top of the bottom panel (the same top region as the
   instruction text), not to `getActionButtonRowBottom()`. The left column holds
   the (now two-line) instruction text with the "Done" button below it; the
   row/list groups occupy the region to the right of that left column.
2. **(D2) Seeker-phase arrangement = side-by-side columns.** During
   `BS_PlaceSeekers`, the ship-source-selection rows render in a middle column
   and the placed-seeker recall list renders in a right column, both anchored to
   the panel top and to the right of the left column. (Chosen over a single
   stacked column to keep the panel shorter.)
3. **(D3) Instruction text wrapping.** Both the mine and the seeker pre-game
   instruction strings wrap onto two lines. The string text is unchanged; only
   its layout becomes two lines. The left column width is sized so the right-side
   columns begin to the right of the wrapped text block and the Done button.
4. **(D4) Shrink semantics = shrink at phase change.** The panel keeps any
   expanded height for the duration of the current tactical phase and recomputes
   its height to fit current content when the tactical phase changes, so it
   shrinks back when the extra space is no longer needed. Within a phase, content
   that overflows must still expand the panel and remain visible/clickable (no
   re-introduction of the row-clipping regression that the existing max-preserve
   behavior fixed).
5. **(D5) Attack-phase recall anchor.** The `PH_ATTACK_FIRE` offensive-seeker
   recall list anchors to the top of the bottom panel, in a column to the right
   of the "Done" button — matching the pre-game treatment.
6. **(D6) Seeker removal is path-agnostic and behavioral.** A seeker that
   contacts a ship and has its attack resolved + any damage applied is removed
   from `m_seekerMissiles` in every impact-resolution path, including the
   activation-phase path. The move-phase removal that already works
   (`applyMovementSeekerDamage()`) and the SMF-06 "seeker visible during the
   ICM/damage dialog, removed afterward" behavior are both preserved.

## Assumptions

1. The defect set and layout changes are confined to the tactical surfaces
   `FBattleDisplay` (lower-panel HUD rendering/click routing) and `FTacticalGame`
   (seeker-resolution model). No `FBattleScreen` delegation change is expected
   for the seeker-removal fix (it is internal to `FTacticalGame`), and no
   weapon-stat, ship-class, strategic-layer, or asset changes are required.
   (Labeled assumption; each subtask's allowed-files list reflects the expected
   minimal surface and the Implementer confirms during work.)
2. The per-ship stats/status widget is either not rendered in the right-side
   region during the pre-game placement phases or can be laid out so it does not
   overlap the repositioned row/list columns; the prior round already required
   placement rows to not render behind the ship-status display, so each layout
   subtask treats "no overlap with the ship-status display" as a constraint.
   (Labeled assumption; Implementer confirms the actual draw state for the phase.)
3. The existing offscreen `wxMemoryDC` GUI test approach
   (`tests/gui/TacticalGuiLiveTest.*`, `WXGuiTestHarness`) and the tactical model
   tests (`tests/tactical/*`) are sufficient to verify the layout, shrink, and
   removal behaviors behaviorally (row click-region alignment, panel min-height
   changes, seeker count after impact). (Labeled assumption.)
4. "Top of the bottom panel" means the same top Y used by the instruction text
   (`getActionPromptLineY(0)` / the panel's top margin), and "right of the left
   column" means horizontally past the wider of the wrapped instruction text and
   the Done button. Exact pixel offsets are an Implementer choice constrained by
   the no-overlap / click-region-alignment acceptance criteria. (Labeled
   assumption.)

## Likely Files To Modify

1. `src/tactical/FBattleDisplay.cpp` / `include/tactical/FBattleDisplay.h`
   (HUD layout for SMRIV-01, SMRIV-02, SMRIV-03; panel-height lifecycle for
   SMRIV-04).
2. `src/tactical/FTacticalGame.cpp` / `include/tactical/FTacticalGame.h`
   (seeker-removal fix for SMRIV-05; cleanup for SMRIV-06).

(Labeled as likely files based on repository evidence; the exact subset per
subtask is given in each Implementer prompt's allowed-files list.)

## Subtasks

### SMRIV-01 — Mine-Phase HUD Layout: Anchor Source Rows To Panel Top, Wrap Instruction Text

Reposition the `BS_PlaceMines` ship-source-selection rows so they anchor to the
top of the bottom panel and render to the right of the left column (instruction
text + "Done" button), instead of starting at `getActionButtonRowBottom()`. Wrap
the mine instruction text onto two lines. This subtask establishes the
"anchor-to-panel-top, right-of-left-column" layout convention reused by SMRIV-02
and SMRIV-03.

Acceptance Criteria:
- During `BS_PlaceMines`, the source-selection rows render anchored to the top of
  the bottom panel (their top aligns with the instruction-text top region, not
  with the bottom of the action-button row) and in a region to the right of the
  left column.
- The mine instruction text renders on two lines.
- Every ship with a deployable mine launcher still appears as a selectable source
  row, each row's click region aligns with its drawn position, and the rows are
  not clipped or rendered behind the "Done" button / left column / ship-status
  display.
- Clicking a source row still selects that ship+weapon for subsequent board
  clicks (existing selection behavior is preserved); with one eligible ship it is
  auto-selected.

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` mine-phase
  render/layout helper(s) (`drawPlaceMines` and any layout helper changed). No
  rules-document edit.

### SMRIV-02 — Seeker-Phase HUD Layout: Side-By-Side Columns Anchored To Panel Top, Wrap Instruction Text

Reposition the `BS_PlaceSeekers` content so the ship-source-selection rows and
the placed-seeker undeploy/recall list both anchor to the top of the bottom panel
and render as side-by-side columns to the right of the left column: selection
rows in a middle column, placed-seeker recall list in a right column (no longer
horizontally centered, no longer below the button row). Wrap the seeker
instruction text onto two lines.

Acceptance Criteria:
- During `BS_PlaceSeekers`, the ship-source-selection rows render in a middle
  column and the placed-seeker recall list renders in a right column, both
  anchored to the top of the bottom panel and to the right of the left column.
- The seeker instruction text renders on two lines.
- Each rendered row in both columns has a click region that aligns with its drawn
  position; rows are not clipped and do not overlap each other, the left column,
  or the ship-status display.
- Selecting a source row still updates the active placement source; clicking a
  placed-seeker recall row still undeploys exactly one seeker for that
  (hex, source) and returns ammo to that ship (existing PGS-04 behavior is
  preserved, only repositioned).

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` seeker-phase
  render/layout helper(s) (`drawPlaceSeekers` and any layout helper changed). No
  rules-document edit.

### SMRIV-03 — Attack-Phase Offensive-Seeker Recall List: Anchor To Panel Top, Right Of Done Button

Reposition the `PH_ATTACK_FIRE` offensive-seeker recall/unplace list so it
renders in a column to the right of the "Done" button, anchored to the top of the
bottom panel, instead of starting below the button row at the left margin.

Acceptance Criteria:
- During `PH_ATTACK_FIRE`, the offensive-seeker recall list renders anchored to
  the top of the bottom panel and to the right of the "Done" button (not below
  the action-button row).
- Each recall row's click region aligns with its drawn position; rows are not
  clipped or rendered behind the button / ship-status display.
- Clicking a recall row still recalls the corresponding pending offensive seeker
  (existing `recallSelectedOffensivePendingSeekerAtHex` behavior is preserved,
  only repositioned).

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` attack-phase recall
  render helper(s) (`drawOffensiveSeekerPendingRows` and any layout helper
  changed). No rules-document edit.

### SMRIV-04 — Lower-Panel Height Shrinks At Phase Change

Change the lower-panel height lifecycle so it no longer ratchets up permanently.
Keep the expanded height within a tactical phase, but recompute the requested
display height to fit current content when the tactical phase changes, so the
panel shrinks back when the extra space is no longer needed. Do not reintroduce
the within-phase row-clipping regression that the current max-preserve behavior
prevents.

Acceptance Criteria:
- After a tactical phase that expanded the panel height transitions to a phase
  whose content needs less height, the panel's requested/min height decreases
  (it shrinks back, rather than remaining at the previously expanded height).
- Within a single phase, content that overflows the current height still expands
  the panel, and the overflowing rows remain visible and clickable (the existing
  no-clipping behavior is preserved within a phase).
- The minimum panel height floor (120 px) is preserved; the panel never shrinks
  below the floor.
- A behavioral test demonstrates the shrink-at-phase-change: a phase that raises
  the requested display height followed by a phase change recomputes the height
  to a smaller value when current content requires less, and this test fails
  against the current ratchet-only behavior and passes after the fix.

Documentation Impact:
- Update Doxygen comments for the modified panel-height lifecycle helpers
  (`ensureLowerPanelLayoutState`, `applyRequestedDisplayHeight`, and any
  phase-change hook added). No rules-document edit.

### SMRIV-05 — Remove Seekers After Impact In The Activation-Phase Resolution Path

Fix the defect where a seeker that impacts a ship during the seeker-activation
phase is not removed and attacks again. Ensure that after a seeker contacts a
ship and its attack is resolved and any damage applied, the impacting seeker is
removed from `m_seekerMissiles` in the activation-phase path
(`completeSeekerActivationPhase()` → resolution), matching the move-phase path
that already removes impacted seekers via `applyMovementSeekerDamage()`. Preserve
the SMF-06 behavior (the impacting seeker remains visible at its final hex during
the ICM/damage dialog and is removed afterward) and do not change the working
move-phase removal.

Acceptance Criteria:
- After a seeker impacts a ship during the seeker-activation phase and its attack
  is resolved (damage applied or not), that seeker is removed from
  `m_seekerMissiles` and does not contact or attack any ship on the next
  phase/turn.
- The impacting seeker still remains present for rendering through the
  ICM/damage-summary display and is removed after the resolution completes
  (SMF-06 preserved).
- Move-phase seeker impacts continue to be removed exactly once (no regression to
  `completeMovePhase()` / `applyMovementSeekerDamage()`), and non-impacting
  seekers survive with their movement bookkeeping intact.
- A behavioral (not source-text-match) model test drives an activation-phase
  seeker impact and asserts the seeker count in `m_seekerMissiles` decreases by
  the number of impacting seekers and that no surviving seeker remains at the
  impacted contact; this test fails against the current (unfixed) code and passes
  after the fix.

Documentation Impact:
- Update Doxygen comments for the changed seeker-resolution methods
  (`completeSeekerActivationPhase`, and any shared removal helper such as
  `applyMovementSeekerDamage` if it is reused/refactored for this path). No
  rules-document edit.

### SMRIV-06 — Reviewer Follow-Up Cleanup (Dead Branch + Defensive Mine-Hex Erase)

Apply the two production-code cleanup/hardening follow-ups from the previous
plan's Reviewer report: remove the now-unreachable `else if (getState() ==
BS_PlaceSeekers)` SM-removal branch in `FTacticalGame::placeOrdnanceAtHex()`, and
add a defensive erase from `m_minedHexList` for the rare case where a mined hex
has no matching `FWeapon::M` placed-ordnance record (so a stale minefield cannot
remain when a non-placing source is selected). These are behavior-preserving for
all normal flows.

Acceptance Criteria:
- The unreachable `else if (getState() == BS_PlaceSeekers)` branch in
  `placeOrdnanceAtHex()` is removed with no change to observable placement,
  removal, or undeploy behavior in any state.
- Pre-game mine removal still removes the minefield and restores ammo to the
  placing ship for the normal flow; additionally, when a hex is in
  `m_minedHexList` with no matching placed-ordnance record, the hex is still
  erased from `m_minedHexList` (no stale minefield remains).
- The existing pre-game ordnance behavioral tests continue to pass (mine
  toggle/removal, additive seeker stacking, seeker undeploy-by-list), confirming
  no regression from the cleanup.

Documentation Impact:
- Update Doxygen comments for `placeOrdnanceAtHex()` if its branch structure
  changes. No rules-document edit.

## Carried-Over Items From pregame-ordnance-placement Reviewer Report

The pregame-ordnance-placement Reviewer report
(`artifacts/pregame-ordnance-placement/reviewer_report.md`, outcome PASS) listed
three non-blocking NOTE follow-ups. They are addressed here as follows:

1. **(Carry-A) Unreachable `BS_PlaceSeekers` branch in `placeOrdnanceAtHex()`** —
   addressed as production work in **SMRIV-06**.
2. **(Carry-B) Defensive `m_minedHexList` erase for the mined-hex-without-record
   fallthrough** — addressed as production work in **SMRIV-06**.
3. **(Carry-C) Include-guard typo in
   `tests/tactical/FTacticalPreGameOrdnanceTest.h:15`
   (`FTACTICALPREAGAMEORDNANCETEST_H_`, PREAGAME → PREGAME).** This is a
   **test-file** change and is Tester-owned; it is **not** an Implementer subtask
   and the test file is **not** placed in any Implementer allowed-files list. The
   Coordinator should route this cosmetic correction to the Tester stage of any
   convenient subtask in this plan (for example SMRIV-05, whose Tester stage also
   touches `tests/tactical/`), preserving the guard's self-consistency
   (define/ifndef/endif all updated together).

## Dependency Ordering

Treat this plan as **serial**. Recommended order:

1. **SMRIV-01** first — establishes the "anchor-to-panel-top, right-of-left-column"
   layout convention and wraps the instruction text in the mine phase; SMRIV-02
   and SMRIV-03 build on the same `FBattleDisplay` layout surface and convention.
2. **SMRIV-02** after SMRIV-01 — applies the convention to the more complex
   two-column seeker phase; shares the same `FBattleDisplay` draw/layout helpers.
3. **SMRIV-03** after SMRIV-02 — applies the convention to the attack-phase recall
   list; same `FBattleDisplay` surface.
4. **SMRIV-04** after SMRIV-03 — the height shrink lifecycle must account for the
   repositioned content from SMRIV-01–03; doing layout first gives a stable
   content model from which to compute (and shrink) the height.
5. **SMRIV-05** after SMRIV-04 — independent file (`FTacticalGame`), sequenced after
   the HUD work for conservative serial execution.
6. **SMRIV-06** last — also touches `FTacticalGame.cpp` (a different function,
   `placeOrdnanceAtHex()`, than SMRIV-05's `completeSeekerActivationPhase()`);
   sequencing after SMRIV-05 avoids overlapping edits to the same file.

Parallelization note: SMRIV-01 through SMRIV-04 all edit `FBattleDisplay.cpp/.h`
and share the same layout/height helpers, so they must run serially. SMRIV-05 and
SMRIV-06 both edit `FTacticalGame.cpp` (different functions) and must run serially
with respect to each other. The `FBattleDisplay` track (01–04) and the
`FTacticalGame` track (05–06) touch disjoint files, so a coordinator could run
them as two parallel lanes; however, this plan conservatively recommends a single
serial sequence and marks **no** subtask as parallelizable.

## Overall Acceptance Criteria

1. During pre-game mine placement, the source-selection rows render anchored to
   the top of the bottom panel and to the right of the left column, and the mine
   instruction text wraps onto two lines; selection and placement behavior are
   unchanged.
2. During pre-game seeker placement, the source-selection rows (middle column) and
   the placed-seeker recall list (right column) render side-by-side anchored to
   the top of the bottom panel and to the right of the left column, and the seeker
   instruction text wraps onto two lines; selection, placement, and undeploy
   behavior are unchanged.
3. During the attack/offensive-fire phase, the offensive-seeker recall list
   renders to the right of the "Done" button anchored to the top of the bottom
   panel; recall behavior is unchanged.
4. In all repositioned lists, click regions align with drawn positions and rows
   are not clipped or rendered behind the buttons, left column, or ship-status
   display.
5. The bottom panel keeps its expanded height within a tactical phase and shrinks
   back (recomputes to fit current content) at phase changes, never below the
   120 px floor, without reintroducing within-phase row clipping.
6. A seeker that impacts a ship and has its attack resolved is removed from the
   board in every resolution path (move phase and activation phase) and does not
   attack again; the impacting seeker remains visible during its ICM/damage dialog
   and is removed afterward (SMF-06 preserved).
7. The unreachable `BS_PlaceSeekers` branch in `placeOrdnanceAtHex()` is removed
   and a defensive `m_minedHexList` erase is added, with no regression to existing
   pre-game ordnance behavior.
8. The `tests/tactical/FTacticalPreGameOrdnanceTest.h` include-guard typo is
   corrected by the Tester stage (Carry-C).
9. New behavioral coverage exists for the panel shrink-at-phase-change (SMRIV-04)
   and the activation-phase seeker removal (SMRIV-05), each failing against the
   current code and passing after the fix.
10. No change edits `doc/rules/tactical_operations_manual.md`.

## Documentation Impact

Documentation impact is low and concentrated in header Doxygen comments for the
modified `FBattleDisplay` HUD render/layout helpers (`drawPlaceMines`,
`drawPlaceSeekers`, `drawOffensiveSeekerPendingRows`, `ensureLowerPanelLayoutState`,
`applyRequestedDisplayHeight`, and any new layout/phase-change helpers) and the
modified `FTacticalGame` seeker-resolution / placement methods
(`completeSeekerActivationPhase`, `applyMovementSeekerDamage` if refactored,
`placeOrdnanceAtHex`). The Documenter stage may update `doc/UsersGuide.md` and/or
`doc/DesignNotes.md` if they describe the tactical HUD layout or seeker lifecycle.
`doc/rules/tactical_operations_manual.md` must not be edited.

## Implementer Prompts

### SMRIV-01 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Reposition the pre-game mine-placement (`BS_PlaceMines`) ship-source-selection
rows in the lower panel. Today `drawPlaceMines()` (in
`src/tactical/FBattleDisplay.cpp`) draws those rows starting at `int y =
getActionButtonRowBottom();` at a hardcoded `int lMargin = 310;`, i.e. below the
action-button row. Change the layout so the source-selection rows anchor their
top to the top of the bottom panel (the same top region as the instruction text,
e.g. `getActionPromptLineY(0)` / the panel top margin) and render in a region to
the right of the left column (the instruction text and the "Done" button), rather
than below the button row. Also wrap the mine instruction text — "The defending
player may now place mines before the attacker sets up their ships." — onto two
lines (the string content is unchanged; only its rendering becomes two lines), so
the left column holds the wrapped text with the "Done" button below it. Ensure
each rendered source row's click region (`m_shipNameRegions` /
`m_shipSelectionSourceIndices`) matches its new drawn position so clicks still
select the intended source, and ensure the rows do not overlap the left column or
the ship-status display and are not clipped. Do not change the model-level
selection, ammo, or placement logic beyond what is needed to reposition and keep
the rows clickable. Do not edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `BS_PlaceMines`, the source-selection rows render anchored to the top of
  the bottom panel (top aligned with the instruction-text top region, not the
  bottom of the action-button row) and to the right of the left column.
- The mine instruction text renders on two lines.
- Every ship with a deployable mine launcher still appears as a selectable source
  row; each row's click region aligns with its drawn position; rows are not
  clipped or rendered behind the "Done" button, left column, or ship-status
  display.
- Clicking a source row still selects that ship+weapon for subsequent board
  clicks; with exactly one eligible ship it is auto-selected.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live layout / click-region behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` plus the
  `WXGuiTestHarness` helpers); source-contract coverage for the layout helper
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round4/SMRIV-01/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRIV-02 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Reposition the pre-game seeker-placement (`BS_PlaceSeekers`) lower-panel content.
Today `drawPlaceSeekers()` draws the ship-source-selection rows starting at
`getActionButtonRowBottom()` at `lMargin = 310`, and the placed-seeker
undeploy/recall list starting at `int cy = getActionButtonRowBottom();` at
horizontal center `panelW / 2`. Change the layout so both groups anchor their top
to the top of the bottom panel and render as side-by-side columns to the right of
the left column (the instruction text + "Done" button): the ship-source-selection
rows in a middle column and the placed-seeker recall list in a right column (no
longer horizontally centered, no longer below the button row). Also wrap the
seeker instruction text — "The defending player may now place seeker missiles
before the attacker sets up their ships." — onto two lines (string content
unchanged). Ensure each rendered row's click region in both columns
(`m_shipNameRegions` for selection, `m_preGameSeekerRecallRegions` and its
parallel arrays for recall) matches its new drawn position so clicks still select
/ recall correctly, and ensure the two columns do not overlap each other, the
left column, or the ship-status display, and are not clipped. Preserve the
existing PGS-04 behavior (source selection updates the active source; clicking a
recall row undeploys exactly one seeker for that (hex, source) and returns ammo)
— only reposition it. Do not edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `BS_PlaceSeekers`, the ship-source-selection rows render in a middle
  column and the placed-seeker recall list renders in a right column, both
  anchored to the top of the bottom panel and to the right of the left column.
- The seeker instruction text renders on two lines.
- Each rendered row in both columns has a click region that aligns with its drawn
  position; rows are not clipped and do not overlap each other, the left column,
  or the ship-status display.
- Selecting a source row still updates the active placement source; clicking a
  placed-seeker recall row still undeploys exactly one seeker for that
  (hex, source) and returns one round of ammo to that ship.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live layout / click-region behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` plus the
  `WXGuiTestHarness` helpers); source-contract coverage for the layout helper
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round4/SMRIV-02/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRIV-03 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Reposition the attack/offensive-fire phase (`PH_ATTACK_FIRE`) offensive-seeker
recall/unplace list in the lower panel. Today it is drawn from `draw()` via
`drawOffensiveSeekerPendingRows(dc, leftOffset, getActionButtonRowBottom() +
BORDER, 10)`, i.e. at the left margin starting below the action-button row. Change
the layout so the recall list renders in a column to the right of the "Done"
button and anchors its top to the top of the bottom panel (consistent with the
pre-game placement treatment), rather than below the button row. Ensure each
recall row's click region (`m_pendingSeekerRecallRegions` /
`m_pendingSeekerRecallHexes`) matches its new drawn position so
`checkOffensiveSeekerPendingSelection()` still recalls the intended pending
seeker, and ensure the rows are not clipped or rendered behind the "Done" button
or the ship-status display. Do not change the underlying pending-seeker model or
recall logic beyond repositioning. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `PH_ATTACK_FIRE`, the offensive-seeker recall list renders anchored to
  the top of the bottom panel and to the right of the "Done" button (not below the
  action-button row).
- Each recall row's click region aligns with its drawn position; rows are not
  clipped or rendered behind the "Done" button or ship-status display.
- Clicking a recall row still recalls the corresponding pending offensive seeker
  (existing recall behavior preserved, only repositioned).

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live layout / click-region behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` plus the
  `WXGuiTestHarness` helpers); source-contract coverage for the layout helper
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round4/SMRIV-03/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRIV-04 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Change the lower-panel height lifecycle so it no longer ratchets up permanently.
Today `m_lowerPanelLayoutState.requestedDisplayHeight` is only ever increased:
draw helpers raise it when content overflows and call
`applyRequestedDisplayHeight()`, and `ensureLowerPanelLayoutState()` preserves the
larger value (`if (m_lowerPanelLayoutState.requestedDisplayHeight > requestedHeight)
requestedHeight = m_lowerPanelLayoutState.requestedDisplayHeight;`). Implement the
behavior that the panel keeps any expanded height for the duration of the current
tactical phase but recomputes its requested display height to fit current content
when the tactical phase changes (for example by detecting a phase change and
resetting the draw-helper-driven expansion so the next layout pass recomputes
height from current content), so the panel shrinks back when the extra space is no
longer needed. Preserve the within-phase no-clipping behavior: content that
overflows within a phase must still expand the panel and remain visible/clickable
(do not reintroduce the row-clipping regression that the current max-preserve
behavior was added to fix). Preserve the 120 px minimum-height floor. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- After a phase that expanded the panel height transitions to a phase whose
  content needs less height, the panel's requested/min height decreases (shrinks
  back) rather than remaining at the previously expanded height.
- Within a single phase, overflowing content still expands the panel and the
  overflowing rows remain visible and clickable.
- The 120 px minimum-height floor is preserved; the panel never shrinks below it.
- Add a behavioral (not source-text-match) test that raises the requested display
  height in one phase, changes the phase to one whose content needs less height,
  and asserts the recomputed height is smaller; this test must fail against the
  current ratchet-only behavior and pass after the fix.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Panel-height shrink/grow behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` driving the draw
  helpers across a phase change and asserting on the panel min-height /
  requested display height); source-contract coverage for the height-lifecycle
  helper belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round4/SMRIV-04/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRIV-05 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`

Task to implement:
Fix the defect where a seeker missile that impacts a ship during the
seeker-activation phase is not removed from the board and attacks again on later
phases/turns. In `src/tactical/FTacticalGame.cpp`,
`completeSeekerActivationPhase()` calls `resolveActiveSeekersForMovingPlayer()`
(which intentionally keeps the impacting seeker alive in the rebuilt list per
SMF-06) and then `resolvePendingSeekerDetonationDamage()`, but — unlike the
move-phase path `completeMovePhase()` → `applyMovementSeekerDamage()`, which
collects the detonating seeker IDs and erases them from `m_seekerMissiles` after
resolution — the activation-phase path never erases the impacted seekers. Make
the activation-phase resolution remove every seeker that contacted a ship after
its attack/damage is resolved (for example by routing the activation path through
the same removal mechanism used by `applyMovementSeekerDamage()`, or by factoring
the "collect detonated IDs → resolve damage → erase by ID" sequence into a shared
helper used by both paths). Preserve SMF-06 (the impacting seeker remains visible
at its final hex during the ICM/damage dialog and is removed only after
resolution returns) and do not change the working move-phase removal or the
behavior of non-impacting seekers (their `movementAllowance`/`movementTurn`
bookkeeping must remain intact). Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- After a seeker impacts a ship during the seeker-activation phase and its attack
  is resolved (whether or not damage is applied), that seeker is removed from
  `m_seekerMissiles` and does not contact or attack any ship on the next
  phase/turn.
- The impacting seeker still remains present for rendering through the
  ICM/damage-summary display and is removed only after the resolution completes
  (SMF-06 preserved).
- Move-phase seeker impacts continue to be removed exactly once (no regression),
  and non-impacting seekers survive with their movement bookkeeping intact.
- Add a behavioral (not source-text-match) model test that drives an
  activation-phase seeker impact (a seeker that contacts a ship during activation
  resolution) and asserts that `m_seekerMissiles` no longer contains the impacting
  seeker after resolution and that it does not attack on the following phase; this
  test must fail against the current (unfixed) code and pass after the fix.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Activation-phase seeker-removal model coverage belongs in
  `tests/tactical/FTacticalSeekerMovementTest.*` or
  `tests/tactical/FTacticalGameMechanicsTest.*`. (The Tester stage of this subtask
  may also correct the include-guard typo in
  `tests/tactical/FTacticalPreGameOrdnanceTest.h:15`, PREAGAME → PREGAME, per the
  plan's Carry-C item.)

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round4/SMRIV-05/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRIV-06 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`

Task to implement:
Apply two behavior-preserving cleanup/hardening follow-ups from the previous
plan's Reviewer report in `FTacticalGame::placeOrdnanceAtHex()`
(`src/tactical/FTacticalGame.cpp`). First, remove the now-unreachable `else if
(getState() == BS_PlaceSeekers)` SM-removal branch — it is dead because PGS-03's
earlier early-return already takes all `BS_PlaceSeekers` SM placements, so this
branch can never execute. Second, add a defensive erase from `m_minedHexList` for
the rare case where a hex is present in `m_minedHexList` but no matching
`FWeapon::M` placed-ordnance record exists, so the mine-removal path always clears
the hex (no stale minefield can remain when a non-placing source is selected),
rather than falling through to a selection-only removal that leaves it. Make no
other behavioral change to placement, removal, or undeploy in any state. Do not
edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- The unreachable `else if (getState() == BS_PlaceSeekers)` branch in
  `placeOrdnanceAtHex()` is removed and no observable placement/removal/undeploy
  behavior changes in any state.
- For the normal flow, pre-game mine removal still removes the minefield and
  restores ammo to the placing ship; additionally, when a hex is in
  `m_minedHexList` with no matching placed-ordnance record, the hex is still
  erased from `m_minedHexList` (no stale minefield remains).
- The existing pre-game ordnance behavioral tests (mine toggle/removal, additive
  seeker stacking, seeker undeploy-by-list) continue to pass, confirming no
  regression.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Mine-removal / placement-dispatch model coverage belongs in
  `tests/tactical/FTacticalGameMechanicsTest.*` or
  `tests/tactical/FTacticalPreGameOrdnanceTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round4/SMRIV-06/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.
