# Seeker Missile Fixes Round 2 Plan

## Output Artifact Path

`plans/seeker-missile-fixes-round2-plan.md` (repository-root-relative).

## Feature Restatement

A second round of fixes to the tactical seeker-missile system, observed while playing. The work spans
the pre-game ordnance deployment flow, the seeker activation HUD, seeker movement/impact rendering, and a
small documentation/test cleanup pass distilled from the `seeker-missile-ui-cleanup` review/verifier
reports. All changes live in the tactical model/UI layer (`FTacticalGame`, `FBattleScreen`,
`FBattleDisplay`, `FBattleBoard`, `ITacticalUI`/`WXTacticalUI`) plus the `Frontier.h` phase enums. No game
rules change; this is flow/interaction/rendering correctness plus the supporting model state.

1. **Pre-game deployment is broken** — the single combined mine+seeker placement phase (`BS_PlaceMines`)
   no longer lets the player place anything. Split it into **two sequential phases**: mine placement, then
   seeker placement. Empty phases (no deployable sources of that type) are skipped automatically.
2. **Deployment button text + prompt grammar** — the combined phase lost its working "Done" button text;
   the prompt above it is ungrammatical. Each new phase gets a correct button ("Mine Placement Done" /
   "Seeker Placement Done") and corrected prompt/selection wording.
3. **Movement-phase pending-seeker list is cut off** — when placing seekers during a movement turn, the
   pending-placement list renders under the ship-summary and is clipped because the status bar does not
   resize, blocking deletion. Move it into its own widget to the **left** of the ship-status widget,
   dynamically sized so it is fully visible and click-deletable.
4. **Activation-phase widget overlap** — on the seeker-activation phase before movement, the
   "Activated seekers" widget overlaps the phase-instructions widget. Both must be dynamically sized and
   positioned so they never overlap.
5. **Seeker-hit has no visual** — when a moving seeker hits a ship, its icon and path vanish immediately
   and the ICM-selection dialog appears with no indication of what happened. Keep the seeker icon and its
   stepped path on the board through the impact, redraw before the ICM/damage dialogs, and only remove the
   seeker after damage is reported.
6. **Activated-seeker list shows stale seekers** — a seeker activated in a prior round still appears in the
   "Activated seekers" list in later activation phases. That list is meant only for seekers activated
   **this** phase (so the player can change their mind that round). Seekers activated in prior rounds keep
   moving automatically and remain non-deactivatable; they simply must not appear in the list.
7. **Move-count overlay** — draw a small upright **red** number in the upper-right of an active seeker's
   hex showing how many hexes it moved that turn, rendered on top of the (rotated) icon but itself always
   upright. When more than one seeker occupies the same hex, stack the numbers vertically down the right
   side rather than overprinting them.
8. **Report cleanup** — address the code/doc/test notes carried in the `seeker-missile-ui-cleanup`
   reviewer/verifier reports (see "Report Cleanup Scope"). Process-only artifact backfill (SMC03/SMC06)
   is explicitly **out of scope** per user decision.

## Confirmed Repository Facts

1. `doc/rules/tactical_operations_manual.md` is a protected external artifact and must not be edited.
2. Battle setup states are `BS_*` and battle phases are `PH_*` in `include/Frontier.h:18-44`. Deployment is
   the single state `BS_PlaceMines`; battle phases include `PH_SEEKER_ACTIVATION`, `PH_MOVE`,
   `PH_ATTACK_FIRE`, etc.
3. The deployment phase is drawn by `FBattleDisplay::drawPlaceMines` (`src/tactical/FBattleDisplay.cpp:1371-1465`),
   which renders mixed mine+seeker sources. The current prompt text (`:1378-1383`) renders ungrammatically
   as *"The defensive player may now place / mines and seeker missiles / sets up their ships."* The
   selection line (`:1390`) is *"Select a source row to place mines or seeker missiles"*.
4. The deployment "Done" button logic (`:1436-1465`) sets a dynamic label
   ("Mine/Seeker/Weapon Placement Done") on the single `m_buttonMinePlacementDone`; only that button is
   ever shown/wired during `BS_PlaceMines`, and its handler `onMinePlacementDone` (`:1350-1357`) always
   calls `m_parent->completeMinePlacement()`. `m_buttonSeekerActivationDone` exists but is used only during
   the battle-phase `PH_SEEKER_ACTIVATION`. This combined-label hack is the regression.
5. Model deployment entry is `beginMinePlacement()` → `beginOrdnancePlacement()` (rebuilds both mine+seeker
   deployable sources, sets `BS_PlaceMines`) → `completeMinePlacement()` → `BS_SetupAttackFleet`
   (`src/tactical/FTacticalGame.cpp:2794-2827`). Deployable sources are
   `FTacticalDeploymentSource{ownerID, weaponType (FWeapon::M|SM), source}` exposed via
   `getDeployablePlacementSources()`. Deployment is entered from `FBattleDisplay.cpp:876` after defend-fleet
   setup.
6. The movement-phase pending offensive-fire seeker list is drawn by
   `FBattleDisplay::drawOffensiveSeekerPendingRows` (`:1525-1561`), invoked from `drawCurrentShipStats`
   (`:1009-1096`, list at `:1092`); its vertical space is constrained by the ship-stats block and the
   lower-panel layout state (`ensureLowerPanelLayoutState`, `:512-579`;
   `LOWER_PANEL_LAYOUT_RIGHT_SPLIT`/`LOWER_PANEL_LAYOUT_STACKED`). Deletion routes through
   `checkOffensiveSeekerPendingSelection` (`:1595-1609`) →
   `recallSelectedOffensivePendingSeekerAtHex(hex)`. Model state is `m_pendingOffensiveSeekerDeployments`;
   board-wide pending hexes are available via `getAllPendingOffensiveFireSeekerHexes()`.
7. `FBattleDisplay::drawSeekerActivation` (`:1467-1523`) draws the "Activated seekers:" header at hard-coded
   `y = BORDER (5)`, colliding with the action-prompt lines at `y = 5/21/37`
   (`ACTION_PROMPT_TOP_MARGIN=5`, `ACTION_PROMPT_LINE_HEIGHT=16`, `ACTION_PROMPT_MAX_LINES=3`); no vertical
   offset is taken from `getActionButtonRowBottom()`. The list is sourced from
   `getActiveSeekersByMovingPlayer()` (`FTacticalGame.h:616`), which returns **all** active moving-player
   seekers with no per-phase distinction.
8. Seeker activation state lives in `FTacticalSeekerMissileState.active`; `activateInactiveSeekerAtHex`
   (`FTacticalGame.h:603`) and `deactivateActiveSeekerByID` (`:586`) exist. There is no field recording
   *which* activation phase a seeker was activated in.
9. Seeker movement resolves in `resolveActiveSeekersForMovingPlayer()` (`FTacticalGame.cpp:1106-1182`),
   which populates each active seeker's `movementPath` (start hex through final hex) and detects contacts
   via `appendSeekerContactOutcome()`. `completeMovePhase()` (`:2971`) calls `checkForActiveSeekersOnPath`
   then `applyMovementSeekerDamage()` (`:2876-2906`), which calls `resolvePendingSeekerDetonationDamage()`
   (`:1184-1268`) — this runs `m_ui->runICMSelection()` and `m_ui->showDamageSummary()` (both blocking) and
   only afterward removes detonated seekers from `m_seekerMissiles`. There is **no board repaint** between
   path population and the dialogs, so the impact is never seen.
10. `ITacticalUI` already declares `virtual void requestRedraw() = 0;` (`include/tactical/ITacticalUI.h:52`),
    implemented by `WXTacticalUI`. This is the existing seam the model can call to force a board repaint.
11. Board seeker rendering is `FBattleBoard::drawSeekerMissiles` (`src/tactical/FBattleBoard.cpp:449-510`):
    active seekers are drawn via `drawCenteredOnHex(*m_seekerMissileIcon, hex, heading)` (icon rotated by
    `heading * pi/3`, `:239-252`). `drawSeekerPaths` (`:512-547`) draws the stepped cyan path **only during
    `PH_MOVE`** (guard at the top of the function). Hex pixel centers are `m_hexCenters[x][y]`
    (`computeCenters`, `:199-207`). Hexes moved this turn = `movementPath.size() - 1`. No upright text/number
    overlays currently exist on the board.
12. `FBattleScreen` forwards model APIs to `FTacticalGame`; new model methods need matching pass-through
    delegations there (pattern per `include/tactical/FBattleScreen.h`).
13. `seeker-missile-ui-cleanup` overall verdict was **CONDITIONAL PASS** with no blocking findings.
    Outstanding code/doc/test notes: (a) `drawSeekerPaths()` Doxygen in `include/tactical/FBattleBoard.h:128`
    omits the `#00CCCC` path color; (b) `AGENTS.md` prose for `getAllPendingOffensiveFireSeekerHexes` has a
    spurious leading `const` on the return type, and `AGENTS.md` references a combined-phase label test
    (`testMinePlacementDoneButtonLabelReflectsOrdnanceTypes`) that this plan makes obsolete; (c) a weak
    assertion in `tests/tactical/FTacticalSeekerMovementTest.cpp:734` (the cross-turn non-accumulation check
    always passes via its third disjunct). 8 pre-existing CWD-dependent GUI test failures are unrelated and
    not introduced by this work.
14. Validation: model/tactical — `cd tests && make tactical-tests && ./tactical/TacticalTests`. GUI —
    `cd tests && make gui-tests && xvfb-run -a ./gui/GuiTests` (X display / xvfb required).

## Resolved Design Decisions

1. **Two-phase deployment + empty-phase skipping (item 1):** Mine placement first, then seeker placement,
   as two sequential setup phases. A phase with zero deployable sources of its type is skipped
   automatically; if neither type has sources, deployment skips straight to attacker setup (preserving the
   old auto-skip behavior). *(User decision.)*
2. **Deployment wording (items 1–2):** *(User-approved.)*
   - Mine-phase prompt: "The defending player may now place mines before the attacker sets up their ships."
   - Seeker-phase prompt: "The defending player may now place seeker missiles before the attacker sets up
     their ships."
   - Selection line: "Select a source row to place mines." / "Select a source row to place seeker missiles."
   - Buttons: "Mine Placement Done" / "Seeker Placement Done".
   The "selection line" is the single instruction rendered directly above the deployable-source list during
   the placement phase (previously `FBattleDisplay.cpp:1390`).
3. **Seeker-hit visual lifecycle (item 5):** Keep the impacting seeker in the model with its `movementPath`
   intact; allow its icon and stepped path to draw through impact resolution (not gated to `PH_MOVE`); force
   a synchronous board repaint via `ITacticalUI::requestRedraw()` so the seeker is shown on the impact hex
   **before** the ICM/damage dialogs run; then remove the seeker and repaint. No animation and no extra
   acknowledgement modal. *(User decision: "Redraw + persist, then clear".)*
4. **Move-count overlay (item 7):** Display `movementPath.size() - 1` (actual hexes moved that turn) as an
   upright red number in the hex's upper-right, on any active seeker whenever it is drawn on the board,
   persisting between turns until the seeker moves again. When more than one seeker shares a hex, stack the
   numbers vertically down the right side of the hex instead of overprinting. *(User decision.)*
5. **Report-cleanup scope (item 8):** Fold in only the code/doc/test fixes (drawSeekerPaths Doxygen color;
   AGENTS.md signature/test-entry corrections; strengthen the weak `FTacticalSeekerMovementTest`
   assertion). Process-only SMC03/SMC06 artifact backfill is **excluded**. *(User decision.)*

## Assumptions

1. **(Assumption)** A second setup state is the cleanest fit for the seeker phase (e.g., a new
   `BS_PlaceSeekers` value in `include/Frontier.h`), mirroring `BS_PlaceMines`. The implementer may instead
   reuse `BS_PlaceMines` with a model-side "deployment kind" flag if that proves less invasive, provided the
   two-phase user-visible behavior and per-phase button/prompt text are achieved.
2. **(Assumption)** The seeker-placement phase gets its own dedicated "Done" button
   (e.g., `m_buttonSeekerPlacementDone`) and handler routing to a new `completeSeekerPlacement()`, rather
   than overloading `m_buttonSeekerActivationDone` (which belongs to the battle-phase activation flow).
3. **(Assumption)** Item 6 is implemented by stamping each seeker with the activation-phase index in which
   it was activated (e.g., a counter incremented when a seeker-activation phase begins) and adding a model
   accessor that returns only seekers activated in the current activation phase; the existing
   `getActiveSeekersByMovingPlayer()` continues to drive actual movement. The activation widget switches to
   the new "this-phase" accessor.
4. **(Assumption)** Move-count rendering uses the existing per-seeker `movementPath` already populated by
   `resolveActiveSeekersForMovingPlayer()`; no new model state is required for item 7. Number font/size and
   exact pixel offsets are implementer choices, subject to "small, red, upright, upper-right, stacked when
   co-located".
5. **(Assumption)** For item 3, the pending-seeker list becomes its own bounded region positioned left of
   the ship-status block within the existing lower panel; the panel's layout-state machine is extended (not
   replaced). Exact widths/margins are implementer choices, subject to "fully visible and click-deletable".
6. **(Assumption)** Item 5's repaint-before-dialog change reorders only when the seeker is removed relative
   to the dialogs; the detonation/damage computation and destroyed-ship cleanup ordering are otherwise
   preserved. The seeker is guaranteed removed after `showDamageSummary()` returns so no impacting seeker
   persists into the next turn.
7. **(Assumption)** Most coverage comes from tactical model tests; GUI-visible seams are covered where wx
   behavior itself matters, consistent with the existing `tests/gui` approach. New tests are authored by the
   downstream Tester stage, not the Implementer.

## Security Considerations

This feature is local single-process tactical UI/model logic with no trust boundary, authentication,
secret handling, untrusted external input, network surface, or destructive operation. **No subtask is
security-sensitive or high-risk**, so no specialist Security stage is required. The only durable invariant
to preserve is the module boundary rule: the tactical model (`FTacticalGame`, `ITacticalUI`) must remain
wx-free; wx types stay in `FBattleDisplay`/`FBattleBoard`/`WXTacticalUI`.

## Likely Files To Modify

- `include/Frontier.h` (deployment phase enum; SMF-01 only)
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`
- `include/tactical/ITacticalUI.h` (only if the existing `requestRedraw()` seam is insufficient; SMF-06)
- `include/gui/WXTacticalUI.h`, `src/gui/WXTacticalUI.cpp` (SMF-06, redraw seam implementation if extended)

## Report Cleanup Scope (item 8)

These are folded into the most-related subtask's downstream Documenter/Tester work rather than a standalone
subtask (per workflow boundaries):

- **drawSeekerPaths Doxygen `#00CCCC` color** (`include/tactical/FBattleBoard.h:128`) → SMF-07
  Documentation Impact (Implementer/Documenter on the board-rendering subtask).
- **AGENTS.md `getAllPendingOffensiveFireSeekerHexes` const-prose fix** → SMF-03 Documentation Impact.
- **AGENTS.md combined-phase label test entry (`testMinePlacementDoneButtonLabelReflectsOrdnanceTypes`)
  now obsolete** → SMF-02 Documentation Impact (remove/replace the entry as the combined-label logic is
  deleted).
- **Weak `tests/tactical/FTacticalSeekerMovementTest.cpp:734` assertion** → SMF-07 Tester handoff
  (tester-owned: strengthen to assert path size from the post-first-move hex instead of relying on the
  `movementTurn >= 2` disjunct).

## Subtasks

Stable IDs: `SMF-01` … `SMF-07`.

### SMF-01 — Model: two-phase deployment state machine

Split the combined mine+seeker deployment into two sequential setup phases at the model level, wx-free.

Scope:
- Add a second deployment setup state for seekers (e.g., `BS_PlaceSeekers` in `include/Frontier.h`), or an
  equivalent model-side deployment-kind distinction (Assumption 1).
- Split deployment entry so the mine phase builds **only mine** deployable sources and the seeker phase
  builds **only seeker** deployable sources (filter `FWeapon::M` vs `FWeapon::SM` when rebuilding
  `getDeployablePlacementSources()`-backing state).
- Transition logic: defend-fleet setup → mine phase (if any mine sources) → seeker phase (if any seeker
  sources) → attacker setup. Skip an empty phase automatically; if neither type has sources, go straight to
  `BS_SetupAttackFleet`. Add `completeSeekerPlacement()` (advances to `BS_SetupAttackFleet`) and make
  `completeMinePlacement()` advance into the seeker phase (or skip it when empty).
- Add `FBattleScreen` delegations for any new begin/complete/query methods.

Allowed files:
- `include/Frontier.h`
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`

Acceptance Criteria:
- Deployment presents a mine phase then a seeker phase as distinct model states; each phase's deployable
  source set contains only its own ordnance type.
- A phase with no deployable sources of its type is skipped; with neither type present, deployment advances
  directly to attacker setup.
- Completing the mine phase advances to the seeker phase (or skips it when empty); completing the seeker
  phase advances to attacker setup.
- `FBattleScreen` exposes pass-through delegations for the new begin/complete/query methods.
- No wx headers/types are introduced into the tactical model; existing tactical tests remain green.

Documentation Impact:
- Doxygen for the new state, the split begin/complete methods, and `FBattleScreen` delegations.
- DesignNotes.md note on the two-phase deployment flow (Documenter).
- No rules-document edit.

Dependencies: none.

### SMF-02 — Deployment UI: two phases, per-phase button and corrected prompt text

Render the two deployment phases with their own "Done" button and grammatically correct, type-specific
prompt/selection text. Builds on SMF-01.

Scope:
- Split/parameterize `drawPlaceMines` (`FBattleDisplay.cpp:1371-1465`) so each phase draws only its own
  source list and the correct text, and add the draw-dispatch branch for the new seeker setup state
  (`FBattleDisplay.cpp:634` area).
- Remove the broken combined dynamic-label logic (`:1436-1465`). Show `m_buttonMinePlacementDone`
  ("Mine Placement Done") during the mine phase and a dedicated seeker-placement button
  ("Seeker Placement Done", routing to `completeSeekerPlacement()` from SMF-01) during the seeker phase.
- Apply the approved wording (Decision 2) for each phase's prompt and selection line.

Allowed files:
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`

Acceptance Criteria:
- During the mine phase the prompt, selection line, and button read the mine-phase strings; during the
  seeker phase they read the seeker-phase strings (Decision 2 wording, verbatim).
- The mine-phase "Done" button completes mine placement; the seeker-phase "Done" button completes seeker
  placement; the correct single button is shown for each phase.
- Source rows remain clickable (hit regions align with drawn text) and ordnance can actually be placed in
  each phase.
- No combined "Weapon Placement Done" label path remains.

Documentation Impact:
- Doxygen/comment for the split draw + per-phase button wiring.
- Remove/replace the now-obsolete `testMinePlacementDoneButtonLabelReflectsOrdnanceTypes` reference and the
  combined-phase description in AGENTS.md (Documenter); update the tactical GUI-test bullet to the
  two-phase button text.
- No rules-document edit.

Dependencies: SMF-01.

### SMF-03 — Movement-phase pending-seeker list as a separate widget left of ship status

Move the offensive-fire pending-seeker list out from under the ship-summary into its own visible,
click-deletable region to the left of the ship-status widget.

Scope:
- Relocate `drawOffensiveSeekerPendingRows` (`FBattleDisplay.cpp:1525-1561`) rendering out of the
  ship-stats block and into a dedicated bounded region positioned left of the ship-status widget within the
  lower panel; extend the lower-panel layout state (`ensureLowerPanelLayoutState`, `:512-579`) so the new
  region is sized to show all pending rows without clipping.
- Keep deletion working: the recall hit regions/IDs (`m_pendingSeekerRecallRegions`/`...Hexes`) and
  `checkOffensiveSeekerPendingSelection` (`:1595-1609`) → `recallSelectedOffensivePendingSeekerAtHex` must
  align with the new layout, and a redraw must occur after a recall.

Allowed files:
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`

Acceptance Criteria:
- While placing seekers during a movement turn, the pending-placement list renders in its own region to the
  left of the ship-status widget and every pending row is fully visible (not clipped by the status bar).
- Each pending row remains clickable and recalling a pending seeker removes it from the list and the board
  on redraw.
- The ship-status widget and other lower-panel layouts (move/fire prompts) are not regressed.

Documentation Impact:
- Doxygen/comment for the new layout region and any new layout-state fields.
- Fix the AGENTS.md `getAllPendingOffensiveFireSeekerHexes` signature prose (remove the spurious leading
  `const` on the return type) (Documenter).
- No rules-document edit.

Dependencies: none (sequenced after SMF-02 because it shares `FBattleDisplay`).

### SMF-04 — Model: per-activation-phase tracking and this-phase activated-seeker accessor

Record which activation phase each seeker was activated in and expose only the current phase's activations.

Scope:
- Add a model marker for the activation-phase index (e.g., a counter advanced when a seeker-activation phase
  begins) and stamp each seeker with that index when it is activated
  (`activateInactiveSeekerAtHex`/related).
- Add a const accessor returning only seekers the moving player activated in the **current** activation
  phase (for the changeable list), leaving `getActiveSeekersByMovingPlayer()` to drive actual movement.
- Add `FBattleScreen` delegation for the new accessor. Keep the model wx-free; ensure any persistence stays
  compatible (or excludes the new field).

Allowed files:
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`

Acceptance Criteria:
- A seeker activated in a prior activation phase is **not** returned by the new this-phase accessor in later
  phases, while still being reported as active for movement purposes.
- A seeker activated in the current phase is returned by the new accessor and can still be deactivated that
  phase (via the existing `deactivateActiveSeekerByID`).
- The new accessor is exposed read-only through `FBattleScreen` and introduces no wx types into the model.
- Existing tactical tests remain green; any persistence remains compatible.

Documentation Impact:
- Doxygen for the new field/marker, accessor, and delegation; DesignNotes note on per-phase activation
  scoping (Documenter).
- No rules-document edit.

Dependencies: none (sequenced after SMF-01 because it shares `FTacticalGame`/`FBattleScreen`).

### SMF-05 — Activation widget: fix overlap and show only this-phase activations

Make the activation-phase HUD non-overlapping and back its list with the this-phase accessor from SMF-04.

Scope:
- In `drawSeekerActivation` (`FBattleDisplay.cpp:1467-1523`), dynamically position the "Activated seekers"
  block **below** the reserved action-prompt block (derive its top from `getActionButtonRowBottom()` /
  the prompt-line reservation) so it never overlaps the phase-instructions text; size both regions
  dynamically.
- Switch the list's data source from `getActiveSeekersByMovingPlayer()` to the SMF-04 this-phase accessor
  so only seekers activated this phase appear (and remain deactivatable). Ensure a redraw after each
  activate/deactivate.

Allowed files:
- `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`

Acceptance Criteria:
- During the seeker-activation phase, the "Activated seekers" widget and the phase-instructions widget do
  not overlap at the panel sizes used in game.
- The list shows only seekers activated in the current activation phase; seekers activated in prior rounds
  do not appear (though they still move).
- Activating then deactivating a current-phase seeker updates the list and board on redraw.
- No regression to other lower-panel phase layouts.

Documentation Impact:
- Doxygen/comment for the revised activation-widget layout and data source.
- No rules-document edit.

Dependencies: SMF-04 (uses the this-phase accessor); sequenced after SMF-03 (shares `FBattleDisplay`).

### SMF-06 — Seeker-hit visual lifecycle: persist icon + path through damage reporting

Keep an impacting seeker visible on its impact hex (with its path) until after damage is reported.

Scope:
- In the movement-resolution/damage path (`resolveActiveSeekersForMovingPlayer` /
  `applyMovementSeekerDamage` / `resolvePendingSeekerDetonationDamage`, `FTacticalGame.cpp:1106-1268`,
  `:2876-2906`), ensure the impacting seeker remains in `m_seekerMissiles` with its `movementPath` intact,
  call `m_ui->requestRedraw()` to force a synchronous board repaint showing the seeker on the impact hex
  **before** `runICMSelection()`/`showDamageSummary()`, and remove the seeker (and repaint) only **after**
  the damage summary returns. Preserve the existing detonation/damage and destroyed-ship cleanup ordering
  otherwise (Assumption 6).
- In `FBattleBoard::drawSeekerPaths` (`:512-547`) and `drawSeekerMissiles` (`:449-510`), allow the impacting
  seeker's stepped path and icon to render through impact resolution (not gated strictly to `PH_MOVE`) so
  the forced repaint actually shows the seeker and path.
- If the existing `ITacticalUI::requestRedraw()` proves insufficient for a synchronous mid-resolution
  repaint, extend the seam minimally and implement it in `WXTacticalUI` (keep the model wx-free).

Allowed files:
- `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`
- `include/tactical/ITacticalUI.h` (only if the redraw seam must be extended)
- `include/gui/WXTacticalUI.h`, `src/gui/WXTacticalUI.cpp` (redraw seam implementation, only if extended)

Acceptance Criteria:
- When a moving seeker hits a ship, the seeker icon and its stepped path are visible on the impact hex
  before the ICM-selection dialog appears.
- The seeker icon/path remain visible through ICM selection and the damage summary, then the seeker is
  removed and the board repaints.
- No impacting seeker persists into the following turn; detonation damage, ICM handling, and destroyed-ship
  cleanup produce the same outcomes as before (only the visual timing/order changes).
- The tactical model remains wx-free; existing tactical tests remain green.

Documentation Impact:
- Doxygen/comment for the reordered resolution + redraw call and any seam change; DesignNotes note on the
  impact-visibility behavior (Documenter).
- No rules-document edit.

Dependencies: none (sequenced after SMF-04 because it shares `FTacticalGame`/`FBattleScreen`).

### SMF-07 — Move-count number overlay on active seekers (+ board-render doc/test cleanup)

Draw an upright red per-turn move-count number on each active seeker, stacking when co-located.

Scope:
- In `FBattleBoard::drawSeekerMissiles` (`:449-510`), after drawing each active seeker icon, draw the move
  count `movementPath.size() - 1` as a small upright **red** number near the upper-right of the seeker's
  hex (using `m_hexCenters[x][y]` for positioning), always upright regardless of icon heading rotation.
- When more than one active seeker occupies the same hex, stack their numbers vertically down the right side
  of the hex instead of overprinting (Decision 4).
- Doc cleanup: add the `#00CCCC` path color to the `drawSeekerPaths()` Doxygen in
  `include/tactical/FBattleBoard.h:128`.

Allowed files:
- `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`

Acceptance Criteria:
- Each active seeker shows an upright red number equal to the hexes it moved that turn
  (`movementPath.size() - 1`) in its hex's upper-right, drawn on top of the (rotated) icon; the number is
  never rotated.
- When multiple active seekers share a hex, their numbers are stacked vertically (no overprint).
- The number persists between turns and updates after the seeker next moves; no regression to existing
  seeker icon/path rendering.
- The `drawSeekerPaths()` Doxygen header documents the `#00CCCC` path color.

Documentation Impact:
- Doxygen/comment for the move-count overlay helper; `drawSeekerPaths()` color note added.
- No rules-document edit.

Dependencies: none (sequenced after SMF-06 because it shares `FBattleBoard`).

## Dependency Ordering & Parallelization

Hard dependencies:
- SMF-02 depends on SMF-01.
- SMF-05 depends on SMF-04.

Shared-file serialization (no two run concurrently against the same file):
- `FTacticalGame.*` / `FBattleScreen.*`: SMF-01, SMF-04, SMF-06.
- `FBattleDisplay.*`: SMF-02, SMF-03, SMF-05.
- `FBattleBoard.*`: SMF-06, SMF-07.

Because the subtasks overlap heavily across these files, run them **sequentially** in this order:

1. SMF-01  (model: two-phase deployment)
2. SMF-02  (deployment UI)
3. SMF-03  (pending-list widget)
4. SMF-04  (model: per-phase activation tracking)
5. SMF-05  (activation widget overlap + this-phase list)
6. SMF-06  (seeker-hit visual lifecycle)
7. SMF-07  (move-count overlay + board doc/test cleanup)

This satisfies both hard dependencies and keeps each shared file edited by one subtask at a time. No
subtasks are marked parallelizable.

## Documentation Impact (Overall)

- Update Doxygen comments on all new/changed `FTacticalGame`, `FBattleScreen`, `FBattleDisplay`,
  `FBattleBoard`, and (if touched) `ITacticalUI`/`WXTacticalUI` members.
- Update `AGENTS.md`: two-phase deployment flow and button text; remove the obsolete
  `testMinePlacementDoneButtonLabelReflectsOrdnanceTypes` reference; fix the
  `getAllPendingOffensiveFireSeekerHexes` signature prose; note the seeker-impact visibility and per-phase
  activation behavior.
- Update `doc/DesignNotes.md` for the two-phase deployment, per-phase activation scoping, seeker-impact
  visibility, and move-count overlay.
- `doc/rules/tactical_operations_manual.md` must not be edited.

## Artifact Conventions

Each subtask writes its orchestration artifacts to `artifacts/seeker-missile-fixes-round2/<SMF-ID>/`:
- `implementer_report.md`, `implementer_result.json`
- `tester_prompt.txt` (Implementer → Tester handoff)
- downstream stages add `tester_*`, `documenter_*`, `verifier_*` files
A final reviewer pass writes to `artifacts/seeker-missile-fixes-round2/reviewer/` (Coordinator default).

---

## Implementer Prompts

### SMF-01 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Split the SSW tactical pre-game ordnance deployment into two sequential setup phases (mine placement, then
seeker placement) at the model level, keeping the tactical model free of wx types.

Implement:
1. Add a second deployment setup state for seekers in `include/Frontier.h` (e.g., `BS_PlaceSeekers`,
   alongside `BS_PlaceMines`), or an equivalent model-side deployment-kind distinction if that is less
   invasive — but the two-phase user-visible behavior must be achievable by the UI subtask.
2. In `src/tactical/FTacticalGame.cpp` / `include/tactical/FTacticalGame.h`, split deployment entry so the
   mine phase builds only mine (`FWeapon::M`) deployable sources and the seeker phase builds only seeker
   (`FWeapon::SM`) deployable sources (filter the source rebuild in `beginOrdnancePlacement`/the
   `getDeployablePlacementSources()`-backing state, `:2794-2827`).
3. Wire transitions: defend-fleet setup → mine phase (if any mine sources) → seeker phase (if any seeker
   sources) → `BS_SetupAttackFleet`. Skip an empty phase; if neither type has sources, go directly to
   `BS_SetupAttackFleet`. Add `completeSeekerPlacement()` (→ `BS_SetupAttackFleet`) and make
   `completeMinePlacement()` advance into the seeker phase (or skip when empty).
4. Add matching `FBattleScreen` pass-through delegations for the new begin/complete/query methods in
   `include/tactical/FBattleScreen.h` / `src/tactical/FBattleScreen.cpp`.

Allowed files: `include/Frontier.h`, `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`,
`include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`. Do not edit other files. Do not edit
`doc/rules/tactical_operations_manual.md`. Introduce no wx headers/types into the model.

Acceptance criteria: deployment exposes a mine phase then a seeker phase as distinct model states, each with
only its own ordnance type in its deployable source set; empty phases are skipped and deployment advances to
attacker setup when neither type is present; mine completion advances to the seeker phase (or skips when
empty) and seeker completion advances to attacker setup; `FBattleScreen` delegates the new methods; no wx in
the model; existing tactical tests stay green.

Validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`; all tests must pass.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-01/tester_prompt.txt` describing the new
states, the per-type source filtering, the begin/complete transition contract, and the empty-phase skip
rules, pointing the Tester at the tactical suite for state-machine coverage.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-01/implementer_report.md` and
`artifacts/seeker-missile-fixes-round2/SMF-01/implementer_result.json`. Continue past preflight without
pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.

### SMF-02 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Render the two-phase deployment UI with a per-phase "Done" button and corrected, type-specific prompt text.
This builds on the SMF-01 model states.

Implement in `include/tactical/FBattleDisplay.h` / `src/tactical/FBattleDisplay.cpp`:
1. Split/parameterize `drawPlaceMines` (`:1371-1465`) so each phase draws only its own source list and add
   the draw-dispatch branch for the new seeker setup state (near `:634`).
2. Remove the broken combined dynamic-label logic (`:1436-1465`). Show `m_buttonMinePlacementDone` labeled
   "Mine Placement Done" during the mine phase, and a dedicated seeker-placement "Done" button labeled
   "Seeker Placement Done" (routing to SMF-01's `completeSeekerPlacement()` via `FBattleScreen`) during the
   seeker phase; only the correct single button shows per phase.
3. Apply this exact wording:
   - Mine prompt: "The defending player may now place mines before the attacker sets up their ships."
   - Seeker prompt: "The defending player may now place seeker missiles before the attacker sets up their
     ships."
   - Mine selection line: "Select a source row to place mines."
   - Seeker selection line: "Select a source row to place seeker missiles."

Allowed files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria: each phase shows its own prompt/selection/button strings verbatim; the mine button
completes mine placement and the seeker button completes seeker placement; source rows are clickable and
ordnance can be placed in each phase; no "Weapon Placement Done" combined path remains.

Validation: `cd tests && make gui-tests && xvfb-run -a ./gui/GuiTests` and
`cd tests && make tactical-tests && ./tactical/TacticalTests` must pass; confirm the GUI builds. Existing GUI
tests that assert the old combined "…Placement Done" label or `testMinePlacementDoneButtonLabelReflectsOrdnanceTypes`
are now obsolete — flag them in the tester handoff for update rather than editing tester-owned files unless
project policy requires it.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-02/tester_prompt.txt` describing the two
phases' button/prompt text and completion routing, and flagging the obsolete combined-label assertions to
update and the AGENTS.md test-entry removal.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-02/implementer_report.md` and
`implementer_result.json`. Continue past preflight without pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.

### SMF-03 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Move the movement-phase offensive-fire pending-seeker list into its own visible, click-deletable region to
the left of the ship-status widget so it is no longer clipped by the status bar.

Implement in `include/tactical/FBattleDisplay.h` / `src/tactical/FBattleDisplay.cpp`:
1. Relocate the `drawOffensiveSeekerPendingRows` (`:1525-1561`) rendering out of the ship-stats block (it is
   currently invoked inside `drawCurrentShipStats`, `:1092`) into a dedicated bounded region positioned to
   the left of the ship-status widget within the lower panel. Extend the lower-panel layout state
   (`ensureLowerPanelLayoutState`, `:512-579`) so the new region is sized to display all pending rows
   without clipping.
2. Keep deletion working: ensure the recall hit regions/IDs (`m_pendingSeekerRecallRegions`/`...Hexes`) and
   `checkOffensiveSeekerPendingSelection` (`:1595-1609`) → `recallSelectedOffensivePendingSeekerAtHex` align
   with the new layout, and that a redraw occurs after a recall.

Allowed files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria: while placing seekers during a movement turn, every pending row renders fully visible in
its own region left of the ship-status widget; each row stays clickable; recalling a pending seeker removes
it from the list and the board on redraw; the ship-status widget and the move/fire lower-panel layouts are
not regressed.

Validation: `cd tests && make gui-tests && xvfb-run -a ./gui/GuiTests` must pass; confirm the GUI builds. If
practical, launch the movement-phase seeker placement to confirm visibility/deletion and note it in the
report.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-03/tester_prompt.txt` describing the new
pending-list region and the visibility/clickability/recall invariants to cover, and note the AGENTS.md
`getAllPendingOffensiveFireSeekerHexes` signature-prose correction for the Documenter.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-03/implementer_report.md` and
`implementer_result.json`. Continue past preflight without pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.

### SMF-04 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Add model-side tracking so the seeker-activation list can show only seekers activated in the current
activation phase, keeping the tactical model wx-free.

Implement in `include/tactical/FTacticalGame.h` / `src/tactical/FTacticalGame.cpp`:
1. Add a model marker for the current activation-phase index (e.g., a counter advanced when a
   seeker-activation phase begins) and stamp each seeker with that index when it is activated (in/around
   `activateInactiveSeekerAtHex`, `:603`, and any other activation entry points).
2. Add a const accessor returning only the moving player's seekers activated in the **current** activation
   phase (for the changeable list). Leave `getActiveSeekersByMovingPlayer()` (`:616`) unchanged for driving
   actual movement.
3. Keep any tactical-state persistence compatible (or exclude the new field) without breaking existing
   tests.

Add a matching read-only `FBattleScreen` pass-through delegation in `include/tactical/FBattleScreen.h` /
`src/tactical/FBattleScreen.cpp`.

Allowed files: `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`,
`include/tactical/FBattleScreen.h`, `src/tactical/FBattleScreen.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`. Introduce no wx headers/types into the model.

Acceptance criteria: a seeker activated in a prior activation phase is excluded by the new this-phase
accessor in later phases while still reported active for movement; a seeker activated this phase is included
and remains deactivatable via `deactivateActiveSeekerByID`; the accessor is read-only through `FBattleScreen`
and wx-free; existing tactical tests stay green and persistence stays compatible.

Validation: `cd tests && make tactical-tests && ./tactical/TacticalTests`; all tests must pass.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-04/tester_prompt.txt` describing the
per-phase activation marker and the this-phase accessor contract (prior-phase exclusion; current-phase
inclusion; movement reporting unchanged) for tactical coverage.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-04/implementer_report.md` and
`implementer_result.json`. Continue past preflight without pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.

### SMF-05 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Fix the seeker-activation HUD so the "Activated seekers" widget never overlaps the phase-instructions widget,
and back the list with the SMF-04 this-phase accessor.

Implement in `include/tactical/FBattleDisplay.h` / `src/tactical/FBattleDisplay.cpp`:
1. In `drawSeekerActivation` (`:1467-1523`), position the "Activated seekers" block dynamically **below** the
   reserved action-prompt block (derive its top from the action-prompt reservation /
   `getActionButtonRowBottom()` instead of the hard-coded `y = BORDER`), and size both regions dynamically so
   they never overlap at in-game panel sizes.
2. Switch the list's data source from `getActiveSeekersByMovingPlayer()` to the SMF-04 this-phase accessor
   (via `FBattleScreen`) so only current-phase activations appear. Ensure a redraw occurs after each
   activate/deactivate.

Allowed files: `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleDisplay.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria: the "Activated seekers" widget and phase-instructions widget do not overlap; the list
shows only seekers activated this phase (prior-round seekers excluded though they still move); activate then
deactivate of a current-phase seeker updates list and board on redraw; no regression to other lower-panel
phase layouts.

Validation: `cd tests && make gui-tests && xvfb-run -a ./gui/GuiTests` and
`cd tests && make tactical-tests && ./tactical/TacticalTests` must pass; confirm the GUI builds.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-05/tester_prompt.txt` describing the
non-overlap layout invariant and the this-phase list behavior to cover.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-05/implementer_report.md` and
`implementer_result.json`. Continue past preflight without pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.

### SMF-06 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Keep an impacting seeker visible on its impact hex (with its stepped path) until after damage is reported,
instead of removing it before the ICM/damage dialogs appear.

Implement, keeping the tactical model wx-free:
1. In `src/tactical/FTacticalGame.cpp` (`resolveActiveSeekersForMovingPlayer` `:1106-1182`,
   `applyMovementSeekerDamage` `:2876-2906`, `resolvePendingSeekerDetonationDamage` `:1184-1268`): ensure the
   impacting seeker stays in `m_seekerMissiles` with its `movementPath` intact, call `m_ui->requestRedraw()`
   to force a board repaint showing the seeker on its impact hex **before** `runICMSelection()` /
   `showDamageSummary()`, and remove the seeker (then repaint) only **after** the damage summary returns.
   Preserve the existing detonation/damage computation and destroyed-ship cleanup ordering otherwise; no
   impacting seeker may persist into the next turn.
2. In `src/tactical/FBattleBoard.cpp` (`drawSeekerPaths` `:512-547`, `drawSeekerMissiles` `:449-510`): allow
   the impacting seeker's icon and stepped path to render through impact resolution (not gated strictly to
   `PH_MOVE`) so the forced repaint actually shows them.
3. Only if `ITacticalUI::requestRedraw()` (`include/tactical/ITacticalUI.h:52`) is insufficient for a
   synchronous mid-resolution repaint, extend the seam minimally and implement it in
   `src/gui/WXTacticalUI.cpp` — keep the model free of wx types.

Allowed files: `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`,
`include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`, `include/tactical/FBattleScreen.h`,
`src/tactical/FBattleScreen.cpp`, `include/tactical/ITacticalUI.h` (only if the seam must be extended),
`include/gui/WXTacticalUI.h`, `src/gui/WXTacticalUI.cpp` (only if the seam must be extended). Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria: on a seeker hit, the seeker icon and stepped path are visible on the impact hex before
the ICM dialog appears and remain through ICM/damage reporting, after which the seeker is removed and the
board repaints; no impacting seeker survives into the next turn; detonation/ICM/destroyed-ship outcomes are
unchanged (only visual timing/order changes); the model stays wx-free; existing tactical tests stay green.

Validation: `cd tests && make tactical-tests && ./tactical/TacticalTests` and
`cd tests && make gui-tests && xvfb-run -a ./gui/GuiTests` must pass; confirm the GUI builds.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-06/tester_prompt.txt` describing the
persist-redraw-then-clear ordering, the "no seeker survives into next turn" invariant, and the unchanged
damage/ICM outcomes to cover.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-06/implementer_report.md` and
`implementer_result.json`. Continue past preflight without pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.

### SMF-07 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

Draw an upright red per-turn move-count number on each active seeker, stacking the numbers when seekers
share a hex, and close out the board-render documentation note.

Implement in `include/tactical/FBattleBoard.h` / `src/tactical/FBattleBoard.cpp`:
1. In `drawSeekerMissiles` (`:449-510`), after each active seeker icon is drawn, draw the move count
   `movementPath.size() - 1` as a small **red**, always-upright number near the upper-right of the seeker's
   hex (use `m_hexCenters[x][y]` for positioning). The number must not be rotated with the icon.
2. When more than one active seeker occupies the same hex, stack their numbers vertically down the right side
   of the hex instead of overprinting them.
3. Add the `#00CCCC` path color to the `drawSeekerPaths()` Doxygen comment in
   `include/tactical/FBattleBoard.h:128`.

Allowed files: `include/tactical/FBattleBoard.h`, `src/tactical/FBattleBoard.cpp`. Do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria: each active seeker shows an upright red number equal to `movementPath.size() - 1` in its
hex's upper-right on top of the (rotated) icon, never rotated; numbers stack vertically when seekers share a
hex; the number persists between turns and updates after the next move; no regression to existing seeker
icon/path rendering; the `drawSeekerPaths()` Doxygen documents the `#00CCCC` color.

Validation: `cd tests && make gui-tests && xvfb-run -a ./gui/GuiTests` must pass; confirm the GUI builds.

Tester handoff: write `artifacts/seeker-missile-fixes-round2/SMF-07/tester_prompt.txt` describing the
move-count overlay behavior (count value, upright orientation, upper-right placement, stacking) to cover.
Additionally instruct the Tester to strengthen the weak cross-turn non-accumulation assertion at
`tests/tactical/FTacticalSeekerMovementTest.cpp:734` so it asserts `movementPath` size from the
post-first-move hex rather than relying on the `movementTurn >= 2` disjunct.

Artifacts: write `artifacts/seeker-missile-fixes-round2/SMF-07/implementer_report.md` and
`implementer_result.json`. Continue past preflight without pausing when no blocker is present.

Completion gate: Do not report success unless all required artifacts exist and all changes are committed.
