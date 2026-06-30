# Seeker Missile UI Cleanup Plan

## Feature Restatement

Five follow-up cleanups to the recently completed tactical seeker-missile feature, observed while
playing the game. All changes live in the tactical UI/model layer (`FTacticalGame`, `FBattleScreen`,
`FBattleDisplay`, `FBattleBoard`). No rules behavior changes; this is interaction/rendering polish plus
the supporting model state required to draw it.

1. **Seeker activation screen rework** — clicking a seeker stack on the board activates one seeker from
   that hex; the lower status window lists *all* currently-activated seekers (any hex) and lets the
   player deactivate each one, returning it to inactive and back onto the board. This requires a new
   model-side deactivation path (today activation is one-way).
2. **Placement "Done" button label** — replace the fixed "Mine Placement Done" label with a label derived
   from the weapon types being placed: "Seeker Placement Done" (seekers only), "Mine Placement Done"
   (mines only), "Weapon Placement Done" (both).
3. **Firing-phase deployed-seeker visibility** — seekers deployed during the moving player's offensive
   fire phase are not drawn on the board, so the player cannot see or reconsider them. Render the seeker
   icon on each pending-deployment hex. (The recall list already exists and is rendered.)
4. **Pre-game placement panel overlap** — fix the overlapping regions in the `BS_PlaceMines` lower panel
   (instructions + "Done" button region overlapping the placement source list).
5. **Seeker movement path + rotation** — when active seekers move at the start of the movement phase,
   draw each seeker's actual stepped path on the main board and rotate its icon by heading, mirroring
   ship rendering. This requires capturing the per-move path (only the final hex/heading is kept today).

## Confirmed Repository Facts

1. `doc/rules/tactical_operations_manual.md` is a protected external artifact and must not be edited.
2. Seeker model state lives in `FTacticalGame` as `std::vector<FTacticalSeekerMissileState> m_seekerMissiles`
   (`include/tactical/FTacticalGame.h:1209`). Each record carries `seekerID, ownerID, hex, heading, active,
   movementTurn, movementAllowance, hasSource, source` (`FTacticalGame.h:137-147`).
3. Pending offensive-fire deployments are tracked separately in
   `std::vector<FTacticalPendingSeekerDeployment> m_pendingOffensiveSeekerDeployments`
   (`FTacticalGame.h:1212`), keyed by `m_offensiveFirePhaseID` (`FTacticalGame.h:1211`). Pending seekers
   are created with `active=false` (`FTacticalGame.cpp:1641`).
4. Activation phase (`PH_SEEKER_ACTIVATION`): board click calls `selectSeekerActivationHex(hex)`
   (`FBattleBoard.cpp:186-187`); the lower panel `drawSeekerActivation` (`FBattleDisplay.cpp:1441-1500`)
   lists `getSelectedInactiveSeekerActivationStack()` rows; a row click calls
   `activateSelectedInactiveSeeker(seekerID)` via `checkSeekerActivationSelection` (`FBattleDisplay.cpp:1557-1569`).
   `checkSeekerActivationSelection` does **not** call `m_parent->reDraw()` (compare `checkShipSelection:1554`).
5. `activateSelectedInactiveSeeker` (`FTacticalGame.cpp:968-986`) sets `active=true` and is explicitly
   one-way; there is no deactivation method.
6. Board seeker rendering `drawSeekerMissiles` (`FBattleBoard.cpp:445-471`): during `PH_SEEKER_ACTIVATION`
   it draws only **inactive** seeker hexes (`getInactiveSeekerActivationHexes`); in all other battle phases
   it draws only **active** seekers (`getSeekerMissilesAtHex(hex, true)`). Pending (inactive) firing-phase
   seekers are therefore invisible.
7. The placement "Done" button is `m_buttonMinePlacementDone` (`FBattleDisplay.h:118`, created with label
   "Mine Placement Done" at `FBattleDisplay.cpp:79`), handler `onMinePlacementDone` (`FBattleDisplay.h:186`,
   `FBattleDisplay.cpp:1350-1357`). The placement source list `getDeployablePlacementSources()` mixes
   `FWeapon::M` and `FWeapon::SM`; helper `getDeploymentWeaponLabel(weaponType)` exists
   (`FBattleDisplay.cpp:36-45`). `drawPlaceMines` is at `FBattleDisplay.cpp:1371-1439`.
8. The offensive-fire recall list `drawOffensiveSeekerPendingRows` (`FBattleDisplay.cpp:1502-1538`) is
   already invoked during `PH_ATTACK_FIRE` (`FBattleDisplay.cpp:1092`); recall is wired through
   `recallSelectedOffensivePendingSeekerAtHex` and grouped by `getSelectedOffensivePendingSeekerHexGroups`.
9. Active seeker movement is resolved in `resolveActiveSeekersForMovingPlayer` (`FTacticalGame.cpp:1031-1100`).
   Movement is greedy step-by-step (`:1070-1084`) but only the **final** `hex`/`heading` is stored; the
   traversed path is discarded.
10. Ships draw their movement path via `drawMovedHexes(dc, PointList, current)` (`FBattleBoard.cpp:354-374`,
    red pen width 3 for current ship, gray width 2 for others), fed by `tItr->second.path.getFullPath()`
    in `drawRoute` (`FBattleBoard.cpp:292-308`). Ship icon rotation uses
    `drawCenteredOnHex(img, p, rot)` (`FBattleBoard.cpp:235-248`), where `rot` is a heading unit and the
    image is rotated `rot * (pi/3)` radians. Ships pass `getRenderedHeadingForShip(...)` (`FBattleBoard.cpp:21-43`).
    Seekers are drawn with the default `rot=0` today.
11. `FBattleScreen` forwards model APIs to `FTacticalGame` (`include/tactical/FBattleScreen.h:359-457`):
    `getSeekerMissilesAtHex`, `getInactiveSeekerActivationHexes`, `selectSeekerActivationHex`,
    `getSelectedInactiveSeekerActivationStack`, `isOffensiveSeekerDeploymentMode`,
    `getSelectedOffensivePendingSeekerHexGroups`, `recallSelectedOffensivePendingSeekerAtHex`,
    `activateSelectedInactiveSeeker`, `completeSeekerActivationPhase`.
12. `icons/SeekerMissile.png` exists and is loaded into `m_seekerMissileIcon` (`FBattleBoard.cpp:69`).
13. Validation: model/tactical — `cd tests && make tactical-tests && ./tactical/TacticalTests`
    (binary `tests/tactical/TacticalTests`). GUI — `cd tests && make gui-tests && ./gui/GuiTests`
    (runner `tests/gui/GuiTests`, source `tests/gui/TacticalGuiLiveTest.cpp`).

## Resolved Design Decisions

1. **Activation interaction (issue #1):** Board click on an inactive seeker stack activates exactly one
   seeker from that hex. The lower status window lists **all** currently-activated seekers owned by the
   moving player (across all hexes); each row deactivates that seeker, which returns it to inactive and
   makes it reappear on the board as a clickable inactive stack. This replaces today's
   select-hex-then-pick-row flow. (User choice: "Board activates, list deactivates".)
2. **Placement button label (issue #2):** Derive the label from the weapon types present in the active
   placement source set: seekers only → "Seeker Placement Done"; mines only → "Mine Placement Done";
   both → "Weapon Placement Done".
3. **Pending firing-phase seeker visual (issue #3):** Draw pending offensive-fire seekers with the
   **same** seeker icon as active/inactive seekers; the existing recall list disambiguates which are
   uncommitted. (User choice: "Same icon as active seekers".)
4. **Seeker path style/persistence (issue #5):** Capture and draw each seeker's **actual stepped path**
   in a distinct color, rotate the icon by heading, and show it during the movement phase, mirroring ship
   path display (path is refreshed each movement turn, not retained for the whole battle). (User choice:
   "Exact stepped path, move phase".)
5. **Overlap scope (issue #4):** The fix targets the `BS_PlaceMines` lower panel (`drawPlaceMines`) — the
   instructions/"Done"-button region versus the placement source list region. (User choice: "Placement
   panel source list".)

## Assumptions

1. When a clicked inactive stack contains more than one inactive seeker, the model activates one
   deterministically (e.g., lowest `seekerID`); the player does not choose which.
2. During `PH_SEEKER_ACTIVATION`, the board must render **both** inactive stacks (clickable to activate)
   and active seekers (so activations are visible). Both use the existing seeker icon; no new asset.
3. The placement button label is computed from the deployable source set each time the placement panel is
   drawn (so it remains correct for the session). If the set is empty/unknown, fall back to
   "Mine Placement Done".
4. The seeker movement path is stored on the seeker model record (e.g., a `std::vector<FPoint>` populated
   in `resolveActiveSeekersForMovingPlayer`) and is purely render-supporting state; it must not introduce
   wx types into the model and must be cleared/repopulated per movement resolution. If tactical state is
   persisted, the new field must serialize compatibly (or be excluded from persistence) without breaking
   existing save/load or tests.
5. The seeker path pen color is a distinct color from ship paths (red/gray) — e.g., a cyan/orange tone —
   chosen by the implementer to be visually distinct on the board.
6. Most coverage comes from tactical model tests; GUI-visible seams (label selection, activation/
   deactivation wiring, pending/ path rendering hooks) are covered where wx behavior itself matters,
   consistent with the existing `tests/gui` approach.

## Likely Files To Modify

- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`

## Subtasks

Stable IDs: `SMC-01` … `SMC-07`.

### SMC-01 — Model: seeker deactivation, activate-one-from-hex, and activated-seeker query

Add the model-side APIs needed for the reworked activation screen, keeping all changes wx-free.

Scope:
- Add a method to deactivate a single active seeker by ID (active → inactive) for the moving player,
  the inverse of `activateSelectedInactiveSeeker`.
- Add a method to activate exactly one inactive seeker located at a given hex for the moving player
  (deterministic choice when multiple inactive seekers share the hex), so a board click can activate
  directly without the select-then-pick-row step.
- Add a const accessor returning all currently-active seekers owned by the moving player (across all
  hexes) for the status-window listing.
- Add `FBattleScreen` delegation for the new methods.

Allowed files:
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`

Acceptance Criteria:
- A model method changes exactly one active, moving-player-owned seeker back to inactive and returns
  success/failure; already-inactive or non-owned seekers are unaffected.
- A model method activates exactly one inactive moving-player-owned seeker at a specified hex
  (deterministic selection when several are present) and returns success/failure.
- A const accessor returns all active seekers owned by the moving player, suitable for UI listing,
  exposing no wx types.
- `FBattleScreen` exposes pass-through delegations for the new methods.
- Existing one-way `activateSelectedInactiveSeeker` and existing seeker tests remain green.
- No wx headers/types are introduced into the tactical model.

Documentation Impact:
- Doxygen comments for the new `FTacticalGame` methods/accessor and `FBattleScreen` delegations.
- No rules-document edit.

Dependencies: none.

### SMC-02 — Activation screen status-window rework (panel + board)

Rework the seeker activation UX so board clicks activate and the status window lists/deactivates all
activated seekers, and make activations visible on the board.

Scope:
- In `drawSeekerActivation` (`FBattleDisplay.cpp:1441`): replace the selected-stack inactive list with a
  list of **all** activated seekers (from SMC-01's accessor); each row is a clickable "deactivate" action.
  Keep the instructions text accurate to the new flow.
- Route activation-phase board clicks to "activate one seeker at the clicked hex" (SMC-01) instead of
  `selectSeekerActivationHex`; ensure the click handler / `checkSeekerActivationSelection` calls
  `m_parent->reDraw()` after activate/deactivate.
- In `drawSeekerMissiles` (`FBattleBoard.cpp:445`) for `PH_SEEKER_ACTIVATION`: render both inactive
  stacks (clickable) and active seekers, using the existing seeker icon.
- Add any `FBattleScreen` delegation needed for the panel to deactivate / list activated seekers.

Allowed files:
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`

Acceptance Criteria:
- Clicking an inactive seeker stack on the board during `PH_SEEKER_ACTIVATION` activates exactly one
  seeker from that hex and the screen redraws to reflect it.
- The status window lists every currently-activated seeker for the moving player (not just one stack),
  each as a deactivate control.
- Activating then deactivating a seeker restores it to inactive and it reappears on the board as a
  clickable inactive stack; the status window updates accordingly.
- Both inactive and active seekers are visible on the board throughout the activation phase.
- A redraw occurs after every activate/deactivate interaction (no stale/vanished state).
- Completing activation proceeds into movement exactly as before.

Documentation Impact:
- Doxygen comments for changed `FBattleDisplay`/`FBattleBoard` activation helpers and any new delegation.
- No rules-document edit.

Dependencies: SMC-01.

### SMC-03 — Dynamic placement "Done" button label

Make the placement "Done" button label reflect which ordnance types are being placed.

Scope:
- Compute the set of weapon types in `getDeployablePlacementSources()` and set the
  `m_buttonMinePlacementDone` label accordingly: seekers only → "Seeker Placement Done"; mines only →
  "Mine Placement Done"; both → "Weapon Placement Done" (fallback "Mine Placement Done"). Apply when the
  placement panel is drawn/shown (`drawPlaceMines`), via `SetLabel`.

Allowed files:
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`

Acceptance Criteria:
- With only seeker sources deployable, the button reads "Seeker Placement Done".
- With only mine sources deployable, the button reads "Mine Placement Done".
- With both mine and seeker sources deployable, the button reads "Weapon Placement Done".
- The label is correct whenever the placement panel is shown and does not regress the existing
  connect/show/hide and `onMinePlacementDone` completion behavior.

Documentation Impact:
- Doxygen/comment note on the label-selection logic.
- No rules-document edit.

Dependencies: none (sequenced after SMC-02 because it shares `FBattleDisplay`).

### SMC-04 — Pre-game placement panel layout overlap fix

Fix the overlapping regions in the `BS_PlaceMines` lower panel so the instructions/"Done"-button region
and the placement source list region no longer overlap.

Scope:
- Diagnose and correct the layout in `drawPlaceMines` (`FBattleDisplay.cpp:1371`) and any related
  spacer/coordinate logic (`reserveActionPromptLines`, `refreshActionButtonSpacer`,
  `getActionButtonTopSpacerHeight`, the action-button sizer). Ensure the instructions text + "Done" button
  do not overlap the source/stack list (drawn at `lMargin=310`) at the panel sizes used in game.

Allowed files:
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`

Acceptance Criteria:
- During pre-game ordnance placement, the instructions/"Done"-button region and the placement source list
  region are visually separated with no overlap.
- The placement source rows remain clickable (hit regions still align with drawn text).
- No regression to other lower-panel layouts (move/fire prompts) that share the same spacer logic.

Documentation Impact:
- Comment any layout/spacer adjustments.
- No rules-document edit.

Dependencies: none (sequenced after SMC-03 because it shares `FBattleDisplay`).

### SMC-05 — Render pending firing-phase seekers on the board

Make seekers deployed during the moving player's offensive fire phase visible on the board.

Scope:
- In `drawSeekerMissiles` (`FBattleBoard.cpp:445`), during `PH_ATTACK_FIRE` (offensive-fire seeker
  deployment), draw the seeker icon on every hex holding a pending offensive-fire seeker, using the same
  icon as active seekers. Source the pending hexes from the model (e.g., via the pending-deployment state
  / a small read-only `FBattleScreen`+`FTacticalGame` query if the existing
  `getSelectedOffensivePendingSeekerHexGroups` is insufficient because it is launcher-scoped).
- Confirm the existing recall list (`drawOffensiveSeekerPendingRows`, invoked at `FBattleDisplay.cpp:1092`)
  remains visible and functional so the player can un-deploy.

Allowed files:
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp` (read-only query delegation only)
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp` (read-only pending-hex query only, if needed)

Acceptance Criteria:
- After deploying a seeker during offensive fire, the seeker icon appears on each deployment hex on the
  board.
- Recalling a pending seeker (via the recall list) removes its icon from the board on redraw.
- Committed/active seeker rendering in other phases is unchanged.
- Any added model/screen query is read-only and introduces no wx types into the model.

Documentation Impact:
- Doxygen/comment for any new read-only pending-hex accessor and the board rendering branch.
- No rules-document edit.

Dependencies: none (sequenced after SMC-02 because it shares `FBattleBoard`).

### SMC-06 — Model: capture seeker movement path

Record each active seeker's traversed path during movement resolution so the board can draw it.

Scope:
- Add render-supporting state to the seeker model record (e.g., a `std::vector<FPoint>` of the stepped
  path for the most recent movement resolution) and populate it in
  `resolveActiveSeekersForMovingPlayer` (`FTacticalGame.cpp:1031-1100`) as each greedy step is taken
  (including the starting hex). Clear/replace it at the start of each movement resolution so it reflects
  only the current move.
- Expose a const accessor (and `FBattleScreen` delegation) returning per-seeker path data for rendering.
- Keep the model wx-free; ensure any persistence/serialization remains compatible (or excludes the new
  field) without breaking existing tests.

Allowed files:
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`

Acceptance Criteria:
- After active seekers move, each moved seeker exposes its stepped path (start hex through final hex) via
  a const accessor.
- The path reflects the actual greedy route taken (not a straight line) and matches the final
  `hex`/`heading` already computed.
- The path is reset/repopulated each movement resolution (no stale accumulation across turns).
- No wx types in the model; existing tactical tests remain green and any persistence is unaffected.

Documentation Impact:
- Doxygen for the new field/accessor and the path-capture change.
- No rules-document edit.

Dependencies: none (sequenced after SMC-01 because it shares `FTacticalGame`).

### SMC-07 — Draw seeker movement path and rotate seeker icon by heading

Render seeker movement on the board like ships: stepped path line plus heading-rotated icon.

Scope:
- In the board rendering (`FBattleBoard.cpp`), draw each moved seeker's path (from SMC-06) using a line
  style analogous to `drawMovedHexes` but in a distinct color, shown during the movement phase.
- Pass each seeker's `heading` to `drawCenteredOnHex(img, hex, rot)` so the seeker icon is rotated like
  ship icons (heading unit → `rot * pi/3`).

Allowed files:
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`

Acceptance Criteria:
- When active seekers move at the start of the movement phase, each seeker's stepped path is drawn on the
  main board in a color distinct from ship paths.
- Each seeker icon is rotated to reflect its current heading (consistent with the 6-direction ship icon
  rotation).
- The seeker path display is scoped to the movement phase (refreshed per movement turn), consistent with
  ship path behavior, and does not persist across the whole battle.
- No regression to existing seeker icon rendering in activation/other phases.

Documentation Impact:
- Doxygen/comment for the seeker path-draw helper and rotation change.
- No rules-document edit.

Dependencies: SMC-06.

## Dependency Ordering & Parallelization

Hard dependencies:
- SMC-02 depends on SMC-01.
- SMC-07 depends on SMC-06.

Shared-file serialization (no two run concurrently against the same file):
- `FTacticalGame.*`: SMC-01, SMC-06 (and read-only touch in SMC-05).
- `FBattleDisplay.*`: SMC-02, SMC-03, SMC-04.
- `FBattleBoard.*`: SMC-02, SMC-05, SMC-07.
- `FBattleScreen.*`: SMC-01, SMC-02, SMC-05, SMC-06.

Because the subtasks overlap heavily across these four files, run them **sequentially** in this order:

1. SMC-01
2. SMC-06
3. SMC-02
4. SMC-05
5. SMC-07
6. SMC-03
7. SMC-04

This order satisfies both hard dependencies (SMC-01 before SMC-02; SMC-06 before SMC-07) and keeps each
shared file edited by one subtask at a time. No subtasks are marked parallelizable.

## Documentation Impact (Overall)

- Update Doxygen comments on all new/changed `FTacticalGame`, `FBattleScreen`, `FBattleDisplay`, and
  `FBattleBoard` members.
- Any developer-facing notes on the seeker UI flow may be updated by the Documenter stage.
- `doc/rules/tactical_operations_manual.md` must not be edited.

## Artifact Conventions

Each subtask writes its orchestration artifacts to `artifacts/seeker-missile-ui-cleanup/<SMC-ID>/`:
- `implementer_report.md`, `implementer_result.json`
- `tester_prompt.txt` (Implementer → Tester handoff)
- downstream stages add `tester_*`, `documenter_*`, `verifier_*` files
A final reviewer pass writes to `artifacts/seeker-missile-ui-cleanup/<feature>_review/` (Coordinator default).

---

## Implementer Prompts

### SMC-01 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Add model-side APIs to support the reworked seeker activation screen in the SSW tactical layer, keeping
the tactical model free of wx types.

Implement in `include/tactical/FTacticalGame.h` / `src/tactical/FTacticalGame.cpp`:
1. A method to deactivate one active seeker by ID for the moving player (active → inactive), the inverse
   of `activateSelectedInactiveSeeker` (`FTacticalGame.cpp:968-986`). Return true only when an active,
   moving-player-owned seeker with that ID was found and flipped to inactive.
2. A method to activate exactly one inactive moving-player-owned seeker located at a given hex (choose
   deterministically — e.g., the lowest `seekerID` — when several inactive seekers share the hex). Return
   true on success.
3. A const accessor returning all active seekers owned by the moving player (across all hexes) as
   `std::vector<FTacticalSeekerMissileState>` for UI listing.

Add matching pass-through delegations in `include/tactical/FBattleScreen.h` / `src/tactical/FBattleScreen.cpp`.

Allowed files: `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`,
`include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`. Do not edit other files. Do not edit
`doc/rules/tactical_operations_manual.md`.

Validation: build and run the tactical model tests with `cd tests && make tactical-tests && ./tactical/TacticalTests`;
all tests must pass. Do not introduce wx headers/types into the model.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-01/tester_prompt.txt` describing the new
methods, their expected pass/fail return semantics (deactivate inverse of activate; activate-one-at-hex
deterministic selection; activated-seekers accessor scope), and pointing the Tester at the tactical test
suite for behavioral coverage.

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-01/implementer_report.md` and
`artifacts/seeker-missile-ui-cleanup/SMC-01/implementer_result.json`.

Completion gate: report success ONLY after all required artifacts above exist and all code changes are
committed.

### SMC-02 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Rework the seeker activation screen so board clicks activate seekers and the lower status window lists and
deactivates all activated seekers, and make activations visible on the board. This builds on the SMC-01
model APIs.

Implement:
1. `FBattleDisplay::drawSeekerActivation` (`src/tactical/FBattleDisplay.cpp:1441-1500`): replace the
   selected-stack inactive list with a list of all activated seekers for the moving player (SMC-01
   accessor, delegated via `FBattleScreen`). Render each as a clickable "Deactivate seeker #N …" row,
   storing hit regions and IDs. Update the instruction text to reflect the new flow (click a stack on the
   board to activate one; click a row to deactivate).
2. Activation-phase board clicks: route the click to "activate one seeker at the clicked hex" (SMC-01)
   instead of `selectSeekerActivationHex`. Update `FBattleBoard.cpp:186-187` accordingly.
3. `FBattleDisplay::checkSeekerActivationSelection` (`:1557-1569`): make row clicks call the deactivate
   delegation, and ensure `m_parent->reDraw()` is called after activate/deactivate (it is currently
   missing).
4. `FBattleBoard::drawSeekerMissiles` (`:445-471`): for `PH_SEEKER_ACTIVATION`, render BOTH inactive
   stacks (so they remain clickable) AND active seekers, using the existing `m_seekerMissileIcon`.
5. Add any `FBattleScreen` delegation required by the panel.

Allowed files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`,
`include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`, `include/tactical/FBattleScreen.h`,
`src/tactical/FBattleScreen.cpp`. Do not edit `doc/rules/tactical_operations_manual.md`.

Validation: `cd tests && make gui-tests && ./gui/GuiTests` must pass; also run
`cd tests && make tactical-tests && ./tactical/TacticalTests` to confirm no model regressions. Verify the
GUI still builds.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-02/tester_prompt.txt` describing the new
activation flow and the GUI seams to cover (board-click activates one; status window lists all activated;
row-click deactivates and restores to board; redraw after each interaction).

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-02/implementer_report.md` and
`implementer_result.json`.

Completion gate: report success ONLY after all required artifacts exist and all changes are committed.

### SMC-03 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Make the placement "Done" button label reflect the ordnance types being placed.

In `src/tactical/FBattleDisplay.cpp` (and `include/tactical/FBattleDisplay.h` if a helper is added),
compute the set of weapon types present in `m_parent->getDeployablePlacementSources()` and set the
`m_buttonMinePlacementDone` label via `SetLabel`: seekers only (`FWeapon::SM`) → "Seeker Placement Done";
mines only (`FWeapon::M`) → "Mine Placement Done"; both → "Weapon Placement Done"; empty/unknown →
"Mine Placement Done". Apply this whenever the placement panel is shown (`drawPlaceMines`,
`:1371-1439`), without breaking the existing connect/show/hide flow or `onMinePlacementDone`.

Allowed files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Validation: `cd tests && make gui-tests && ./gui/GuiTests` must pass (note: existing GUI tests in
`tests/gui/TacticalGuiLiveTest.cpp` reference the "Mine Placement Done" label — update or extend them only
if they are tester-owned per project policy; otherwise flag the dependency in the tester handoff). Confirm
the GUI builds.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-03/tester_prompt.txt` describing the three
label cases and the fallback, and flagging the existing label-string GUI assertions that may need updating.

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-03/implementer_report.md` and
`implementer_result.json`.

Completion gate: report success ONLY after all required artifacts exist and all changes are committed.

### SMC-04 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Fix the overlapping regions in the pre-game ordnance placement lower panel (`BS_PlaceMines`).

Diagnose and correct the layout so the instructions text + "Done" button region does not overlap the
placement source list (drawn at `lMargin=310`) in `FBattleDisplay::drawPlaceMines`
(`src/tactical/FBattleDisplay.cpp:1371-1439`). Adjust the relevant prompt-line reservation / action-button
spacer logic if needed (`reserveActionPromptLines`, `refreshActionButtonSpacer`,
`getActionButtonTopSpacerHeight`, and the action-button sizer set up in the constructor around
`:82-100`). Keep the source-row hit regions aligned with their drawn text, and do not regress the
move/fire prompt layouts that share the same spacer logic.

Allowed files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Validation: `cd tests && make gui-tests && ./gui/GuiTests` must pass; confirm the GUI builds. If practical,
launch the placement screen to confirm visually that the regions no longer overlap and document the check
in the report.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-04/tester_prompt.txt` describing the
overlap fix and any layout invariants to assert (source rows clickable; no overlap; other prompt layouts
unchanged).

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-04/implementer_report.md` and
`implementer_result.json`.

Completion gate: report success ONLY after all required artifacts exist and all changes are committed.

### SMC-05 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Render seekers deployed during the moving player's offensive fire phase on the main board.

In `FBattleBoard::drawSeekerMissiles` (`src/tactical/FBattleBoard.cpp:445-471`), add rendering of pending
offensive-fire seekers during `PH_ATTACK_FIRE`: draw `m_seekerMissileIcon` on every hex that holds a
pending offensive-fire seeker for the moving player, using the same icon as active seekers. Source the
pending hexes from the model; `getSelectedOffensivePendingSeekerHexGroups` is launcher-scoped, so if a
board-wide pending-hex list is needed, add a read-only accessor on `FTacticalGame` (returning pending
deployment hexes for the current phase) and delegate it through `FBattleScreen`. Confirm the existing
recall list (`drawOffensiveSeekerPendingRows`, invoked at `FBattleDisplay.cpp:1092`) still renders and
that recall removes the icon on redraw.

Allowed files: `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`,
`include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`, `include/tactical/FTacticalGame.h`,
`src/tactical/FTacticalGame.cpp` (the model/screen edits limited to a read-only pending-hex query). Do not
edit `doc/rules/tactical_operations_manual.md`. Introduce no wx types into the model.

Validation: `cd tests && make gui-tests && ./gui/GuiTests` and
`cd tests && make tactical-tests && ./tactical/TacticalTests` must pass; confirm the GUI builds.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-05/tester_prompt.txt` describing the
pending-seeker rendering and recall-removal behavior and any new read-only accessor to cover.

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-05/implementer_report.md` and
`implementer_result.json`.

Completion gate: report success ONLY after all required artifacts exist and all changes are committed.

### SMC-06 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Capture each active seeker's stepped movement path during movement resolution so the board can draw it.

In `include/tactical/FTacticalGame.h` / `src/tactical/FTacticalGame.cpp`, add render-supporting state to
the seeker model record (e.g., a `std::vector<FPoint>` holding the stepped path of the most recent
movement) and populate it inside `resolveActiveSeekersForMovingPlayer` (`:1031-1100`): record the starting
hex and each greedy step (`:1070-1084`) so the stored path matches the final `hex`/`heading`. Clear/replace
the path at the start of each movement resolution so it reflects only the current move. Add a const
accessor returning per-seeker path data and delegate it through `FBattleScreen`. Keep the model wx-free and
ensure any tactical-state persistence remains compatible (or excludes the new field) without breaking
existing tests.

Allowed files: `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`,
`include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`. Introduce no wx types into the model.

Validation: `cd tests && make tactical-tests && ./tactical/TacticalTests` must pass.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-06/tester_prompt.txt` describing the
path-capture semantics (start-through-final, matches greedy route, reset per resolution) and the accessor
to cover.

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-06/implementer_report.md` and
`implementer_result.json`.

Completion gate: report success ONLY after all required artifacts exist and all changes are committed.

### SMC-07 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Draw seeker movement on the board like ships: stepped path line plus heading-rotated icon. This builds on
the SMC-06 path accessor.

In `src/tactical/FBattleBoard.cpp` (and `include/tactical/FBattleBoard.h` as needed):
1. Draw each moved seeker's stepped path (from the SMC-06 accessor, via `FBattleScreen`) during the
   movement phase, using a line style analogous to `drawMovedHexes` (`:354-374`) but in a color distinct
   from ship paths (red/gray). Scope the display to the movement phase (refreshed per movement turn), not
   the whole battle.
2. Rotate the seeker icon by heading: pass each seeker's `heading` to
   `drawCenteredOnHex(img, hex, rot)` (`:235-248`) so the icon rotates `heading * pi/3` radians, matching
   ship icon rotation.

Allowed files: `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Validation: `cd tests && make gui-tests && ./gui/GuiTests` must pass; confirm the GUI builds. Confirm no
regression to seeker icon rendering in the activation/other phases.

Tester handoff: write `artifacts/seeker-missile-ui-cleanup/SMC-07/tester_prompt.txt` describing the path
draw and icon-rotation behavior and any seams to cover.

Artifacts: write `artifacts/seeker-missile-ui-cleanup/SMC-07/implementer_report.md` and
`implementer_result.json`.

Completion gate: report success ONLY after all required artifacts exist and all changes are committed.
