# Seeker Missile Fixes Round 3 Plan

## Output Artifact Path

`plans/seeker-missile-fixes-round3-plan.md`

Coordinator artifact root for this plan: `artifacts/seeker-missile-fixes-round3/`
with per-subtask subdirectories `SMFR-01` … `SMFR-05`.

## Feature Restatement

Resolve six observed defects in the tactical seeker-missile and mine systems so
seekers and mines work end-to-end through pre-game setup, attack-phase
deployment, seeker activation, autonomous seeker movement, and damage
resolution:

1. The damage summary dialog does not display after a ship passes through a
   minefield. It must display whether or not damage was taken, and triggered
   minefield hexes must be highlighted on the board.
2. The seeker speed (move-count) label must stay visible at all times once a
   seeker is activated, not only after it has moved.
3. The seeker movement path must be visible before/during ICM selection and the
   damage summary. Impacting seekers' paths clear when the damage summary
   closes; non-impacting seekers' paths remain until the moving player's ship
   movement paths clear.
4. During attack-phase mine/seeker placement, the placed-ordnance removal list
   widget does not appear in a visible location.
5. During the seeker activation phase, the seeker deactivation list widget does
   not appear in a visible location.
6. During pre-game setup, mine and seeker placement no longer records anything
   (ammo unchanged, no hex recorded, no board highlight) — a regression from
   earlier working behavior.

## Confirmed Repository Facts

1. The tactical model owns placement, movement, damage, and phase state in
   `src/tactical/FTacticalGame.cpp` / `include/tactical/FTacticalGame.h`;
   `FBattleScreen` forwards model APIs and `FBattleBoard` / `FBattleDisplay`
   render wx output and handle input.
2. `drawOffensiveSeekerPendingRows()` (`src/tactical/FBattleDisplay.cpp`
   ~lines 1590-1631) auto-expands the lower-panel height when its rows overflow
   (`requestedDisplayHeight` / `applyRequestedDisplayHeight()`), but
   `drawPlaceMines()` (~1382), `drawPlaceSeekers()` (~1452), and
   `drawSeekerActivation()` (~1528) do **not** have that auto-expansion; they
   render rows at a fixed `lMargin` (~x=310) starting at
   `getActionButtonRowBottom()` and can be clipped below the ship-status region.
3. The board draw dispatch renders placed ordnance during `BS_PlaceMines` only
   (`src/tactical/FBattleBoard.cpp` ~line 118 calling
   `drawPlacementOrdnanceHexes()`); there is **no `BS_PlaceSeekers` branch**, so
   placed seekers are not drawn during the seeker placement phase.
4. `drawPlacementOrdnanceHexes()` (`src/tactical/FBattleBoard.cpp` ~line 627)
   reads `getPlacedOrdnance()` and shades each placed-ordnance hex; an empty
   `m_placedOrdnance` yields no highlight.
5. Board clicks route through `FBattleBoard::onLeftUp()`
   (`src/tactical/FBattleBoard.cpp` ~line 186) to
   `FBattleScreen::handleHexClick()` (~line 587) to
   `FTacticalGame::handleHexClick()`, which redraws on change.
6. `FTacticalGame::beginOrdnancePlacement()` auto-selects a deployable source
   before entering `BS_PlaceMines` (`src/tactical/FTacticalGame.cpp`
   ~lines 2881-2911); `FMinelayer` initializes its mine launcher to 20 ammo and
   seeker launcher to 4 ammo at construction (`src/ships/FMinelayer.cpp`
   lines 34-42), so deployable sources have ammo at placement time.
7. The pre-game placement coverage in
   `tests/tactical/FTacticalGameMechanicsTest.cpp` uses source-text
   `assertContains(...)` checks (for example lines ~592-595), not behavioral
   placement assertions. No existing test constructs a game, enters placement,
   clicks a hex, and asserts ammo decrement, `m_minedHexList` insertion, or
   `m_placedOrdnance` recording. A runtime placement regression therefore passes
   the current suite undetected.
8. Seeker movement paths are drawn by `drawSeekerPaths()`
   (`src/tactical/FBattleBoard.cpp` ~line 517) during `PH_MOVE` and
   `PH_SEEKER_ACTIVATION`; the speed/move-count label is drawn by
   `drawSeekerMoveCountOverlay()` (~line 554). `movementPath` is populated by
   `resolveActiveSeekersForMovingPlayer()` (`src/tactical/FTacticalGame.cpp`
   ~lines 1127-1210) and cleared for non-moving seekers (~line 1148).
9. Impacting seekers are retained in `m_seekerMissiles` (with `movementPath`)
   until `applyMovementSeekerDamage()` erases them after the damage dialog
   (`src/tactical/FTacticalGame.cpp` ~line 3006). Non-contact seekers keep their
   `movementPath` into the next resolution pass rather than clearing at end of
   the current move phase.
10. `completeMovePhase()` (`src/tactical/FTacticalGame.cpp` ~line 3076) is the
    post-move resolution seam; it sets `m_drawRoute = false` and is where ship
    routes clear. `applyMineDamage()` (~line 3123) builds the mine damage report
    and calls the damage-summary UI path.
11. `doc/rules/tactical_operations_manual.md` is a protected external artifact
    and must not be edited.

## Resolved Design Decisions

1. **(D1) Minefield damage summary** — Display the damage summary dialog
   immediately after a ship's movement resolves through a minefield, for all
   passages including zero-damage outcomes. Highlight the triggered minefield
   hex(es) on the board using the same shaded-hex fill pattern used for movement
   and placement, in the green color used for pre-game mine placement. The
   highlight persists while the summary is shown and clears when the dialog
   closes.
2. **(D2) Seeker speed label** — The label is already rendered in the correct
   place; the only change is persistence: keep it visible continuously for every
   active seeker from activation until the seeker impacts a target or exhausts
   its movement allowance.
3. **(D3) Seeker movement path visibility** — The path is visible from seeker
   resolution onward, persists through ICM selection and the damage summary
   dialog, and (for impacting seekers) clears when the damage summary closes.
4. **(D3b) Non-impacting seeker path persistence** — A non-impacting seeker's
   displayed path remains drawn until the moving player's ship movement phase
   completes (`completeMovePhase`), clearing together with ship routes. The
   seeker's movement-allowance/turn bookkeeping must be preserved; only the
   displayed path is cleared.
5. **(D4) Mine placement removal widget** — The widget already exists; the fix
   is visibility/layout. The placed-ordnance removal list must render in the
   visible lower-panel middle region (right of the phase-ending button widget,
   left of the ship-status display) instead of being clipped below ship-status.
6. **(D5) Seeker deactivation widget** — Same pattern as D4: the activation/
   deactivation list widget exists but is clipped; restore its visibility in the
   correct region while preserving intended behavior.
7. **(D6) Pre-game placement regression** — Investigate and fix the root cause
   in the current codebase (no full revert). The model placement math is correct
   in isolation and a source is auto-selected on entry, so the failure is in
   runtime state/sequencing or rendering. The fix must be validated by a new
   behavioral test (not a source-text match) and must include the missing
   `BS_PlaceSeekers` board render branch.

## Assumptions

1. The defect set is confined to the tactical module surfaces
   (`FTacticalGame`, `FBattleScreen`, `FBattleDisplay`, `FBattleBoard`) plus the
   tactical damage-summary GUI seam already used for mine/seeker damage. No
   strategic-layer or weapon-stat changes are required.
2. The green pre-game mine placement color and the existing shaded-hex helper
   (`drawShadedHex` / `drawPlacementOrdnanceHexes` color path in
   `src/tactical/FBattleBoard.cpp`) can be reused for the triggered-minefield
   highlight; no new asset is required. (Labeled assumption — the implementer
   confirms the exact reusable color/helper during implementation.)
3. The mine damage report already produced by `applyMineDamage()` carries enough
   information (target ships, damage, triggered hexes) to drive the summary
   dialog and hex highlight; if triggered-hex data is not currently retained for
   the UI, exposing it is part of SMFR-03.
4. Behavioral placement tests can be added at the model level
   (`FTacticalGame`) without a wx runtime, consistent with existing model test
   seams.

## Likely Files To Modify

1. `src/tactical/FTacticalGame.cpp`
2. `include/tactical/FTacticalGame.h`
3. `src/tactical/FBattleScreen.cpp`
4. `include/tactical/FBattleScreen.h`
5. `src/tactical/FBattleDisplay.cpp`
6. `include/tactical/FBattleDisplay.h`
7. `src/tactical/FBattleBoard.cpp`
8. `include/tactical/FBattleBoard.h`

(Labeled as likely files based on repository evidence; exact subset per subtask
is given in each Implementer prompt's allowed-files list.)

## Subtasks

### SMFR-01 - Fix Lower-Panel List-Widget Visibility (Placement Removal And Seeker Deactivation)

Make the placed-ordnance removal list (mine/seeker placement phases) and the
seeker activation/deactivation list render in the visible lower-panel region
instead of being clipped below the ship-status display.

Acceptance Criteria:
- `drawPlaceMines()`, `drawPlaceSeekers()`, and `drawSeekerActivation()` reserve
  vertical space so their rendered rows are fully visible, using the same
  panel-height auto-expansion approach already used by
  `drawOffensiveSeekerPendingRows()` (update `requestedDisplayHeight` and call
  `applyRequestedDisplayHeight()` when rendered rows would extend past the
  current requested height).
- During attack-phase and pre-game mine/seeker placement, the placed-ordnance
  removal list is visible in the lower-panel middle region (right of the
  phase-ending button widget, left of the ship-status display) and its rows are
  clickable to remove/undo a placement.
- During the seeker activation phase, the seeker activation/deactivation list is
  visible in the same region and its rows are clickable.
- The lists no longer render below or behind the ship-status display.
- Existing lower-panel controls (phase-ending buttons, ship-status display,
  offensive-fire pending list) keep their current positions and behavior.

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` rendering/layout
  helpers.
- No rules-document edit.

### SMFR-02 - Root-Cause And Fix Pre-Game Mine/Seeker Placement Regression

Investigate why pre-game mine and seeker placement records nothing at runtime
(ammo unchanged, no hex recorded, no board highlight), fix the root cause, add
the missing `BS_PlaceSeekers` board render branch, and lock the fix with a
behavioral regression test.

Acceptance Criteria:
- During pre-game `BS_PlaceMines`, clicking a valid hex records a mine: the
  selected source's mine launcher ammo decrements by one, the hex is added to
  `m_minedHexList`, and a placed-ordnance record is appended to
  `m_placedOrdnance`.
- During pre-game `BS_PlaceSeekers`, clicking a valid hex records an inactive
  seeker: the selected source's seeker launcher ammo decrements by one and an
  inactive seeker record plus placed-ordnance record are created.
- Placed mines and inactive seekers are highlighted on the board during their
  respective placement phases; the board draw dispatch renders placed ordnance
  during `BS_PlaceSeekers` as well as `BS_PlaceMines`.
- After completing pre-game placement and entering battle, an enemy ship moving
  through a recorded mined hex triggers mine damage resolution.
- The recorded ammo state is consistent between placement and battle (a placed
  mine/seeker is no longer present in the launcher's available ammo in battle).
- A new behavioral (not source-text-match) test constructs a tactical game,
  enters mine and seeker placement, places at least one of each, and asserts
  ammo decrement, hex recording, and placed-ordnance recording; the test fails
  against the current regression and passes after the fix.

Documentation Impact:
- Update Doxygen comments for any changed placement state/flow methods and the
  board render dispatch.
- No rules-document edit.

### SMFR-03 - Minefield Damage Summary Display And Triggered-Hex Highlight

Ensure the damage summary dialog displays after any minefield passage (including
zero damage) and highlight the triggered minefield hex(es) on the board.

Acceptance Criteria:
- After a ship's movement resolves through a minefield, the damage summary
  dialog is shown via the existing tactical damage-summary UI seam, including
  the case where no damage was dealt (the dialog still reports that the
  minefield was triggered).
- The triggered minefield hex(es) are highlighted on the board using the
  existing shaded-hex fill pattern in the green color used for pre-game mine
  placement.
- The triggered-hex highlight is visible while the damage summary dialog is
  shown and is cleared after the dialog closes.
- Existing mine damage application (ICM allocation, destroyed-ship cleanup
  ordering) continues to work and runs exactly once per movement completion.
- Seeker detonation damage reporting is unaffected by this change.

Documentation Impact:
- Update Doxygen comments for the mine damage summary/highlight helpers on
  `FTacticalGame` / `FBattleBoard` / `FBattleScreen`.
- User-facing docs may later describe the minefield damage summary; no
  rules-document edit.

### SMFR-04 - Persist Seeker Speed Label From Activation To Impact Or Exhaustion

Keep the seeker speed (move-count) label continuously visible for every active
seeker from activation until it impacts a target or exhausts its movement
allowance.

Acceptance Criteria:
- The speed/move-count label is rendered for every active seeker whenever active
  seekers are shown on the board, not only after a seeker has moved this phase.
- The label remains visible from the moment a seeker becomes active through its
  subsequent turns until the seeker detonates or is removed at movement
  exhaustion.
- The label continues to render correctly for multiple co-located seekers
  (stacked display preserved).
- No change to seeker movement, targeting, or damage behavior.

Documentation Impact:
- Update Doxygen comments for the seeker label rendering helper if its
  visibility conditions change.
- No rules-document edit.

### SMFR-05 - Seeker Movement-Path Persistence Lifecycle

Make seeker movement paths persist correctly through resolution, ICM selection,
and the damage summary, and clear at the right time depending on whether the
seeker impacted a target.

Acceptance Criteria:
- A seeker's movement path is visible from the moment its movement is resolved,
  through ICM selection, and while the damage summary dialog is displayed.
- An impacting seeker's path clears when the damage summary dialog closes (the
  seeker and its path are removed together, consistent with the existing
  post-damage removal seam).
- A non-impacting seeker's path remains drawn until the moving player's ship
  movement phase completes (`completeMovePhase`), clearing together with ship
  routes.
- Clearing a non-impacting seeker's displayed path does not discard the seeker's
  movement-allowance/turn bookkeeping needed for its next-turn movement.
- Seeker path rendering continues to work for multiple simultaneous seekers and
  during `PH_SEEKER_ACTIVATION` and `PH_MOVE`.

Documentation Impact:
- Update Doxygen comments for the seeker path population/clearing helpers on
  `FTacticalGame` and the path rendering helper on `FBattleBoard`.
- No rules-document edit.

## Dependency Ordering

1. `SMFR-01` and `SMFR-02` are the foundation and both touch placement UI/flow
   and shared files (`FBattleDisplay`, `FBattleBoard`, `FTacticalGame`). Run
   `SMFR-01` first, then `SMFR-02`. They are not parallelizable because they
   overlap in the placement rendering/flow surfaces and `SMFR-02`'s board
   render-branch work depends on a stable lower-panel layout from `SMFR-01`.
2. `SMFR-03` depends on `SMFR-02` because reliable mine recording/highlight is a
   prerequisite for a meaningful minefield damage summary and triggered-hex
   highlight, and both share board highlight/render code.
3. `SMFR-04` depends on `SMFR-02` (stable seeker placement/rendering) and shares
   seeker-render code with `SMFR-05`; run `SMFR-04` before `SMFR-05`.
4. `SMFR-05` depends on `SMFR-04` because both modify seeker board rendering and
   path/label visibility; running them sequentially avoids overlapping edits to
   the same rendering helpers.

Parallelization note: Treat this plan as serial. Every subtask touches one or
more of `FTacticalGame`, `FBattleDisplay`, and `FBattleBoard`, and several share
the same rendering helpers and phase-flow seams, so conservative sequential
execution is required.

## Overall Acceptance Criteria

1. Pre-game mine and seeker placement records placements (ammo decrement, hex
   recording, placed-ordnance recording) and highlights placed ordnance on the
   board for both `BS_PlaceMines` and `BS_PlaceSeekers`.
2. A behavioral test proves pre-game placement recording and fails against the
   current regression.
3. The placed-ordnance removal list and the seeker activation/deactivation list
   render in the visible lower-panel middle region and are clickable.
4. A minefield passage shows the damage summary dialog (including zero-damage)
   and highlights triggered minefield hexes in the pre-game-placement green,
   clearing the highlight when the dialog closes.
5. The seeker speed label stays visible for every active seeker from activation
   until impact or movement exhaustion.
6. Seeker movement paths persist through ICM selection and the damage summary;
   impacting seekers' paths clear on dialog close; non-impacting seekers' paths
   clear when the moving player's ship movement phase completes, without losing
   seeker movement bookkeeping.
7. Existing mine damage, seeker detonation damage, ICM allocation, and
   destroyed-ship cleanup continue to work and run exactly once per resolution.
8. No change edits `doc/rules/tactical_operations_manual.md`.

## Documentation Impact

Overall documentation impact is low-to-moderate and concentrated in header
Doxygen comments for the modified tactical rendering, layout, placement-flow,
mine-damage-summary, and seeker-path helpers. The Documenter stage may update
`doc/UsersGuide.md` to reflect that minefield passages show a damage summary
with highlighted triggered hexes and that placement-removal and
seeker-deactivation lists are available during their phases. `doc/DesignNotes.md`
may be updated if it tracks tactical UI behavior. `doc/rules/tactical_operations_manual.md`
must not be edited.

## Implementer Prompts

### SMFR-01 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

Task to implement:
Fix the lower-panel list-widget visibility so the placed-ordnance removal list
(mine and seeker placement phases) and the seeker activation/deactivation list
render in the visible lower-panel middle region instead of being clipped below
the ship-status display. Apply the same panel-height auto-expansion approach
already used by `drawOffensiveSeekerPendingRows()` (update
`requestedDisplayHeight` and call `applyRequestedDisplayHeight()` when rendered
rows would extend past the current requested height) to `drawPlaceMines()`,
`drawPlaceSeekers()`, and `drawSeekerActivation()`. Do not change placement,
activation, or seeker model behavior.

Acceptance criteria:
- `drawPlaceMines()`, `drawPlaceSeekers()`, and `drawSeekerActivation()` reserve vertical space so their rendered rows are fully visible, mirroring the auto-expansion in `drawOffensiveSeekerPendingRows()`.
- The placed-ordnance removal list is visible (right of the phase-ending button widget, left of the ship-status display) and its rows remain clickable during mine and seeker placement.
- The seeker activation/deactivation list is visible in the same region and its rows remain clickable during the seeker activation phase.
- The lists no longer render below or behind the ship-status display.
- Existing phase-ending buttons, ship-status display, and offensive-fire pending list keep their current positions and behavior.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Source-contract coverage for the layout helpers belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*` or `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`.
- Live placement/activation visibility behavior can be covered in `tests/gui/TacticalGuiLiveTest.*` if needed.

Artifact guidance:
Write implementation artifacts under `artifacts/seeker-missile-fixes-round3/SMFR-01/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMFR-02 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`

Task to implement:
Root-cause and fix the pre-game mine/seeker placement regression. At runtime,
clicking a hex during pre-game `BS_PlaceMines` / `BS_PlaceSeekers` records
nothing: the launcher ammo does not decrement, no hex is recorded, and no board
highlight appears. The model placement math is correct in isolation and
`beginOrdnancePlacement()` auto-selects a deployable source, so investigate the
runtime state/sequencing (which placement state is actually entered, for which
active player, with which ship/weapon selected, and whether the board click
reaches the recording path) and fix the root cause. Add the missing
`BS_PlaceSeekers` branch to the board draw dispatch so placed seekers are drawn
during seeker placement (mirroring the existing `BS_PlaceMines` branch). Do not
edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During pre-game `BS_PlaceMines`, clicking a valid hex decrements the selected source's mine launcher ammo by one, inserts the hex into `m_minedHexList`, and appends a record to `m_placedOrdnance`.
- During pre-game `BS_PlaceSeekers`, clicking a valid hex decrements the selected source's seeker launcher ammo by one and creates an inactive seeker record plus a placed-ordnance record.
- The board renders placed ordnance during `BS_PlaceSeekers` as well as `BS_PlaceMines`, so placed mines and inactive seekers are highlighted during their respective placement phases.
- After completing pre-game placement and entering battle, an enemy ship moving through a recorded mined hex triggers mine damage resolution.
- Launcher ammo state is consistent between placement and battle (a placed mine/seeker is not still counted as available in battle).
- Add a behavioral (not source-text-match) model test that constructs a tactical game, enters mine and seeker placement, places at least one of each, and asserts ammo decrement, hex recording, and placed-ordnance recording; this test must fail against the current regression and pass after the fix.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Behavioral placement model coverage belongs in `tests/tactical/FTacticalGameMechanicsTest.*` or a focused new tactical placement fixture under `tests/tactical/`.
- Board render-dispatch source-contract coverage belongs in `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/seeker-missile-fixes-round3/SMFR-02/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMFR-03 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`

Task to implement:
Ensure the damage summary dialog displays after any minefield passage, including
zero-damage outcomes, and highlight the triggered minefield hex(es) on the board
using the existing shaded-hex fill pattern in the green color used for pre-game
mine placement. The highlight must be visible while the dialog is shown and
cleared after it closes. Reuse the existing mine damage application path
(`applyMineDamage()` / the tactical damage-summary UI seam); if triggered-hex
data is not currently retained for the UI, expose the minimum read-only state
needed to drive the highlight. Do not change seeker detonation damage behavior
and do not edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- After a ship's movement resolves through a minefield, the damage summary dialog is shown through the existing tactical damage-summary UI seam, including when no damage was dealt.
- Triggered minefield hex(es) are highlighted on the board using the existing shaded-hex pattern in the pre-game-placement green.
- The highlight is visible while the dialog is shown and is cleared after the dialog closes.
- Existing mine damage application (ICM allocation, destroyed-ship cleanup ordering) still runs exactly once per movement completion.
- Seeker detonation damage reporting is unchanged.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Mine damage report/summary model coverage belongs in `tests/tactical/FTacticalMineDamageReportSummaryTest.*` or `tests/tactical/FTacticalMineDamageFlowTest.*`.
- Triggered-hex highlight render-dispatch coverage belongs in `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/seeker-missile-fixes-round3/SMFR-03/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMFR-04 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`

Task to implement:
Keep the seeker speed (move-count) label continuously visible for every active
seeker from activation until it impacts a target or exhausts its movement
allowance, rather than only after the seeker has moved this phase. Adjust the
label rendering visibility conditions in `drawSeekerMoveCountOverlay()` (and any
directly related guard) accordingly. Do not change seeker movement, targeting,
or damage behavior.

Acceptance criteria:
- The speed/move-count label renders for every active seeker whenever active seekers are shown on the board, not only after movement this phase.
- The label remains visible from activation through subsequent turns until the seeker detonates or is removed at movement exhaustion.
- The label still renders correctly for multiple co-located seekers (stacked display preserved).
- No change to seeker movement, targeting, or damage behavior.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Seeker label render-dispatch coverage belongs in `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*` or `tests/tactical/FTacticalSeekerMovementTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/seeker-missile-fixes-round3/SMFR-04/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### SMFR-05 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`

Task to implement:
Correct the seeker movement-path persistence lifecycle. Paths must be visible
from seeker resolution through ICM selection and the damage summary. An impacting
seeker's path clears when the damage summary closes (consistent with the
existing post-damage removal seam in `applyMovementSeekerDamage()`). A
non-impacting seeker's displayed path must remain drawn until the moving player's
ship movement phase completes (`completeMovePhase`), clearing together with ship
routes — without discarding the seeker's movement-allowance/turn bookkeeping
needed for next-turn movement. Separate the displayed-path state from the seeker
movement bookkeeping where necessary so clearing the path does not corrupt
movement state. Do not edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- A seeker's movement path is visible from movement resolution, through ICM selection, and while the damage summary dialog is displayed.
- An impacting seeker's path clears when the damage summary dialog closes.
- A non-impacting seeker's path remains drawn until `completeMovePhase` clears the moving player's ship movement paths, then clears with them.
- Clearing a non-impacting seeker's displayed path preserves its movement-allowance/turn bookkeeping for next-turn movement.
- Seeker path rendering still works for multiple simultaneous seekers during `PH_SEEKER_ACTIVATION` and `PH_MOVE`.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Seeker path population/clearing model coverage belongs in `tests/tactical/FTacticalSeekerMovementTest.*`.
- Path render-dispatch coverage belongs in `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/seeker-missile-fixes-round3/SMFR-05/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.
