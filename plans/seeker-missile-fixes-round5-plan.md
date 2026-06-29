# Seeker Missile Fixes Round 5 Plan

## Output Artifact Path

`plans/seeker-missile-fixes-round5-plan.md`

Coordinator artifact root for this plan: `artifacts/seeker-missile-fixes-round5/`
with per-subtask subdirectories `SMRV-01`, `SMRV-02`, `SMRV-03`. The three
test-track follow-ups (items 4–6 below) are Tester-owned and write their
artifacts under the host subtask directory whose Tester stage performs them (see
"Test-Track Follow-Ups").

## Feature Restatement

Continuation of the tactical seeker-missile / pre-game ordnance work. Round 4
established a HUD layout convention for the lower (bottom) panel: pre-game
placement panels anchor their top to `getActionPromptLineY(0)` (top of the lower
panel), source-selection rows render in a right/middle column at `lMargin = 310`,
and (for seeker placement) the recall list renders in a right column at
`recallMargin = 620`, while the left column holds wrapped instruction text with
the "Done" button below it. Round 5 corrects/extends that convention to two
panels Round 4 missed, fixes one board-rendering defect, and clears three
carried-over Reviewer follow-ups. Concretely:

1. **Attack-phase "Pending seeker deployments" panel overlaps the left
   instruction text.** During the attack/offensive-fire phase (`PH_ATTACK_FIRE`)
   the offensive-seeker pending-deployment panel (`drawOffensiveSeekerPendingRows`,
   at `lMargin = 310`) horizontally collides with the left-column instruction
   text ("Select legal path hexes to deploy seeker missiles.") because that left
   text is drawn unwrapped/unbounded at `x = leftOffset = 40` and overruns past
   `x = 310`. **User's explicit preference:** shift/resize the pending-deployment
   panel clear to the **right** of the left column (instruction-text + button
   area) rather than wrapping the instruction text.
2. **Activation-phase "Activated seekers" panel is anchored below the button,
   not at panel top.** During the seeker-activation phase (`PH_SEEKER_ACTIVATION`)
   `drawSeekerActivation()` still anchors the "Activated seekers:" list at
   `y = getActionButtonRowBottom()` (the older SMF-05 below-button convention)
   instead of at `getActionPromptLineY(0)` (panel top), even though the pre-game
   PLACEMENT panels were corrected in Round 4. Apply the Round 4
   anchor-to-panel-top, right-of-left-column convention to the activation-phase
   panel.
3. **Opponent seeker speed values render on the board during seeker activation.**
   During `PH_SEEKER_ACTIVATION` the opponent's seeker sprites are correctly
   hidden, but their red speed-value labels are still drawn (stray floating
   numbers). The sprite render path is owner-filtered; the speed-label render path
   (`drawSeekerMoveCountOverlay()`) is not. Suppress the opponent's speed labels
   under the same ownership condition that hides the sprite.

Plus three carried-over Reviewer follow-ups from
`artifacts/seeker-missile-fixes-round4/reviewer/reviewer_result.json`
(test-track, cleanup/hardening only): reconcile a pre-existing failing GUI test
with the accepted SMRIV-04 panel-height design, harden the GUI source-contract
path resolution, and de-flake one live modal-chain test.

## Confirmed Repository Facts

(All file/line references are from the current `sf2` working tree; line numbers
are approximate and may shift as edits land. Geometry constants:
`BORDER = 5`, `ZOOM_SIZE = 30`, `leftOffset = 2*BORDER + ZOOM_SIZE = 40`
(`src/tactical/FBattleDisplay.cpp:34–37`); `ACTION_PROMPT_TOP_MARGIN = 5`,
`ACTION_PROMPT_LINE_HEIGHT = 16` (`include/tactical/FBattleDisplay.h:149,151`), so
`getActionPromptLineY(0)=5`, `(1)=21`, `(2)=37`,
`src/tactical/FBattleDisplay.cpp:227–229`.)

1. **Attack-phase pending panel and left text share the same vertical band.** In
   `draw()` (`src/tactical/FBattleDisplay.cpp:713–718`), the `PH_ATTACK_FIRE`
   branch calls `drawOffensiveSeekerPendingRows(dc, 310, getActionPromptLineY(0),
   10)`. The panel's label "Pending seeker deployments:" is drawn at
   `lMargin = 310`, `y = getActionPromptLineY(0)` (~`src/tactical/FBattleDisplay.cpp:1755`).
   This is the same panel Round 4 SMRIV-03 repositioned — not a distinct panel.
2. **The left attack-phase instruction text is unwrapped/unbounded.**
   `drawAttackFire()` (~`src/tactical/FBattleDisplay.cpp:1220–1234`) draws
   "The moving player may now" / "declare offensive fire." and then, in
   offensive-seeker-deployment mode, "Select legal path hexes to deploy seeker
   missiles." at `x = leftOffset = 40` on prompt lines 0/1/2 with raw
   `DrawText` calls. Unlike the pre-game panels, this text is **not** clipped to a
   column width; the long line overruns past `x = 310` and collides horizontally
   with the pending panel on the same rows. (Root cause of defect 1.)
3. **Pre-game panels DO bound their left column.** `drawPlaceMines()` uses
   `lMargin = 310; instructionMaxWidth = lMargin - leftOffset - BORDER = 265`
   (~`src/tactical/FBattleDisplay.cpp:1456–1457`) and `drawPlaceSeekers()` does the
   same with `recallMargin = 620` (~`src/tactical/FBattleDisplay.cpp:1538–1540`).
   Both anchor content at `y = getActionPromptLineY(0)`. This is the Round 4
   convention to reuse.
4. **Pending-panel click regions.** `m_pendingSeekerRecallRegions`
   (`include/tactical/FBattleDisplay.h:134`) and `m_pendingSeekerRecallHexes`
   (`include/tactical/FBattleDisplay.h:136`) are populated in
   `drawOffensiveSeekerPendingRows()` (~`src/tactical/FBattleDisplay.cpp:1771–1772`)
   and hit-tested in `checkOffensiveSeekerPendingSelection()`
   (~`src/tactical/FBattleDisplay.cpp:1817–1822`). Click-region rects must be
   recomputed from the new draw position.
5. **Activation-phase list is anchored below the button row.**
   `drawSeekerActivation()` (~`src/tactical/FBattleDisplay.cpp:1670–1737`) sets
   `lMargin = 310` (~:1674) but `y = getActionButtonRowBottom()` (~:1677) and draws
   "Activated seekers:" at `(lMargin, y)` (~:1694). The left column draws the
   instruction lines at `leftOffset` on prompt lines 0/1/2 (~:1688–1690). The
   action button is `m_buttonSeekerActivationDone` ("Seeker Activation Done"):
   created ~:99, added to `actionSizer` ~:109/115, hidden ~:128, shown/connected
   ~:1726–1736, and included in the `getActionButtonRowBottom()` button array
   (~:513). Activation click regions are `m_seekerActivationRegions`
   (`include/tactical/FBattleDisplay.h:130`) and `m_seekerActivationSeekerIDs`
   (`include/tactical/FBattleDisplay.h:132`), populated ~:1713–1714. (Root cause of
   defect 2: the only change needed is the anchor `y`.)
6. **The seeker sprite render path is owner-filtered during activation.**
   `FBattleBoard::drawSeekerMissiles()` (`src/tactical/FBattleBoard.cpp:475`) has a
   `PH_SEEKER_ACTIVATION` block (~:481–500) that draws inactive seekers from
   `getInactiveSeekerActivationHexes()` (~:483–484) and active seekers from
   `getActiveSeekersByMovingPlayer()` (~:491–492), then `return`s. Both source
   helpers filter on `ownerID == getMovingPlayerID()`
   (`getActiveSeekersByMovingPlayer` `src/tactical/FTacticalGame.cpp:1050–1058`,
   condition `ownerID == getMovingPlayerID() && active` at ~:1054;
   `getInactiveSeekerActivationHexes` `src/tactical/FTacticalGame.cpp:932–937`,
   `ownerID != getMovingPlayerID() || active → skip`). So the opponent's sprites
   are not drawn during activation.
7. **The speed-label render path is NOT owner-filtered (root cause of defect 3).**
   `drawSeekerMoveCountOverlay()` is called unconditionally for `BS_Battle`
   (`src/tactical/FBattleBoard.cpp:117`) and defined at
   `src/tactical/FBattleBoard.cpp:575–635`. It iterates the full unfiltered
   `getSeekerMissiles()` list (~:581) and skips only on `!itr->active` (~:594) —
   there is **no** `ownerID` check. The red speed number (font 9pt swiss bold
   ~:611, color `#FF0000` ~:612, value = `path.size()-1` or `movementAllowance`
   ~:597–602, drawn upper-right of the hex ~:622–624, `DrawText` ~:632) is
   therefore drawn for the opponent's active seekers even though their sprites are
   hidden. `getSeekerMissiles()` returns all `m_seekerMissiles`
   (`include/tactical/FTacticalGame.h:622`); `FTacticalSeekerMissileState` carries
   `unsigned int ownerID` (`include/tactical/FTacticalGame.h:163`), `bool active`
   (~:166), `int movementAllowance` (~:168); the moving player is
   `getMovingPlayerID()`.
8. **Pre-existing failing GUI test (carried follow-up item 4).**
   `tests/gui/TacticalGuiLiveTest.cpp::testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges`
   fails at `tests/gui/TacticalGuiLiveTest.cpp:1233`
   (`CPPUNIT_ASSERT_EQUAL(beforeResizeHeight /*120*/, restoredHeight /*137*/)`).
   The block (~:1199–1233) stays in `PH_MOVE` throughout: it captures height 120,
   narrows the window to 760 (~:1216) which forces `LOWER_PANEL_LAYOUT_STACKED`
   (`src/tactical/FBattleDisplay.cpp:580–595`) raising the geometry-driven stacked
   height to **137**, then widens back (~:1229) and expects the height to return to
   120. Under the accepted SMRIV-04 lifecycle
   (`src/tactical/FBattleDisplay.cpp:601–621`: `phaseChanged` recomputes; the
   `else` branch at ~:614–618 preserves the expanded height within a phase), the
   widen-back stays in `PH_MOVE`, so 137 is preserved and the `EQUAL(120)`
   assertion is contradicted by design. 120 is the content-fit floor (floor at
   ~:597–599, ~:623–628; constructor seed ~:75; `SetMinSize(wxSize(-1,120))` ~:85);
   137 is the geometry-driven *stacked* height, not a content-overflow expansion.
   The current preserve logic does not distinguish content-overflow expansion from
   geometry-driven stacking expansion.
9. **Source-contract test path resolution is CWD-relative (carried follow-up
   item 5).** `tests/gui/TacticalGuiLiveTest.cpp:331–346` defines
   `sourceContainsLineToken()`, which opens a path via a CWD-relative `std::ifstream`
   (~:333) and silently skips a file it cannot open (~:334); its 22 call sites pass
   `"../../src/..."` (assume CWD = `tests/gui`).
   `tests/gui/BattleSimGuiLiveTest.cpp:80–85` (and
   `tests/gui/StrategicGuiLiveTest.cpp:228`) define `readFileText()`, also
   CWD-relative; BattleSim call sites pass **repo-root-relative** paths
   (e.g. `"src/battleSim/LocalGameDialog.cpp"` ~:533–535, ~:545, ~:552), while
   StrategicGuiLiveTest call sites pass `"../../src/..."`. The two conventions are
   mutually inconsistent, and a failed open is treated as empty content, so these
   tests false-fail (or pass vacuously) when `GuiTests` is launched from any CWD
   other than the one each call site assumed. An existing fixed-root pattern lives
   in the test tree: `tests/core/Makefile:19` defines
   `-DCORE_TEST_REPO_ROOT=\"$(CURDIR)/../..\"`, consumed at
   `tests/core/WXIconCacheTest.cpp:22`. (`FGameConfig::resolveAssetPath` resolves
   data/icon assets only, not arbitrary repo source files, so it is not the right
   tool here.)
10. **Flaky modal-chain test (carried follow-up item 6).**
    `tests/gui/BattleSimGuiLiveTest.cpp::testLocalGameDialogLaunchesPredefinedAndCustomModalChains`
    (~:446–523) already uses the deterministic harness helpers
    (`runVoidFunctionWithAction` ~:455/491, `waitForModalDialog(200,5)` ~:458/494,
    `pumpEvents` ~:450/477/483/512/519, `assertDialogCenteredOnParent` ~:463). The
    flakiness is a timeout race: the first chain's fallback auto-dismiss is 400ms
    (~:471) and the second chain's is 200ms (~:506), which ties the inner
    `waitForModalDialog(200ms)` poll — so a slow-appearing modal can be dismissed
    by the fallback before/while it is observed, leaving `launchedDialog == NULL`
    and tripping the presence assertions (~:472, :507). Harness helpers available
    for deterministic waits are declared in `tests/gui/WXGuiTestHarness.h:118–266`
    (`waitForTopLevelWindow`, `waitForTopLevelWindowClosed`, `waitForModalDialog`,
    `showModalWithAction`, `runModalFunctionWithAction`, `runVoidFunctionWithAction`).
11. `doc/rules/tactical_operations_manual.md` is a protected external artifact and
    must not be edited by any subtask.

## Resolved Design Decisions

1. **(D1) Item 1 fix = shift the pending panel right, do NOT wrap the left text.**
   Per the user's explicit preference, the `PH_ATTACK_FIRE` pending-deployment
   panel moves to a column whose left edge is clear to the **right** of the left
   column (the widest of the unwrapped attack-phase instruction lines and the
   "Done" button), anchored at the panel top (`getActionPromptLineY(0)`). The
   left instruction text is left as-is (not wrapped/clipped). The exact panel
   `lMargin` is an Implementer choice constrained by the no-overlap and
   click-region-alignment acceptance criteria (it may be a fixed margin chosen to
   clear the longest instruction line, or computed from the measured text extent),
   but it must be large enough that the pending panel never overlaps the left
   instruction text or the Done button at the maximum left-text width.
2. **(D2) Item 2 fix = adopt the Round 4 anchor.** The activation-phase
   "Activated seekers:" list anchors its top to `getActionPromptLineY(0)` (panel
   top), in the right/middle column at `lMargin = 310` (already its x), matching
   the pre-game placement convention — instead of `getActionButtonRowBottom()`.
   Click regions (`m_seekerActivationRegions`) must be recomputed from the new
   draw position so deactivation clicks still hit the correct rows.
3. **(D3) Item 3 fix = match the sprite-visibility condition, scoped to the
   activation phase.** `drawSeekerMoveCountOverlay()` suppresses the speed label
   for any seeker not owned by the moving player during `PH_SEEKER_ACTIVATION`
   (i.e. add an ownership guard equivalent to the sprite path —
   `ownerID == getMovingPlayerID()` — applied only in `PH_SEEKER_ACTIVATION`, or
   source the activation-phase labels from `getActiveSeekersByMovingPlayer()`),
   while preserving the existing behavior in all other `BS_Battle` phases where
   both players' labels are intended. The goal is that wherever the sprite is
   hidden during activation, its speed label is also suppressed, with no change to
   non-activation rendering.
4. **(D4) Items 4–6 are Tester-owned test-track work, not Implementer subtasks.**
   Under the recommended directions (D5 for item 4), none of items 4–6 requires
   production (`src/`) code; all are confined to the `tests/` tree (assertion
   reconciliation, test-harness path resolution + Makefile macro, test timing
   de-flake). Per the planner contract, these are routed to the Tester stages of
   the production subtasks rather than decomposed as Implementer subtasks (mirrors
   Round 4's handling of the Carry-C include-guard typo). See "Test-Track
   Follow-Ups" for exact routing. (Open question O1 records the approver's option
   to escalate item 4 to a production lifecycle change.)
5. **(D5) Item 4 recommended direction = update the test assertion (option a).**
   The failing assertion expects within-phase geometry-driven shrink-back, which
   directly contradicts the accepted SMRIV-04 design (AC5: "keep expanded height
   within a phase, recompute only at phase change"). The lowest-risk, design-
   consistent fix is to relax the within-phase resize-back assertion at
   `tests/gui/TacticalGuiLiveTest.cpp:1233` from `CPPUNIT_ASSERT_EQUAL(120, …)` to
   "height is preserved within the phase" (e.g. `>= beforeResizeHeight`), keeping
   the existing across-phase-change shrink assertions (which already pass)
   unchanged. The alternative (option b: extend the lifecycle to distinguish
   content-overflow expansion from geometry-driven stacking so the geometry
   component shrinks on widen-back within a phase) is more invasive, would touch
   production code (`ensureLowerPanelLayoutState()`), and is recorded as open
   question O1 for the approver. Recommended: option (a).

## Assumptions

1. The defect fixes are confined to `FBattleDisplay` (lower-panel HUD layout,
   items 1–2) and `FBattleBoard` (board speed-label rendering, item 3). No
   `FTacticalGame` model change, no `FBattleScreen` delegation change, and no
   weapon-stat/ship-class/strategic/asset change is expected. (Labeled assumption;
   each subtask's allowed-files list reflects the expected minimal surface and the
   Implementer confirms during work.)
2. For item 3, the opponent's speed labels should be suppressed during
   `PH_SEEKER_ACTIVATION` to mirror the sprite suppression, and the overlay's
   behavior in all other phases (where both players' labels are intended) must be
   preserved. (Labeled assumption derived from the observed sprite/owner filtering;
   the Implementer confirms the intended per-phase visibility against the live
   render path.)
3. "Top of the bottom panel" means the same top Y used by the instruction text
   (`getActionPromptLineY(0)`), and "right of the left column" means horizontally
   past the wider of the (unwrapped) instruction text and the Done button. Exact
   pixel offsets are an Implementer choice constrained by the no-overlap /
   click-region-alignment acceptance criteria. (Labeled assumption.)
4. The existing offscreen `wxMemoryDC` GUI test approach
   (`tests/gui/TacticalGuiLiveTest.*`, `WXGuiTestHarness`) is sufficient to verify
   the layout no-overlap, panel anchor, and speed-label-suppression behaviors
   behaviorally (drawn-region positions, click-region alignment, absence of the
   opponent's speed text at its hex during activation). (Labeled assumption.)
5. Item 5's fixed-root resolution can reuse the established
   `tests/core` compile-define pattern (a `tests/gui/Makefile` macro such as
   `-DGUI_TEST_REPO_ROOT=\"$(CURDIR)/../..\"`) consumed by both
   `sourceContainsLineToken()` and `readFileText()`, with all source-contract
   call-site paths normalized to repo-root-relative. (Labeled assumption; Tester
   confirms the macro/Makefile mechanism during the work.)

## Likely Files To Modify

1. `src/tactical/FBattleDisplay.cpp` / `include/tactical/FBattleDisplay.h`
   (attack-phase pending-panel layout for SMRV-01; activation-phase panel anchor
   for SMRV-02).
2. `src/tactical/FBattleBoard.cpp` / `include/tactical/FBattleBoard.h`
   (opponent speed-label suppression for SMRV-03).
3. Test-track follow-ups (Tester-owned, not Implementer subtasks):
   `tests/gui/TacticalGuiLiveTest.cpp` (items 4 + 5),
   `tests/gui/BattleSimGuiLiveTest.cpp` (items 5 + 6),
   `tests/gui/StrategicGuiLiveTest.cpp` (item 5),
   `tests/gui/Makefile` (item 5 repo-root macro), and possibly
   `tests/gui/WXGuiTestHarness.cpp/.h` (item 5 shared helper, if a shared resolver
   is preferred).

(Labeled as likely files based on repository evidence; the exact subset per
subtask is given in each Implementer prompt's allowed-files list.)

## Subtasks

### SMRV-01 — Attack-Phase Pending-Seeker Panel: Shift Right Clear Of The Left Instruction Text

Security review: not required.

Reposition the `PH_ATTACK_FIRE` offensive-seeker pending-deployment panel
(`drawOffensiveSeekerPendingRows`, currently drawn at `lMargin = 310`,
`getActionPromptLineY(0)`) so its left edge clears the left column (the unwrapped
attack-phase instruction text and the "Done" button), eliminating the horizontal
overlap with "Select legal path hexes to deploy seeker missiles." Do not wrap the
left instruction text (per the user's explicit preference). Keep the panel
anchored to the top of the bottom panel.

Acceptance Criteria:
- During `PH_ATTACK_FIRE` with offensive-seeker deployment active, the
  pending-deployment panel ("Pending seeker deployments:" and its rows) renders in
  a column whose left edge is to the right of the widest left-column content (the
  longest attack-phase instruction line and the "Done" button), so the panel no
  longer overlaps the left instruction text.
- The left instruction text is unchanged (still rendered as before, not
  wrapped/clipped); the panel moved, not the text.
- Each pending recall row's click region (`m_pendingSeekerRecallRegions` /
  `m_pendingSeekerRecallHexes`) aligns with its new drawn position, so
  `checkOffensiveSeekerPendingSelection()` still recalls the intended pending
  seeker; rows are not clipped or rendered behind the "Done" button or the
  ship-status display.
- A behavioral test demonstrates the no-overlap: it drives `PH_ATTACK_FIRE`
  offensive-seeker deployment, renders to an offscreen `wxMemoryDC`, and asserts
  the pending panel's drawn column does not horizontally intersect the left
  instruction-text region (and the recall row click region matches its draw
  position). This test fails against the current overlapping layout and passes
  after the fix.

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` attack-phase render
  helper(s) (`drawOffensiveSeekerPendingRows` and the `draw()` call site / any
  layout helper changed). No rules-document edit.

### SMRV-02 — Activation-Phase "Activated Seekers" Panel: Anchor To Panel Top

Security review: not required.

Change `drawSeekerActivation()` so the "Activated seekers:" list anchors its top
to `getActionPromptLineY(0)` (top of the bottom panel) in the right/middle column
(`lMargin = 310`), matching the Round 4 pre-game placement convention, instead of
anchoring at `getActionButtonRowBottom()` (below the "Seeker Activation Done"
button).

Acceptance Criteria:
- During `PH_SEEKER_ACTIVATION`, the "Activated seekers:" list renders anchored to
  the top of the bottom panel (top aligned with the instruction-text top region,
  not below the action-button row) and to the right of the left column / button
  area.
- Each activation row's click region (`m_seekerActivationRegions` /
  `m_seekerActivationSeekerIDs`) aligns with its new drawn position, so clicking a
  row still deactivates exactly the intended activated seeker; rows are not clipped
  and do not overlap the left column, the "Seeker Activation Done" button, or the
  ship-status display.
- A behavioral test demonstrates the anchor change: it drives
  `PH_SEEKER_ACTIVATION` with at least one activated seeker, renders to an
  offscreen `wxMemoryDC`, and asserts the "Activated seekers:" block's top is at
  the panel-top prompt region (not below the button row) and that an activation
  row's click region matches its draw position. This test fails against the
  current below-button layout and passes after the fix.

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` activation-phase render
  helper (`drawSeekerActivation`). No rules-document edit.

### SMRV-03 — Suppress Opponent Seeker Speed Labels During The Activation Phase

Security review: not required.

Fix the defect where the opponent's seeker speed-value labels are drawn on the
board during `PH_SEEKER_ACTIVATION` even though the opponent's seeker sprites are
correctly hidden. In `drawSeekerMoveCountOverlay()`, suppress the speed label for
any seeker not owned by the moving player during `PH_SEEKER_ACTIVATION` — applying
the same ownership condition (`ownerID == getMovingPlayerID()`) the sprite render
path uses — while preserving the overlay's behavior in all other `BS_Battle`
phases.

Acceptance Criteria:
- During `PH_SEEKER_ACTIVATION`, no speed-value label is drawn for any seeker not
  owned by the moving player (the opponent's stray floating numbers are gone),
  matching the existing sprite suppression for those seekers.
- The moving player's own seeker speed labels continue to render during activation,
  and the overlay's behavior in all other `BS_Battle` phases is unchanged (both
  players' labels still render where previously rendered).
- A behavioral test demonstrates the suppression: it sets up `PH_SEEKER_ACTIVATION`
  with an active opponent-owned seeker and an active moving-player-owned seeker,
  renders the overlay to an offscreen `wxMemoryDC`, and asserts that the opponent's
  seeker hex has no speed label while the moving player's seeker hex does. This
  test fails against the current unfiltered overlay and passes after the fix.

Documentation Impact:
- Update Doxygen comments for the modified `FBattleBoard` overlay helper
  (`drawSeekerMoveCountOverlay`). No rules-document edit.

## Test-Track Follow-Ups (Tester-Owned; Carried Over From The Round 4 Reviewer)

These three items are confined to the `tests/` tree under their recommended
directions (no `src/` production code) and are therefore Tester-owned, addressed
within the Tester stages of the production subtasks above rather than as separate
Implementer subtasks (mirrors Round 4's Carry-C handling). They are tracked with
identifiers `SMRV-T1`…`SMRV-T3` for coordination clarity only. Tester artifacts
for each item are written under the host subtask's
`artifacts/seeker-missile-fixes-round5/<host-subtask>/` directory.

### SMRV-T1 (item 5) — Harden GUI Source-Contract Path Resolution
Route to: **Tester stage of SMRV-01** (earliest, so later source-contract tests
in SMRV-02/SMRV-03 resolve reliably).
- Replace the CWD-relative source open in `sourceContainsLineToken()`
  (`tests/gui/TacticalGuiLiveTest.cpp:331–346`) and `readFileText()`
  (`tests/gui/BattleSimGuiLiveTest.cpp:80–85`,
  `tests/gui/StrategicGuiLiveTest.cpp:228`) with a fixed repo-root resolution.
  Add a `tests/gui/Makefile` compile-define (e.g.
  `-DGUI_TEST_REPO_ROOT=\"$(CURDIR)/../..\"`, mirroring
  `tests/core/Makefile:19` / `tests/core/WXIconCacheTest.cpp:22`), have both
  helpers prepend it, and normalize all source-contract call-site paths to
  repo-root-relative (drop the `../../` prefix). Optionally consolidate into a
  single shared resolver helper in `tests/gui/WXGuiTestHarness.*`.
- Acceptance: source-contract tests resolve the same source files regardless of
  the CWD `GuiTests` is launched from; a missing/mistyped source path causes a
  visible failure rather than a silently skipped/empty read; the canonical
  `cd tests/gui && make && xvfb-run -a ./GuiTests` run stays green.

### SMRV-T2 (item 4) — Reconcile The Pre-Existing Failing Panel-Height Test
Route to: **Tester stage of SMRV-02** (after the FBattleDisplay layout changes in
SMRV-01/SMRV-02 settle; same `TacticalGuiLiveTest.cpp` surface).
- Recommended direction (D5, option a): relax the within-phase resize-back
  assertion at `tests/gui/TacticalGuiLiveTest.cpp:1233` from
  `CPPUNIT_ASSERT_EQUAL(beforeResizeHeight, restoredHeight)` to a within-phase
  preservation assertion (height `>= beforeResizeHeight`), consistent with the
  accepted SMRIV-04 design (keep expanded height within a phase, recompute only at
  phase change). Keep the across-phase-change shrink assertions unchanged.
- Acceptance: the test passes and now encodes the accepted SMRIV-04 within-phase
  preservation semantics; the across-phase-change shrink behavior remains asserted.
- If the approver chooses option (b) instead (see open question O1), this becomes a
  production lifecycle change in `ensureLowerPanelLayoutState()` and must be
  promoted to its own Implementer subtask with a behavioral test that distinguishes
  content-overflow expansion from geometry-driven stacking expansion.

### SMRV-T3 (item 6) — De-Flake The Live Modal-Chain Test
Route to: **Tester stage of SMRV-03** (independent BattleSim surface).
- In `tests/gui/BattleSimGuiLiveTest.cpp::testLocalGameDialogLaunchesPredefinedAndCustomModalChains`
  (~:446–523), remove the timeout race: make each action's inner
  `waitForModalDialog(...)` timeout comfortably shorter than the enclosing
  `runVoidFunctionWithAction(...)` fallback `timeoutMs`, and raise the second
  chain's fallback above its current 200ms so it cannot tie/beat the inner 200ms
  poll. Optionally replace `pumpEvents(n)` settle calls with bounded
  `waitForTopLevelWindow(...)` / `waitForTopLevelWindowClosed(...)` waits keyed on
  the actual dialog appearing/closing (`tests/gui/WXGuiTestHarness.h:118–266`).
- Acceptance: the test is deterministic across repeated runs (no fallback-vs-poll
  race) and the GUI suite is reliably green.

## Dependency Ordering

Treat this plan as **serial**. Recommended order:

1. **SMRV-01** first — establishes the corrected attack-phase pending-panel layout
   and (via its Tester stage, SMRV-T1) hardens source-contract path resolution so
   the later subtasks' source-contract tests resolve reliably.
2. **SMRV-02** after SMRV-01 — same `FBattleDisplay.cpp/.h` surface (must not edit
   concurrently with SMRV-01). Its Tester stage performs SMRV-T2 once the
   FBattleDisplay layout/height behavior has settled.
3. **SMRV-03** after SMRV-02 — disjoint file (`FBattleBoard.cpp/.h`); sequenced
   last for conservative serial execution. Its Tester stage performs SMRV-T3.

Parallelization note: SMRV-01 and SMRV-02 both edit `FBattleDisplay.cpp/.h` and
share the same layout helpers, so they must run serially with respect to each
other. SMRV-03 edits `FBattleBoard.cpp/.h` (disjoint from FBattleDisplay), so the
FBattleDisplay track (SMRV-01 → SMRV-02), the FBattleBoard track (SMRV-03), and the
test-only track (SMRV-T1…T3) touch separable surfaces and a coordinator *could*
run the FBattleDisplay and FBattleBoard tracks as two parallel lanes; however, this
plan conservatively recommends a single serial sequence and marks **no** subtask
as parallelizable. (SMRV-T2 must follow the FBattleDisplay layout work regardless,
because the layout changes can shift the panel height it reconciles.)

## Overall Acceptance Criteria

1. During `PH_ATTACK_FIRE` offensive-seeker deployment, the pending-deployment
   panel renders to the right of the left column and no longer overlaps the left
   instruction text; the instruction text is unchanged; recall click regions align
   with drawn positions and recall behavior is unchanged.
2. During `PH_SEEKER_ACTIVATION`, the "Activated seekers:" list anchors to the top
   of the bottom panel (not below the button); deactivation click regions align
   with drawn positions and deactivation behavior is unchanged.
3. During `PH_SEEKER_ACTIVATION`, opponent-owned seeker speed labels are suppressed
   (matching the hidden sprites) while the moving player's labels still render, and
   speed-label rendering in all other phases is unchanged.
4. In all repositioned panels, click regions align with drawn positions and rows
   are not clipped or rendered behind buttons, the left column, or the ship-status
   display.
5. New behavioral coverage exists for each defect fix (SMRV-01 no-overlap,
   SMRV-02 panel-top anchor, SMRV-03 opponent-label suppression), each failing
   against the current code and passing after the fix.
6. The pre-existing failing GUI test
   (`testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges`)
   is reconciled with the accepted SMRIV-04 design and passes (SMRV-T2).
7. GUI source-contract tests resolve source files from a fixed repo root regardless
   of launch CWD, and a missing source path fails visibly rather than silently
   (SMRV-T1).
8. The flaky modal-chain test is deterministic and the GUI suite is reliably green
   (SMRV-T3).
9. No change edits `doc/rules/tactical_operations_manual.md`.

## Documentation Impact

Documentation impact is low and concentrated in header Doxygen comments for the
modified `FBattleDisplay` HUD render helpers (`drawOffensiveSeekerPendingRows`,
`drawSeekerActivation`, and any layout helper changed) and the modified
`FBattleBoard` overlay helper (`drawSeekerMoveCountOverlay`). The Documenter stage
may update `doc/UsersGuide.md` and/or `doc/DesignNotes.md` if they describe the
tactical HUD layout or board seeker-speed rendering. The test-track follow-ups
(SMRV-T1…T3) are confined to `tests/` and do not change product documentation.
`doc/rules/tactical_operations_manual.md` must not be edited.

## Open Questions For The Approver

- **O1 (item 4 direction).** This plan recommends option (a): relax the failing
  within-phase resize-back assertion to match the accepted SMRIV-04 design
  (Tester-owned, no production change). If the approver instead wants within-phase
  geometry-driven shrink-back (option b), that requires a production change to
  `ensureLowerPanelLayoutState()` to distinguish content-overflow expansion from
  geometry-driven stacking expansion, and SMRV-T2 must be promoted to a standalone
  Implementer subtask with its own behavioral test. Default if no response:
  option (a).
- **O2 (decomposition of items 4–6).** This plan treats items 4–6 as Tester-owned
  follow-ups routed to subtask Tester stages (no Implementer prompts), per the
  planner contract and the Round 4 precedent. If the approver prefers them tracked
  as standalone numbered subtasks, SMRV-T1/T3 (test-infrastructure + test-timing)
  could be packaged as a dedicated Tester-focused work item; note this would be
  test-only work with no `src/` allowed files.
- **O3 (item 1 target margin).** The exact left edge for the shifted pending panel
  is left to the Implementer (constrained by the no-overlap criteria). If the
  approver wants a specific fixed margin or a measured-text-extent computation,
  state it; otherwise the Implementer chooses the smallest margin that clears the
  longest attack-phase instruction line and the Done button.

## Implementer Prompts

### SMRV-01 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Reposition the attack/offensive-fire phase (`PH_ATTACK_FIRE`) offensive-seeker
pending-deployment panel in the lower panel so it no longer overlaps the
left-column instruction text. Today `draw()` (in `src/tactical/FBattleDisplay.cpp`,
~line 713) calls `drawOffensiveSeekerPendingRows(dc, 310, getActionPromptLineY(0),
10)`, drawing "Pending seeker deployments:" and its rows starting at
`lMargin = 310`. The left-column attack-phase instruction text drawn by
`drawAttackFire()` (~lines 1220–1234) — in particular "Select legal path hexes to
deploy seeker missiles." at `x = leftOffset = 40` — is rendered unwrapped and
unbounded, so it overruns past `x = 310` and collides horizontally with the
pending panel. Per the user's explicit preference, do NOT wrap the left
instruction text; instead shift/resize the pending-deployment panel so its left
edge is clear to the right of the left column (the widest attack-phase instruction
line and the "Done" button), keeping the panel anchored to the top of the bottom
panel (`getActionPromptLineY(0)`). Recompute each pending recall row's click
region (`m_pendingSeekerRecallRegions` / `m_pendingSeekerRecallHexes`) from the new
draw position so `checkOffensiveSeekerPendingSelection()` still recalls the
intended pending seeker, and ensure the rows are not clipped or rendered behind the
"Done" button or the ship-status display. Choose the smallest panel `lMargin` that
clears the longest instruction line and the Done button (a fixed margin or a
margin computed from the measured text extent is acceptable). Do not change the
underlying pending-seeker model or recall logic beyond repositioning. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `PH_ATTACK_FIRE` with offensive-seeker deployment active, the pending
  panel ("Pending seeker deployments:" and its rows) renders in a column whose left
  edge is to the right of the widest left-column content, so it no longer overlaps
  the left instruction text; the panel stays anchored to the top of the bottom
  panel.
- The left instruction text is unchanged (not wrapped/clipped); only the panel
  moved.
- Each pending recall row's click region aligns with its new drawn position; rows
  are not clipped or rendered behind the "Done" button or ship-status display, and
  recall behavior is preserved.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live layout / no-overlap / click-region behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` plus the
  `WXGuiTestHarness` helpers); source-contract coverage for the layout helper
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`. The Tester
  stage of this subtask also performs carried-over item SMRV-T1 (harden the GUI
  source-contract path resolution: add a `tests/gui/Makefile` repo-root compile
  macro mirroring `tests/core/Makefile:19`/`tests/core/WXIconCacheTest.cpp:22`,
  make `sourceContainsLineToken()` and `readFileText()` resolve from that fixed
  root, and normalize source-contract call-site paths to repo-root-relative).

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round5/SMRV-01/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRV-02 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Reposition the seeker-activation phase (`PH_SEEKER_ACTIVATION`) "Activated seekers:"
list in the lower panel. Today `drawSeekerActivation()` (in
`src/tactical/FBattleDisplay.cpp`, ~lines 1670–1737) sets `lMargin = 310` but
anchors the list at `int y = getActionButtonRowBottom();` (~line 1677), drawing
"Activated seekers:" below the "Seeker Activation Done" button row. Change the
anchor so the list's top is `getActionPromptLineY(0)` (the top of the bottom panel,
the same top region as the instruction text), in the right/middle column at
`lMargin = 310`, matching the Round 4 pre-game placement convention
(`drawPlaceMines`/`drawPlaceSeekers`). Recompute each activation row's click region
(`m_seekerActivationRegions` / `m_seekerActivationSeekerIDs`) from the new draw
position so clicking a row still deactivates exactly the intended activated seeker,
and ensure the rows are not clipped and do not overlap the left column, the "Seeker
Activation Done" button, or the ship-status display. Do not change the underlying
activation/deactivation model logic beyond repositioning. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `PH_SEEKER_ACTIVATION`, the "Activated seekers:" list renders anchored to
  the top of the bottom panel (top aligned with the instruction-text top region,
  not below the action-button row) and to the right of the left column / button
  area.
- Each activation row's click region aligns with its new drawn position; clicking a
  row still deactivates the intended seeker; rows are not clipped and do not overlap
  the left column, the "Seeker Activation Done" button, or the ship-status display.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live layout / anchor / click-region behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` plus the
  `WXGuiTestHarness` helpers); source-contract coverage for the layout helper
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`. The Tester
  stage of this subtask also performs carried-over item SMRV-T2 (reconcile the
  pre-existing failing test
  `tests/gui/TacticalGuiLiveTest.cpp::testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges`
  at ~line 1233 by relaxing the within-phase resize-back assertion to a
  within-phase preservation assertion, consistent with the accepted SMRIV-04
  design; keep the across-phase-change shrink assertions unchanged).

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round5/SMRV-02/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMRV-03 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleBoard.h`

Task to implement:
Fix the defect where the opponent's seeker speed-value labels are drawn on the
board during the seeker-activation phase even though the opponent's seeker sprites
are correctly hidden. In `src/tactical/FBattleBoard.cpp`, the sprite render path
`drawSeekerMissiles()` (~line 475) draws only the moving player's seekers during
`PH_SEEKER_ACTIVATION` (it sources from `getActiveSeekersByMovingPlayer()` and
`getInactiveSeekerActivationHexes()`, both filtered on
`ownerID == getMovingPlayerID()`). But the speed-label overlay
`drawSeekerMoveCountOverlay()` (~lines 575–635), called unconditionally for
`BS_Battle` (~line 117), iterates the full unfiltered `getSeekerMissiles()` list
(~line 581) and skips only on `!itr->active` (~line 594) — with no ownership check
— so the opponent's active seekers still get their red speed numbers drawn. Add an
ownership guard so that during `PH_SEEKER_ACTIVATION` the overlay skips any seeker
whose `ownerID != getMovingPlayerID()` (equivalently, source the activation-phase
labels from `getActiveSeekersByMovingPlayer()`), matching the sprite suppression.
Preserve the overlay's behavior in all other `BS_Battle` phases (both players'
labels must still render where they do today); the guard must be scoped to the
activation phase only. Do not edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `PH_SEEKER_ACTIVATION`, no speed-value label is drawn for any seeker not
  owned by the moving player (the opponent's stray floating numbers are gone),
  matching the existing hidden-sprite behavior.
- The moving player's own seeker speed labels still render during activation, and
  the overlay's behavior in all other `BS_Battle` phases is unchanged.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live render / speed-label-suppression behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` asserting the absence
  of the opponent's speed text at its hex during activation and the presence of the
  moving player's); source-contract coverage for the overlay helper belongs in
  `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`. The Tester stage of
  this subtask also performs carried-over item SMRV-T3 (de-flake
  `tests/gui/BattleSimGuiLiveTest.cpp::testLocalGameDialogLaunchesPredefinedAndCustomModalChains`
  by removing the fallback-vs-poll timeout race: make each inner
  `waitForModalDialog(...)` timeout shorter than the enclosing
  `runVoidFunctionWithAction(...)` fallback, and raise the second chain's fallback
  above 200ms; optionally replace `pumpEvents(n)` settle calls with bounded
  `waitForTopLevelWindow(...)` / `waitForTopLevelWindowClosed(...)` waits).

Artifact guidance:
Write implementation artifacts under
`artifacts/seeker-missile-fixes-round5/SMRV-03/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.
