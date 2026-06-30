# Pre-Game Ordnance Placement Fixes Plan

## Output Artifact Path

`plans/pregame-ordnance-placement-plan.md`

Coordinator artifact root for this plan: `artifacts/pregame-ordnance-placement/`
with per-subtask subdirectories `PGS-01` … `PGS-04`.

## Feature Restatement

Fix and complete the **pre-game setup** ordnance-placement experience for the
tactical battle screen so the defending player can place mines and seeker
missiles correctly. Scope is limited to the pre-game placement phases
(`BS_PlaceMines`, `BS_PlaceSeekers`); attack-phase / in-battle seeker deployment
is out of scope.

Required end-state behavior:

1. **Mines.** Clicking a board hex during the mine phase places a minefield in
   that hex; clicking it again removes it (one minefield per hex — toggle). When
   more than one ship on the board has deployable mines, the player must be able
   to select which ship is placing them. The game records which ship placed the
   mine in each hex; removing a mine returns it to the inventory of the ship that
   placed it, regardless of which source row is currently selected.
2. **Seeker missiles.** When more than one ship has seeker missiles, the player
   must be able to select which ship is deploying. Clicking a board hex adds one
   seeker missile to that hex; clicking again adds another (stacking allowed — no
   board-click removal). A list displayed in the center of the bottom panel shows
   all placed seeker missiles, grouped by hex and source ship; clicking a row
   undeploys one missile and returns it to the ship it came from.

## Confirmed Repository Facts

1. The board click path is `FBattleBoard::onLeftUp()` →
   `FBattleScreen::handleHexClick()` → `FTacticalGame::handleHexClick()`
   (`src/tactical/FTacticalGame.cpp:3282`). The lower-panel (HUD) click path is
   `FBattleDisplay::onLeftUp()` (`src/tactical/FBattleDisplay.cpp:687`).
2. The mine phase is entered from `FBattleDisplay::onSetSpeed()`
   (`src/tactical/FBattleDisplay.cpp:884`). After the defender's last ship speed
   is set and `getDone()` is true, it calls `m_parent->beginMinePlacement()`
   (line 897) — which selects the mine ship+weapon into `m_curShip`/`m_curWeapon`
   via `selectPlacementSourceByIndex()` — and then **unconditionally calls
   `m_parent->setShip(NULL)` on line 907**. `setShip()` is
   `m_curShip = ship` (`include/tactical/FTacticalGame.h:346`), so this nulls
   `m_curShip` immediately after the mine phase is entered.
3. `FTacticalGame::handleHexClick()` `case BS_PlaceMines:` calls
   `placeMineAtHex(hex)` when `m_curWeapon` is an `FWeapon::M`
   (`src/tactical/FTacticalGame.cpp:3300-3307`). `placeMineAtHex()`
   (line 3180) returns false immediately when `m_curShip == NULL ||
   m_curWeapon == NULL`. Because of fact (2), `m_curShip` is null on entry, so
   every mine board click is silently dropped. This is the root cause of "can't
   place mines."
4. The seeker phase is entered via the "Mine Placement Done" button →
   `FBattleDisplay::onMinePlacementDone()` (`src/tactical/FBattleDisplay.cpp:1368`)
   → `FTacticalGame::completeMinePlacement()` (line 2927) →
   `beginSeekerPlacement()` (line 2871), which re-selects the seeker source and is
   **not** followed by a `setShip(NULL)`. So `m_curShip`/`m_curWeapon` remain
   valid and seeker board clicks work — explaining why seekers work but mines do
   not.
5. `beginOrdnancePlacement()` (`src/tactical/FTacticalGame.cpp:2894`) ignores the
   return value of `selectPlacementSourceByIndex()` and sets
   `BS_PlaceMines` unconditionally; if selection ever fails, the phase is entered
   with no valid current weapon. (Latent robustness gap on the same entry path.)
6. `placeOrdnanceAtHex()` (`src/tactical/FTacticalGame.cpp:3200`) always attempts
   `removePlacedOrdnanceForSelection(hex, removed)` first. For seekers this makes
   a second click on the same hex+source remove the seeker instead of adding
   another (current toggle behavior). `placeSeekerFromSelection()` (line 2009) is
   already additive (no per-hex limit); `placeMineFromSelection()` (line 1988) is
   one-per-hex.
7. `removePlacedOrdnanceForSelection()` (`src/tactical/FTacticalGame.cpp:1932`)
   only removes a placed record when `sourceMatchesSelection(itr->source)` — i.e.
   the currently selected source placed it. With multiple mine ships, a mine
   placed by ship A cannot be removed while ship B is selected.
   `FTacticalPlacedOrdnance` (`include/tactical/FTacticalGame.h:123`) records the
   placing `source` (shipID, weaponIndex, weaponID); `restoreAmmoForSource()`
   (line 1964) returns ammo to that exact source.
8. A source/ship-selection row UI already exists: `drawPlaceMines()`
   (`src/tactical/FBattleDisplay.cpp:1389`) and `drawPlaceSeekers()` (line 1466)
   render one clickable row per deployable source at `lMargin = 310`, starting at
   `getActionButtonRowBottom()`, capturing click regions in `m_shipNameRegions` /
   `m_shipSelectionSourceIndices`. `FBattleDisplay::onLeftUp()` routes
   `BS_PlaceMines` and `BS_PlaceSeekers` clicks to `checkShipSelection()`
   (line 1661), which calls `selectPlacementSourceByIndex()`. The rows
   auto-expand the panel height when they overflow (added in round 3), but the reported
   "can't select which ship" symptom indicates the rows are still not reliably
   visible/clickable in practice.
9. The board renders placed ordnance via `FBattleBoard::drawPlacementOrdnanceHexes()`
   (`src/tactical/FBattleBoard.cpp:660`), shading each placed-ordnance hex with a
   per-source color, for both `BS_PlaceMines` and `BS_PlaceSeekers` (added in round 3).
10. An analogous centered/HUD recall list already exists for **attack-phase**
    offensive-fire seekers: `drawOffensiveSeekerPendingRows()`
    (`src/tactical/FBattleDisplay.cpp:1618`) + `checkOffensiveSeekerPendingSelection()`
    (line 1693) + model accessor `getSelectedOffensivePendingSeekerHexGroups()`
    and `recallSelectedOffensivePendingSeekerAtHex()`. There is **no** equivalent
    list for the pre-game `BS_PlaceSeekers` phase.
11. The existing pre-game behavioral regression test
    `testPreGameOrdnancePlacementRecordingBehavior`
    (`tests/tactical/FTacticalGameMechanicsTest.cpp`, from round 3) enters mine
    placement at the **model** level via `beginMinePlacement()` then
    `handleHexClick()`, which never reproduces the GUI-level `setShip(NULL)` from
    fact (2). That is why the regression passed while the real GUI flow stayed
    broken. New coverage for PGS-01 must exercise the real GUI entry sequence
    (`onSetSpeed` path) or replicate the `beginMinePlacement()`-then-`setShip(NULL)`
    sequence so it fails against the unfixed code.
12. `doc/rules/tactical_operations_manual.md` is a protected external artifact
    and must not be edited.

## Resolved Design Decisions

1. **(D1) Phase asymmetry.** Mines toggle on the board (click places / click
   removes, one minefield per hex) with no center list. Seekers stack on the
   board (each click adds one) and are removed **only** via the centered
   placed-seeker undeploy list.
2. **(D2) Mine removal selection.** Clicking any mined hex removes that minefield
   regardless of which source row is currently selected, and auto-returns the
   mine to the inventory of the ship that placed it (using the recorded placed-
   ordnance source).
3. **(D3) Seeker undeploy list grouping.** The centered list groups placed
   seekers by (hex + source ship), each row showing the source ship, the hex, and
   the count (e.g. `Minelayer at (5,7): 3`). Clicking a row returns exactly one
   seeker from that ship at that hex to that ship's launcher.
4. **(D4) Ship selection requirement.** When more than one ship has the relevant
   deployable ordnance for the active phase, the player can select among them via
   the source rows; with a single eligible ship it is auto-selected. Selecting a
   source sets the active placement ship/weapon for subsequent board clicks.
5. **(D5) Scope.** Only pre-game `BS_PlaceMines` / `BS_PlaceSeekers` behavior is
   in scope. Attack-phase offensive-fire seeker deployment and in-battle mine
   laying are unchanged.

## Assumptions

1. The defect set and required additions are confined to the tactical surfaces
   `FTacticalGame` (model), `FBattleDisplay` (lower-panel HUD), and
   `FBattleScreen` (delegation). `FBattleBoard` already renders placed ordnance
   for both phases and is unlikely to need changes beyond, at most, read-back of
   model state. No weapon-stat, strategic-layer, or ship-class changes are
   required. (Labeled assumption; each subtask's allowed-files list reflects the
   expected minimal surface and the implementer confirms during work.)
2. The existing per-source color/shaded-hex helpers used by
   `drawPlacementOrdnanceHexes()` are reusable for displaying stacked seekers; no
   new asset is required.
3. The model already retains enough state to drive the centered seeker list
   (`m_seekerMissiles` inactive entries with `hasSource`/`source`, and
   `m_placedOrdnance`); exposing a read-only grouped accessor and a
   recall-by-(hex+source) method is part of PGS-04.
4. Behavioral verification of the mine entry fix (PGS-01) requires a test that
   drives the real GUI entry path (e.g. a `TacticalGuiLiveTest` exercising
   `onSetSpeed` → mine placement → hex click) or a model-level test that
   replicates the `beginMinePlacement()` + `setShip(NULL)` sequence; a plain
   model `beginMinePlacement()` + `handleHexClick()` test is insufficient because
   it does not reproduce the regression.

## Likely Files To Modify

1. `src/tactical/FBattleDisplay.cpp` / `include/tactical/FBattleDisplay.h`
2. `src/tactical/FTacticalGame.cpp` / `include/tactical/FTacticalGame.h`
3. `src/tactical/FBattleScreen.cpp` / `include/tactical/FBattleScreen.h`
   (delegation forwarders for any new model accessors)

(Labeled as likely files based on repository evidence; the exact subset per
subtask is given in each Implementer prompt's allowed-files list.)

## Subtasks

### PGS-01 — Restore Pre-Game Mine Board Placement (Entry-State Fix)

Fix the root cause that drops every mine board click: the mine phase is entered
with `m_curShip` nulled. Ensure that once `BS_PlaceMines` is entered, the active
mine ship and mine launcher remain selected so a board click reaches the mine
recording path.

Acceptance Criteria:
- After the defending player finishes ship setup and the game enters the mine
  placement phase through the real flow (the `onSetSpeed` → `beginMinePlacement`
  path), `m_curShip` and `m_curWeapon` reference the selected mine ship and its
  mine launcher (they are not null) when the first board click arrives.
- During pre-game `BS_PlaceMines`, clicking a valid in-bounds hex records a mine:
  the selected source's mine launcher ammo decrements by one, the hex is added to
  `m_minedHexList`, and a placed-ordnance record is appended to `m_placedOrdnance`.
- Placed mines are highlighted on the board during the mine phase.
- The pre-game mine entry no longer enters `BS_PlaceMines` in a state where no
  valid mine source is selected (the entry only proceeds when a deployable mine
  source is successfully selected).
- A behavioral test reproduces the real entry sequence (driving the GUI
  `onSetSpeed`/mine-entry path, or replicating `beginMinePlacement()` followed by
  the GUI's `setShip(NULL)` before the click) and asserts a mine is recorded;
  this test fails against the current (unfixed) code and passes after the fix.

Documentation Impact:
- Update Doxygen comments for the changed entry/handler methods
  (`onSetSpeed`, `beginOrdnancePlacement`/`beginMinePlacement`, and any guard
  added). No rules-document edit.

### PGS-02 — Multi-Ship Source Selection For Mine And Seeker Phases

Make the source/ship-selection rows in the lower panel reliably visible and
clickable during both `BS_PlaceMines` and `BS_PlaceSeekers`, so when more than
one ship has the relevant ordnance the player can choose which ship deploys, and
the chosen source drives subsequent board clicks.

Acceptance Criteria:
- During `BS_PlaceMines`, every ship that has a deployable mine launcher appears
  as a selectable source row; during `BS_PlaceSeekers`, every ship that has a
  deployable seeker launcher appears as a selectable source row.
- Clicking a source row selects that ship+weapon as the active placement source
  (updates the selected-source index and `m_curShip`/`m_curWeapon`), and the
  selected row is visibly indicated.
- The selectable rows render in a visible region of the lower panel and their
  click regions align with their drawn positions (clicking a row reliably selects
  the intended source; rows are not clipped or rendered behind the ship-status
  display).
- After selecting a different source, the next board click places ordnance from
  the newly selected ship/weapon.
- With exactly one eligible ship for the phase, that source is auto-selected and
  placement works without requiring a manual row click.

Documentation Impact:
- Update Doxygen comments for the modified `FBattleDisplay` source-row
  rendering/selection helpers (`drawPlaceMines`, `drawPlaceSeekers`,
  `checkShipSelection`). No rules-document edit.

### PGS-03 — Seeker Stacking (Additive Board Placement, No Board-Click Removal)

Change pre-game seeker placement so each board click adds one seeker to the hex
(stacking permitted) and a board click never removes a seeker; board-click
toggle/removal is reserved for mines only.

Acceptance Criteria:
- During pre-game `BS_PlaceSeekers`, clicking a hex always adds one inactive
  seeker for the selected source: the seeker launcher ammo decrements by one, a
  new inactive seeker record is created, and a placed-ordnance record is appended
  — every click, including repeated clicks on the same hex.
- Repeated clicks on the same hex by the same source accumulate multiple seekers
  in that hex (no toggle/removal occurs from the board during seeker placement).
- Mine placement retains its toggle behavior (clicking a mined hex removes the
  mine) and is not affected by this change.
- Seeker placement stops adding when the selected source's launcher ammo is
  exhausted.

Documentation Impact:
- Update Doxygen comments for the changed placement-dispatch/seeker-placement
  methods (`placeOrdnanceAtHex`, `placeSeekerFromSelection`, and any helper used
  to distinguish phase removal behavior). No rules-document edit.

### PGS-04 — Centered Placed-Seeker Undeploy List

Add a list displayed in the center of the lower panel during `BS_PlaceSeekers`
that shows all placed seekers grouped by (hex + source ship) with counts, and
lets the player click a row to undeploy one seeker — returning it to the ship it
came from. Also implement the mine removal semantics for multi-ship mines (remove
regardless of selected source, auto-return to the placing ship).

Acceptance Criteria:
- During pre-game `BS_PlaceSeekers`, a centered lower-panel list shows one row per
  (hex, source ship) that has at least one placed inactive seeker, displaying the
  source ship, the hex, and the count.
- Clicking a list row undeploys exactly one seeker for that (hex, source ship):
  the matching inactive seeker record is removed, the placed-ordnance record is
  removed, and one round of ammo is returned to that ship's seeker launcher.
- The list updates after each placement and each undeploy, and the board
  highlight reflects the remaining placed seekers.
- During pre-game `BS_PlaceMines`, clicking a mined hex removes that minefield
  regardless of which source row is currently selected, and returns the mine to
  the ship that placed it (ammo restored to the recorded placing source); the
  hex is removed from `m_minedHexList` and the placed-ordnance record is removed.
- The centered seeker list does not appear during the mine phase, and the seeker
  list rows do not overlap the source-selection rows or the ship-status display.

Documentation Impact:
- Update Doxygen comments for the new model accessor/recall method on
  `FTacticalGame`, the new `FBattleDisplay` list-render/click helpers, and any
  `FBattleScreen` delegation forwarders. `doc/UsersGuide.md` may later describe
  the pre-game seeker undeploy list and the mine remove-and-return behavior. No
  rules-document edit.

## Dependency Ordering

1. `PGS-01` first — without a working mine entry state, neither mine placement nor
   mine removal can be exercised, and it stabilizes the placement entry path that
   later subtasks build on.
2. `PGS-02` after `PGS-01` — it shares the `FBattleDisplay` lower-panel
   placement-draw surface with `PGS-01`'s entry work and must build on a stable
   placement state.
3. `PGS-03` after `PGS-02` — it changes `FTacticalGame` placement dispatch
   (`placeOrdnanceAtHex`) that `PGS-01` also touches; sequencing avoids
   overlapping edits to the same placement flow.
4. `PGS-04` last — it depends on `PGS-03` (additive seeker placement must exist so
   the list is the counterpart removal path) and shares the lower-panel layout
   region with `PGS-02`; it also finalizes the multi-ship mine removal semantics
   that build on `PGS-01`/`PGS-03`.

Parallelization note: Treat this plan as **serial**. Every subtask touches one or
more of `FTacticalGame`, `FBattleDisplay`, and `FBattleScreen`, and several share
the same placement-flow and lower-panel rendering helpers, so conservative
sequential execution is required. None are parallelizable.

## Carried-Over Items From seeker-missile-fixes-round3 Reviewer Report

The round 3 reviewer report
(`artifacts/seeker-missile-fixes-round3/reviewer/reviewer_report.md`, outcome
CONDITIONAL PASS) left two non-blocking follow-ups (its "Follow-up feature
requests for planning"). They are addressed here as follows. These are **not**
implementer subtasks: per the downstream role boundaries they are Tester-owned
test-suite hygiene/coverage and are dispatched by the Coordinator to the Tester
stage, not given to an Implementer (test files are not placed in any Implementer
allowed-files list).

1. **(Carry-A) Stale `[DEFECT]` test comment cleanup (round 3 lower-panel
   list-visibility subtask's WARNING).** In
   `tests/gui/TacticalGuiLiveTest.cpp` (~lines 1893–1900), the
   `drawSeekerActivation` behavioral test still has an inline comment and a
   `CPPUNIT_ASSERT_MESSAGE` that describe the now-fixed panel-height persistence
   as an active `[DEFECT: ...]`. Correct the comment and assertion message so they
   describe the persistence as the intended, now-correct behavior, while
   **preserving the existing passing assertion** (`heightAfterActivation >=
   heightBeforeActivation`) as a regression guard. This is a cosmetic,
   Tester-owned correction with no production-code or acceptance-criterion impact.
   The Coordinator should route it to the Tester stage of `PGS-02` (the subtask
   whose work is closest — lower-panel placement/selection rendering), or to any
   convenient Tester stage in this plan.

2. **(Carry-B) Seeker placement-undo behavioral coverage (round 3
   placement-regression subtask's NOTE) — superseded by `PGS-04`.** The original
   follow-up asked for a behavioral test
   of the pre-game `BS_PlaceSeekers` board-click placement-undo/removal path
   (click-to-place then remove), asserting seeker-launcher ammo restoration,
   inactive-seeker record removal, and placed-ordnance record removal. In this
   plan, `PGS-03` removes board-click seeker removal entirely (seekers stack), and
   `PGS-04` replaces it with the centered list-based undeploy. The behavioral
   tests required by `PGS-04` (undeploy-one returns one round of ammo, decrements
   the group count, removes the inactive-seeker record and placed-ordnance record)
   provide exactly the ammo-restoration / record-removal coverage the follow-up
   sought, on the new removal mechanism. No separate test for the removed
   board-click undo path is created; the obsolete path no longer exists.

## Overall Acceptance Criteria

1. During pre-game mine placement, clicking a hex places a minefield (ammo
   decrement, `m_minedHexList` insertion, placed-ordnance record, board
   highlight); clicking it again removes it and returns the mine to the ship that
   placed it, regardless of which source row is currently selected.
2. During pre-game seeker placement, each hex click adds one seeker (stacking),
   ammo decrements per added seeker, and no seeker is removed by a board click.
3. When more than one ship has the relevant ordnance, the player can select which
   ship deploys for both the mine and seeker phases; with one ship it is
   auto-selected.
4. A centered lower-panel list during seeker placement shows placed seekers
   grouped by (hex + source ship) with counts and lets the player undeploy one
   seeker per click, returning it to its ship.
5. The game records the placing ship for every placed mine and seeker, and all
   removals/undeploys return ammo to the correct ship.
6. New behavioral coverage exercises the real mine entry sequence and fails
   against the current regression, and behavioral coverage exists for additive
   seeker stacking and seeker undeploy-by-list.
7. The carried-over round 3 follow-ups are resolved: the stale `[DEFECT]`
   comment/assert message in `tests/gui/TacticalGuiLiveTest.cpp` (~1893–1900) is
   corrected while preserving its passing regression assertion (Carry-A), and the
   seeker placement-undo coverage follow-up is satisfied by the `PGS-04` undeploy
   behavioral tests (Carry-B).
8. No change edits `doc/rules/tactical_operations_manual.md`.

## Documentation Impact

Documentation impact is low-to-moderate and concentrated in header Doxygen
comments for the modified tactical placement-flow (`FTacticalGame`), lower-panel
HUD rendering/selection (`FBattleDisplay`), and delegation (`FBattleScreen`)
helpers, plus any new model accessor/recall method and new list-render/click
helpers. The Documenter stage may update `doc/UsersGuide.md` to describe pre-game
mine toggle placement, seeker stacking, multi-ship source selection, and the
centered seeker undeploy list, and may update `doc/DesignNotes.md` if it tracks
tactical UI behavior. `doc/rules/tactical_operations_manual.md` must not be
edited.

## Implementer Prompts

### PGS-01 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`

Task to implement:
Restore pre-game mine board placement. The mine phase is entered from
`FBattleDisplay::onSetSpeed()`: after the defender's last ship speed is set and
`getDone()` is true, it calls `beginMinePlacement()` (which selects the mine
ship+weapon into `m_curShip`/`m_curWeapon`) and then unconditionally calls
`m_parent->setShip(NULL)` (around `src/tactical/FBattleDisplay.cpp:907`), nulling
`m_curShip`. Because `FTacticalGame::placeMineAtHex()` returns false when
`m_curShip == NULL`, every mine board click is silently dropped, while seekers
work because the seeker phase is entered via a different path that does not null
the ship. Fix the entry so that once `BS_PlaceMines` is active the selected mine
ship and mine launcher remain set when the first board click arrives (for
example, do not null the current ship after a successful `beginMinePlacement()`,
while preserving the existing null-on-failure / non-mine branches). Also harden
`beginOrdnancePlacement()` so it only enters `BS_PlaceMines` when a deployable
mine source is successfully selected (check the result of
`selectPlacementSourceByIndex()`), rather than entering the state unconditionally.
Do not change seeker placement behavior, mine removal semantics, or
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- When the game enters the mine placement phase through the real flow
  (`onSetSpeed` → `beginMinePlacement`), `m_curShip` and `m_curWeapon` reference
  the selected mine ship and its mine launcher (not null) when the first board
  click arrives.
- During pre-game `BS_PlaceMines`, clicking a valid in-bounds hex decrements the
  selected source's mine launcher ammo by one, inserts the hex into
  `m_minedHexList`, and appends a record to `m_placedOrdnance`.
- Placed mines are highlighted on the board during the mine phase.
- `beginOrdnancePlacement()` only enters `BS_PlaceMines` when a deployable mine
  source is successfully selected.
- A behavioral (not source-text-match) test reproduces the real entry sequence —
  either by driving the GUI `onSetSpeed`/mine-entry path or by replicating
  `beginMinePlacement()` followed by the GUI's `setShip(NULL)` before the click —
  and asserts that a mine is recorded; this test fails against the current
  (unfixed) code and passes after the fix.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- A GUI-entry behavioral test belongs in `tests/gui/TacticalGuiLiveTest.*`
  (driving the real `onSetSpeed`/mine-entry path); a model-level reproduction of
  the entry sequence belongs in `tests/tactical/FTacticalGameMechanicsTest.*`.
  Note that the existing `testPreGameOrdnancePlacementRecordingBehavior` does not
  reproduce this regression because it enters mine placement at the model level
  only.

Artifact guidance:
Write implementation artifacts under
`artifacts/pregame-ordnance-placement/PGS-01/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### PGS-02 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Make the source/ship-selection rows reliably visible and clickable during both
`BS_PlaceMines` and `BS_PlaceSeekers` so the player can select which ship deploys
when more than one ship has the relevant ordnance. The selection rows are
rendered by `drawPlaceMines()` and `drawPlaceSeekers()` (capturing click regions
in `m_shipNameRegions` / `m_shipSelectionSourceIndices`), and clicks are routed
through `FBattleDisplay::onLeftUp()` → `checkShipSelection()` →
`selectPlacementSourceByIndex()`. Investigate why the rows are not reliably
visible/clickable in practice (row placement region, panel height/clipping, or
click-region/draw-position misalignment) and fix it so each rendered row's click
region matches its drawn position and the rows are not clipped or drawn behind
the ship-status display. Do not change the underlying placement, ammo, or model
selection logic beyond what is needed to make row selection usable.

Acceptance criteria:
- During `BS_PlaceMines`, every ship with a deployable mine launcher appears as a
  selectable source row; during `BS_PlaceSeekers`, every ship with a deployable
  seeker launcher appears as a selectable source row.
- Clicking a source row selects that ship+weapon (updates the selected-source
  index and `m_curShip`/`m_curWeapon`) and the selected row is visibly indicated.
- Each row's click region aligns with its drawn position; rows are not clipped or
  rendered behind the ship-status display.
- After selecting a different source, the next board click places ordnance from
  the newly selected ship/weapon.
- With exactly one eligible ship for the phase, that source is auto-selected and
  placement works without a manual row click.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Live selection-row visibility/click behavior belongs in
  `tests/gui/TacticalGuiLiveTest.*` (offscreen `wxMemoryDC` plus the
  `WXGuiTestHarness` helpers); source-contract coverage for the layout helpers
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/pregame-ordnance-placement/PGS-02/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### PGS-03 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`

Task to implement:
Make pre-game seeker placement additive (stacking) and remove board-click
removal for seekers. Currently `placeOrdnanceAtHex()` always attempts
`removePlacedOrdnanceForSelection()` first, so a second click on the same
hex+source removes the seeker (toggle). Change the dispatch so that during
pre-game `BS_PlaceSeekers` a board click always adds one seeker via
`placeSeekerFromSelection()` (which is already additive), never removing on a
board click. Preserve mine toggle behavior for `BS_PlaceMines` (clicking a mined
hex still removes the mine) and do not change attack-phase / `BS_Battle` seeker
or mine behavior. Do not edit `doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During pre-game `BS_PlaceSeekers`, clicking a hex always adds one inactive
  seeker for the selected source: seeker launcher ammo decrements by one, a new
  inactive seeker record is created, and a placed-ordnance record is appended —
  on every click, including repeated clicks on the same hex.
- Repeated clicks on the same hex by the same source accumulate multiple seekers
  in that hex; no toggle/removal occurs from the board during seeker placement.
- Mine placement retains toggle behavior (clicking a mined hex removes the mine)
  and attack-phase seeker/mine behavior is unchanged.
- Seeker placement stops adding when the selected source's launcher ammo is
  exhausted.
- Add a behavioral (not source-text-match) model test that enters seeker
  placement, clicks the same hex multiple times for one source, and asserts the
  seeker count and ammo decrement increase per click (no removal); this test must
  fail against the current toggle behavior and pass after the fix.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Additive seeker-placement model coverage belongs in
  `tests/tactical/FTacticalGameMechanicsTest.*` or a focused tactical placement
  fixture under `tests/tactical/`.

Artifact guidance:
Write implementation artifacts under
`artifacts/pregame-ordnance-placement/PGS-03/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### PGS-04 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleDisplay.h`

Task to implement:
Add a centered lower-panel placed-seeker undeploy list for the pre-game
`BS_PlaceSeekers` phase, and finalize the multi-ship mine removal semantics.

Seeker list (model + HUD): Expose a read-only model accessor on `FTacticalGame`
that returns placed inactive seekers grouped by (hex + source ship) with counts,
and a recall method that undeploys exactly one seeker for a given (hex, source),
removing the matching inactive seeker record and its placed-ordnance record and
returning one round of ammo to that ship's seeker launcher. Forward both through
`FBattleScreen`. In `FBattleDisplay`, render a list in the center of the lower
panel during `BS_PlaceSeekers` (mirroring the existing
`drawOffensiveSeekerPendingRows()` / `checkOffensiveSeekerPendingSelection()`
pattern, but reading the new pre-game accessor), one row per (hex, source ship)
with the source ship, hex, and count; route clicks on those rows (via
`onLeftUp()` for `BS_PlaceSeekers`) to the new recall method. Ensure the list
auto-expands the panel height when needed (consistent with existing lower-panel
list handling) and does not overlap the source-selection rows or the ship-status
display.

Mine removal: Make pre-game mine removal work regardless of the currently
selected source — clicking a mined hex removes that minefield by matching any
placed mine record at that hex (not only one whose source matches the current
selection) and returns the mine to the ship that placed it (restore ammo to the
recorded placing source), erasing the hex from `m_minedHexList` and removing the
placed-ordnance record. Do not change attack-phase behavior and do not edit
`doc/rules/tactical_operations_manual.md`.

Acceptance criteria:
- During `BS_PlaceSeekers`, a centered lower-panel list shows one row per (hex,
  source ship) with at least one placed inactive seeker, showing the source ship,
  hex, and count, and updates after each placement and undeploy.
- Clicking a list row undeploys exactly one seeker for that (hex, source ship):
  the inactive seeker record and its placed-ordnance record are removed and one
  round of ammo is returned to that ship's seeker launcher; the board highlight
  reflects the remaining seekers.
- During `BS_PlaceMines`, clicking a mined hex removes that minefield regardless
  of which source row is selected and returns the mine to the placing ship (ammo
  restored to the recorded placing source); the hex is removed from
  `m_minedHexList` and the placed-ordnance record is removed.
- The centered seeker list does not appear during the mine phase and does not
  overlap the source-selection rows or ship-status display.
- Add behavioral (not source-text-match) model tests for: (a) undeploy-one from a
  stacked (hex, source) returns one round of ammo and decrements the group count
  while leaving the rest; and (b) mine removal with a non-placing source selected
  still removes the mine and restores ammo to the placing ship.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build the GUI test module with `cd tests/gui && make`; if a display or xvfb is
  available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Model accessor / recall and mine-removal behavioral coverage belongs in
  `tests/tactical/FTacticalGameMechanicsTest.*` or a focused tactical placement
  fixture under `tests/tactical/`.
- Centered-list render/click coverage belongs in
  `tests/gui/TacticalGuiLiveTest.*`; source-contract coverage for the list helper
  belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.

Artifact guidance:
Write implementation artifacts under
`artifacts/pregame-ordnance-placement/PGS-04/` using the active implementer role
conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.
