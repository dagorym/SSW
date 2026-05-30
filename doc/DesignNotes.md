# Design Notes

## Combat Game Entry

Upon entering the combat game you have the following information:

1. A list of the attacking ships
2. A list of the defending ships
3. A flag indicating whether or not there is a planet (this could
   eventually be replaced by a planet object pointer that would have
   information about the planet such as the name and icon to use)
4. A pointer to the station object if there is a station orbiting the
   planet. This parameter is null if there is no station.

At any given point in the control flow, you need to know who the active
player is. You also need to know which player was the last moving player
so you can draw the paths of their ships. You need some mechanism to
track the ships' paths, locations, number of turns, speed and heading.
— **Done**: the live wx flow still keeps high-level dialog/control flow in
`FBattleScreen`, but the canonical tactical battle state now lives in
`FTacticalGame`. That model owns movement bookkeeping (`FTacticalTurnData`),
hex occupancy (`FTacticalHexData`), tactical report state, and the
renderer-facing accessors `FBattleScreen` forwards to the wx layer.
Milestone 8 Subtask 3 narrows `FBattleBoard` to geometry caching,
scrolling, rendering, and hit-testing; it no longer owns duplicate tactical
mechanics containers and now forwards clicked hexes through
`FBattleScreen::handleHexClick()` into `FTacticalGame`.

## Combat

A. During the attacking player's movement phase, you have to be able
   to select a MP ship and see its projected path. You should be able
   to select points along the path and have options to turn at the
   points selected. You should be able to back out any of your moves
   before committing the final movements. In this phase you have to
   know who the moving player is and that it is the same as the active
   player. You need a structure to keep track of all the movements. At
   the end of this phase the ships' speed and heading should be updated
   to reflect the final move position. The active player designation
   should be toggled but the moving player shouldn't be. — **Done**.

B. During the defensive fire phase, you should still draw the paths of
   the moving player's ships. The structure used to hold the movement
   should still be associated with the moving player and not the active
   player. The active player should be able to select his ships, select
   a weapon from that ship and then select a target ship from the
   enemy's ships to fire at. The game should compute the closest
   approach of the enemy's ship to the firing ship to validate that
   the ship was in range of the weapon and if necessary if it was in
   the field of fire for FF weapons. You should be able to change your
   weapons assignment before declaring that all assignments are done.
   Once this phase is over the weapons fire should be resolved and all
   damage applied to the target ships. Any destroyed ships should be
   removed from the game. The active player should switch back to the
   moving player so that they may fire their weapons. — **Done**.

C. During the offensive fire phase, the moving player now has the
   opportunity to fire its weapons at the non-moving player. Again,
   the player selects the ships and then the weapon and then the
   target. The moving player can fire its weapons from any point
   along its movement and the system has to calculate the optimal
   location to fire the selected weapon at the selected target or tell
   the player that it may not fire at that vessel. When selecting a
   moving vessel's weapon, the system should calculate every hex that
   the vessel can fire into during its move and highlight these
   hexes, storing them in a list of some sort. When the player attempts
   to select a target, it must be within the area defined. Target
   selections should be allowed to be changed until all weapon
   assignments have been completed and the player signifies that the
   turn is over. At this point the system should calculate the damage
   effects and apply the damage to the non-moving player's ships.
   — **Done**.

D. At the end of any firing phase, the target ships with ICMs should
   have the opportunity to declare how many ICMs they want to use in
   defense against incoming rocket weapons or mines. — **Done**
   (`fireICM()` / `ICMSelectionGUI`).

E. At the end of the moving player's fire phase, the movement data
   shall be cleared, the moving player marker will switch to the other
   player and the active player will switch to the new moving player.
   At which point the process will repeat but with the roles switched.
   — **Done** (`toggleActivePlayer()`, `toggleMovingPlayer()`, phase
   transitions in `FBattleScreen::setPhase()`).

## Keeping State

1. We need a Point class/structure that holds x, y data. This can be used
   to store the physical positions (i.e. centers) of the hexes for
   drawing on the screen or pairs of coordinates that correspond to the
   hex numbers on the map. This should be contained by the hexData
   structure and all the places where the hex data structure is being
   used in where the vector of ship positions is not needed can be
   replaced by this structure. — **Done** (`FPoint` in `include/core/FPoint.h`;
   `hexData` struct in `FBattleBoard.h` contains `FPoint pos` for hex centres.)

2. Instead of keeping track of the side as a boolean, maybe we should
   have two variables, `m_activePlayer` and `m_movingPlayer`, that store
   the ID of the player in question. All data structures will be
   referenced by the ID values stored here. This allows for expansion
   of the game from just two sides to multiple sides if
   necessary/desired. — **Partial**: `m_activePlayer` and `m_movingPlayer`
   are still `bool` in `FBattleScreen.h`; however `getActivePlayerID()` and
   `getMovingPlayerID()` map those booleans to real player IDs, so external
   code always works with IDs. The internal storage remains boolean.

3. All of the calls to `getSide()` should be replaced by `getActivePlayer()`.
   — **Done** (no remaining `getSide()` calls found in the codebase).

4. Lookups should use the ID to refer to and find the relevant data
   structures. — **Done** (ID-based lookups used throughout `FBattleBoard`
   and `FBattleDisplay`).

5. Likewise, when we need to access information about the moving
   player, the ID can be obtained by querying `getMovingPlayerID()`. The
   returned ID can then be used to access the relevant data
   structures. — **Done** (`getMovingPlayerID()` exists in `FBattleScreen.h`
   and is used throughout `FBattleDisplay.cpp` and `FBattleBoard.cpp`).

6. Data on ships needs to be indexable by the ship's ID value (i.e.
   the value returned by `getID()`). — **Done**.

7. For each moving ship, a list of the hexes moved through should be kept to
   facilitate determining where that ship can fire and whether or not
   it can be fired upon. — **Done** (`m_movementHexes`, `m_leftHexes`, and
   `m_rightHexes` now live in `FTacticalGame`; `FBattleBoard` renders them via
   `FBattleScreen` forwarding accessors and `computePath()` /
   `computeMovedWeaponRange()` still iterate over that model-owned path for
   range/fire checks).

8. We need a mechanism to keep track of whether or not a ship has fired
   its weapons defensively in a round or not. (This really isn't
   needed until we have more than two sides.) — **Not implemented**; tracking
   is phase-based rather than per-ship. Acceptable for the current two-player
   system.

9. However, we do need a mechanism to keep track of which weapons are
   firing at which ships. Should this be part of the weapon class?
   Probably, each weapon should keep track of the ship it is firing at
   as well. — **Done**: `FWeapon` holds `m_target` (`FVehicle*`) and
   `m_targetRange`; `FVehicle` is forward-declared in `FWeapon.h` to avoid
   circular dependencies.

10. We need a structure to keep track of which ships are where. Should
    ships know their position? Probably not, since this is related
    to the battle field implementation more than the ship itself. — **Done**:
    `FVehicle` has no position members; positions are stored in
    `FTacticalHexData m_hexData[100][100]` inside `FTacticalGame`, where each
    cell holds a `FPoint pos` (hex centre) and a `VehicleList ships` (ships
    present). `FBattleBoard` now reads occupants through `FBattleScreen`
    forwarding methods instead of owning a duplicate occupancy map.

11. Thinking on this more I believe we may want to use a `std::map`
    structure to hold the actual battle map. The key will be a Point
    object that holds the array coordinate of the hex. The data member
    held would be a hexData structure containing the list of ships and
    the coordinates of the hex centers. This would arbitrarily allow us
    to expand the map as needed during the course of a battle if ships
    needed to move off the edge of what is currently displayed. It might
    even be able to only store hexes that ships actually are in and
    add more as the game goes and they travel through new hexes. —
    **Not implemented**: the battle map is still a fixed
    `FTacticalHexData m_hexData[100][100]` array in `FTacticalGame`. A
    `std::map<FPoint,int>` is used for gravity turns and `FHexMap` is used for
    mine targets, but the main tactical occupancy map remains a fixed array.
    This refactor is still outstanding.

## Adding Defenses

1. Ships will need a `m_defenses` vector of `FDefense` pointers. — **Done**
   (`DefenseList m_defenses` in `FVehicle.h`; seven concrete types: `FNone`,
   `FReflectiveHull`, `FMaskingScreen`, `FICM`, `FStasisScreen`,
   `FProtonScreen`, `FElectronScreen`).

2. The first defense in the vector should be the default (i.e. None or
   Reflective Hull). — **Done**.

3. Add display of list of defenses. — **Done** (`drawDefenseList()` in
   `FBattleDisplay.cpp`).

4. Defense names should be green for the active defense, white for
   inactive defenses and red for damaged defenses. — **Done** (green = active,
   white = available, red = damaged, orange = unavailable due to power damage).

5. The defense list should be active on your moving turn only for
   changing the active defense. — **Done** (`checkDefenseSelection()` in
   `FBattleDisplay.cpp`).

6. To change the defense, simply click on the one you want (clicking on
   an active defense will turn it off). — **Done**.

7. ICMs shouldn't be active but when fired upon give an option of
   how many to use. — **Done** (`ICMSelectionGUI` dialog; triggered in
   `FBattleBoard.cpp` when weapons target a ship carrying ICMs).

8. When clicking a Masking Screen defense, confirm. This defense may
   be held off for now as it may be a bit tricky to do right. — **Done**:
   Masking Screen is fully implemented with 2-charge ammo, turn counting
   (`m_maskingScreenTurnCount`), and auto-clear when the count reaches zero.

9. Ships all get a `m_currentDefense` pointer and a `getCurrentDefense()`
   method that points to and returns the currently selected defense.
   This should be added to the `FVehicle` base class. — **Done**
   (`m_currentDefense` and `getCurrentDefense()` / `setCurrentDefense()` in
   `FVehicle`).

10. When firing a weapon, the weapon should query the target for the
    to-hit modifier of its current defense for the attacking weapon type
    and apply it to the to-hit roll. — **Done** (`FWeapon::attack()` calls
    `getCurrentDefense()->getAttackModifier(m_type)` on the target; special
    case for laser weapons dealing half damage against a Masking Screen).

11. When firing at a ship with ICMs and a functioning launcher, the
    defending player should be prompted to select the number of ICMs
    to use in defense for that attack. — **Done** (see item 7 above).

12. It'd be nice to be able to show the player all the attacks against
    that ship so they can ration as needed. How do we implement this?
    — **Partially addressed** via `ICMSelectionGUI` which shows nearby ships
    and incoming weapons; full ration-across-all-attacks display not yet done.

13. Masking screens are deployed at the start/end/middle? of round. —
    **Done**: activated by clicking the defense during the moving player's
    turn; charges deducted via `setCurrentDefense()`.

14. A Masking Screen needs to be cleared (i.e. turned off) if a turn is
    made during that round's movement phase. — **Done**: `decrementMSTurnCount()`
    is called each turn from `FBattleBoard.cpp`; the screen reverts to `None`
    when the counter reaches zero. Orbit handling uses a fixed turn count of
    5 for station types.

15. What visual cues do we use for the screens? The masking screen has a
    marker that we can use but the proton, electron and stasis screens
    don't. Maybe a blue circle for the proton, green for the electron
    and yellow for the stasis screen would work — put the letter (P,
    E, or S) in the middle. Add this as an overlay on the image of the
    ship. — **Partially done**: Masking Screen replaces the ship icon with
    `MaskingScreen.png`. **No circle overlays exist yet for Proton, Electron,
    or Stasis screens** — the P/E/S letter-in-circle idea is still outstanding.

## Advanced Damage Table

1. Initially just add this to the `FVehicle` class `takeDamage()` method.
   We can adapt it later. — **Done**: `FVehicle::takeDamage(int damage,
   int damageMod, bool basic, FTacticalDamageResolution*)` in `FVehicle.cpp`;
   the `basic` flag routes to hull-only damage or the full advanced table.

2. Maybe what we should do is make a `takeBasicDamage()` method that
   keeps the basic behavior. — **Not implemented as a separate method**:
   basic damage is handled by passing `basic=true` to `takeDamage()` instead.

3. The basis for the table can be lifted directly from the KHVector
   Obiter code as a lot of this was implemented there, at least the
   form of it. — **Done**: `FVehicle::advancedDamage()` implements the full
   d100 Knight Hawks damage table with double hull, ADF/MR loss, weapon hits
   (by priority list), defense hits, power system damage, combat control
   damage, navigation errors, electrical fire, disastrous fire, and DCR loss.
   Outcomes are recorded in `FTacticalDamageResolution`.

## Combat Phases

All three combat phases and their supporting mechanics are fully implemented:

- **Movement phase** (`PH_MOVE`): path projection via `computePath()`; hex
  highlighting for forward movement, left turns, and right turns stored in
  model-owned `m_movementHexes`, `m_leftHexes`, `m_rightHexes` on
  `FTacticalGame` and rendered by `FBattleBoard` through `FBattleScreen`
  accessors.
- **Seeker activation pre-phase** (`PH_SEEKER_ACTIVATION`): `FTacticalGame`
  now intercepts `setPhase(PH_MOVE)` and routes movement entry through
  `beginSeekerActivationPhase()` first. The model exposes one selected
  inactive-seeker stack hex plus filtered stack records and one-way activation
  helpers so later wx surfaces can activate individual seekers before movement.
  If the moving player has no inactive seekers, the model still calls
  `resolveActiveSeekersForMovingPlayer()` and then auto-enters `PH_MOVE`, so
  the active-seeker seam runs even when no visible activation stop is needed.
  The shipped TSM-005 wx follow-up makes that stop user-visible: the board now
  renders only the moving player's inactive seeker stacks during
  `PH_SEEKER_ACTIVATION`, board clicks select one activation hex stack, and the
  lower panel renders instructional text plus one clickable row per inactive
  seeker in the selected stack together with a `Seeker Activation Done` action
  button. Once tactical play leaves that phase, normal battle rendering hides
  inactive seekers again and shows only active seekers on the map. TSM-008
  extends activation completion so `completeSeekerActivationPhase()` resolves
  the moving player's active seekers before `PH_MOVE`: same-hex contacts are
  captured first, movement-step contacts append pending
  `FTacticalSeekerContactOutcome` records for downstream damage work, seekers
  that contact are removed, and seekers that reach their 12-hex allowance
  without contact expire.
- **Defensive fire phase** (`PH_DEFENSE_FIRE`): closest-approach range
  checking in `setIfValidTarget()`; FF weapon arc validation in
  `computeFFRange()`.
- **Offensive fire phase** (`PH_ATTACK_FIRE`): moving ship fires from the
  optimal point along its path via `computeMovedWeaponRange()`; iterates
  every hex in the ship's path and checks weapon range/arc at each position.
  Selecting an `SM` launcher during the moving player's offensive fire phase
  now enters seeker deployment mode instead of normal target assignment:
  legal board clicks are limited to the selected ship's current path, each
  click adds one inactive pending seeker for that launcher, same-hex clicks
  keep stacking more pending seekers if ammo remains, and the lower panel
  exposes explicit grouped recall rows until `Offensive Fire Done` commits the
  surviving pending deployments as ordinary inactive seekers.
- **ICM interception**: `fireICM()` in `FBattleScreen.cpp` presents
  `ICMSelectionGUI` before weapon damage is resolved.
- **End-of-phase cleanup**: `toggleActivePlayer()` and `toggleMovingPlayer()`
  in `FBattleScreen`; masking screen turn count decremented via
  `decrementMSTurnCount()` each movement phase.

Milestone 5 also adds an additive `FTacticalGame` implementation that mirrors
these mechanics without rewiring the runtime wx flow yet. The new model owns
safe-default reset/setup helpers, movement reset/finalization helpers,
tactical report lifecycle built on `FTacticalCombatReport`, `fireAllWeapons()`
combat report aggregation/cleanup, and winner/combat-end detection so later
migration milestones can delegate to it incrementally.

The raw `FTacticalReportEvent` payload now also preserves structured damage
metadata from both attack-generated internal events and standalone immediate
damage-resolution events. The two live conversion seams in
`src/tactical/FTacticalGame.cpp`—`buildTacticalAttackEvent(...)` for attack
effects and `appendTacticalDamageResolutionEvents(...)` for immediate
resolution events—both copy `previousValue`, `newValue`, and `amount` into the
report event so the runtime tactical flow retains exact numeric state changes.
Weapon-hit effects carry `damageEffectType`, `damagedWeaponType`,
`damagedWeaponID`, and `damagedWeaponName`, while defense-hit effects also
preserve `damagedDefenseType` and `damagedDefenseName`. Quantitative effect
events therefore retain `amount`, `previousValue`, and `newValue` so later
summary formatting can use numeric state changes instead of re-parsing label
text. The player-facing ship-summary
rollup consumes that structured defense identity to emit abbreviated defense
wording in the form `Defense Hit: <abbr-list>` (for example `Defense Hit: MS,
PS`) instead of depending on long-form label/detail text. Repeated weapon hits,
defense hits, and other non-weapon effects remain separate event entries in the
same report path, and hit-detail text suppresses only the redundant lowercase
placeholder note `Attack hit target` while preserving meaningful notes.

The summary rollup now consumes that structured weapon metadata to emit one
player-facing weapon effect entry per ship in the form `Weapon Hit:
<abbr-list>`, preserving event order and duplicates such as `Weapon Hit: LB,
LB, AR`. Each ship rollup is model-owned and now renders as a `<Ship Name>:`
header followed by one ` - ...` bullet per aggregate summary item, so grouped
weapon, defense, and other effect wording no longer has to share a single flat
display line. `TacticalDamageSummaryGUI` continues to render the prebuilt
`FTacticalShipReportSummary::displayLines` without inspecting raw attack or
event collections itself. ADF-loss and MR-loss events now use the same
structured payload to accumulate exact point-loss totals per summarized ship
scope and emit dedicated bullets such as ` - ADF (-3)` and ` - MR (-1)`,
rather than echoing label-only text like `ADF reduced` or `MR reduced`.

Milestone 6 adds the tactical UI boundary alongside that model-only work:
`ITacticalUI` now defines the non-wx tactical callback surface and
`WXTacticalUI` provides the wx-backed adapter for redraw requests,
informational messaging, the tactical damage summary dialog, ICM selection,
and winner notification semantics. This remains additive infrastructure only;
the live tactical wx runtime still flows through `FBattleScreen`,
`FBattleBoard`, and `FBattleDisplay` until Milestone 7 rewires delegation.

Milestone 7 Subtask 1 extends `FTacticalGame` further as the canonical
additive tactical state-owner surface for upcoming `FBattleScreen`
delegation. Newly exposed API categories now cover:

- **State/control:** installed UI access (`installUI(ITacticalUI*)`,
  `getUI()`), control-mode toggles, and done-state tracking.
- **Setup/scenario:** planet choice, planet/station placement, station pointer,
  and direct access to the attacker/defender fleet and ship lists established
  during setup.
- **Ship/weapon selection:** current ship and weapon getters/setters remain on
  the model surface so UI callers can forward selection state instead of
  owning duplicate members.
- **Movement-state helpers:** previously added movement reset/finalization
  helpers continue to live in the model beside the newer state accessors,
  keeping tactical bookkeeping wx-free.
- **Tactical report access:** callers can continue to build, append, clear, and
  inspect `FTacticalCombatReport` state from the model layer.

The UI seam remains additive: `FTacticalGame` only stores an `ITacticalUI*`
through `installUI()` / `getUI()` and still introduces no wx headers or wx
types. This unblocks Milestone 7 delegation work by giving `FBattleScreen` a
stable forwarding surface for existing runtime state while preserving current
behavior until later subtasks move the active tactical wx path over to that
model.

Milestone 7 Subtask 2 keeps that migration additive but moves ownership
responsibility into `FBattleScreen` itself. The tactical top-level now
constructs and owns its `FTacticalGame*` and `WXTacticalUI*`, installs the wx
adapter onto the model during setup, detaches it with `installUI(NULL)` during
teardown, and then deletes both delegated runtime objects safely. The guarded
close-path gate now lives on `FBattleScreen` itself instead of in
`FTacticalGame`, keeping UI lifecycle state on the screen surface. The later
battle-board menu-bar follow-up keeps that ownership model intact
while moving `FBattleScreen` from `wxDialog` to `wxFrame`, so the tactical
window can expose native frame features such as `SetMenuBar(...)` /
`GetMenuBar()` without breaking the established blocking `ShowModal()` launch
sites. `FBattleScreen` now provides its own frame-backed `ShowModal()`,
`EndModal(...)`, `IsModal()`, and return-code shim so stack-owned BattleSim and
strategic launch paths still block until the battle screen closes, while
heap-owned non-modal callers continue to close through `closeBattleScreen(...)`
without terminating the app. The tactical `Makefile` change remains limited to
the new `FBattleScreen` header dependencies needed for that wiring.

Milestone 8 Subtask 1 extends that additive delegation surface again, but only
on the model and `FBattleScreen` seam. `FTacticalGame` now exposes the
non-wx interaction APIs that later `FBattleDisplay` and `FBattleBoard` work
need in order to stop owning tactical mechanics directly. The newly validated
surface now includes:

- **Selection APIs:** model-side weapon selection, defense selection, ship
  selection from a clicked hex, and target assignment from a clicked hex.
- **Setup and placement APIs:** planet placement, station placement, ship
  placement, pending-rotation heading confirmation, and setup-oriented
  state access needed during scenario initialization.
- **Movement and fire progression APIs:** hex-click dispatch, generalized
  ordnance-placement start/source-selection/placement helpers (with legacy
  mine-placement compatibility entry points), move completion,
  defensive/offensive fire phase completion, and model-owned weapon-range
  computation.
- **Renderer-facing state accessors:** read access to hex occupancy, current
  selected ship hex, route and turn highlight lists, target and head-on range
  sets, mined hexes, mine targets/ownership, mine-capable ship lists, turn
  bookkeeping, and simple in-bounds / occupied checks.

`FBattleScreen` now forwards those calls directly to `FTacticalGame` and
requests redraws when the forwarded operation mutates tactical state. That
model/screen forwarding layer was the first Milestone 8 compatibility step.

Milestone 8 Subtask 2 then converts `FBattleDisplay` into a tactical HUD/view
translator for the flows already exposed on `FBattleScreen` / `FTacticalGame`.
The panel now reads battle state through `m_parent->getState()`,
`m_parent->getControlState()`, and `m_parent->getPhase()` during draw/paint,
removes its local `fireAllWeapons()` helper entirely, and delegates the
remaining non-view interactions through the existing forwarding seam:

- **Weapon/defense selection:** click handling now calls
  `FBattleScreen::selectWeapon()` / `selectDefense()` instead of mutating the
  selected ship directly from `FBattleDisplay`.
- **Fire-phase resolution (shipped Subtask 2 state):** defensive and offensive
  fire completion delegated through `resolveCurrentFirePhase()`, shared tactical
  summary UI, and `FBattleScreen::clearDestroyedShips()` rather than local
  `FBattleDisplay` fire-resolution logic. However, the shipped Milestone 8 build
  still had a blocking runtime cleanup-order bug in this seam; the explicit
  single-clear lifecycle contract was added later by Milestone 8 remediation
  Subtasks 1/2/4.
- **Mine/ordnance placement setup:** setup-speed completion still reaches the
  legacy `beginMinePlacement()` seam, but that compatibility entry point now
  delegates to generalized source-tracked setup placement through
  `beginOrdnancePlacement()`. The display can also forward exact source
  selection through `selectPlacementSource(...)` or
  `selectPlacementSourceByIndex(...)`, and map clicks now have a generalized
  `placeOrdnanceAtHex()` path behind the legacy `placeMineAtHex()` wrapper.
  Setup placement undo is keyed to the exact selected ship/weapon slot rather
  than only ship ownership, and successful mine/seeker placement rebuilds the
  deployable-source list while reselecting that same source slot so wx-side row
  indices and model-side ship/weapon pointers stay aligned after ammo changes.
  The shipped TSM-003 wx follow-up now renders one lower-panel row per
  deployable ship/weapon slot, labels each row with the exact mine or seeker
  source plus its live ammo count, switches the active source through row
  clicks, and keeps the visible row state synchronized with map placement/undo
  rather than relying on the legacy `getShipsWithMines()` list alone.
  `FBattleBoard` also reads `getPlacedOrdnance()` during setup placement and
  shades placed mines and inactive seekers with deterministic source-specific
  colors derived from per-source ordinals rather than the old 12-color modulo
  wrap, so same-hex markers remain visually attributable to their launching
  slot even when many deployable rows are present.

Milestone 8 Subtask 3 then applies that forwarding surface to
`FBattleBoard`. The board now keeps only wx-side geometry, scaling,
scrolling, drawing, and hex hit-testing helpers (`m_hexCenters`,
`getHex()`, `drawGrid()`, and related rendering code). Tactical mechanics data
such as occupancy, turn bookkeeping, movement highlights, target/head-on
ranges, and mined hex overlays are read from `FTacticalGame` through
`FBattleScreen` pass-through accessors during rendering. Hex clicks are now
handled by computing the clicked board hex in `FBattleBoard::onLeftUp()` and
forwarding that `FPoint` to `FBattleScreen::handleHexClick()`, which delegates
the behavior to `FTacticalGame::handleHexClick()`.

The shipped TSM-004 follow-up extends that forwarding seam again for the new
pre-movement seeker-activation model state. `FBattleScreen` now forwards
inactive-stack discovery, activation-hex selection, selected-stack inspection,
one-way individual seeker activation, and activation completion so the later
board/display subtasks can render and drive the activation UI without
reintroducing wx state ownership into `FTacticalGame`.

TSM-008 keeps that boundary intact while making activation completion
behaviorful: `FBattleScreen::completeSeekerActivationPhase()` still remains a
thin forwarding seam, but the delegated `FTacticalGame` path now performs the
moving-side active seeker resolution pass, records pending contact outcomes for
later damage handling, expires 12-hex seekers, and only then enters ordinary
movement and redraw.

TSM-009 completes that detonation handoff. Pending
`FTacticalSeekerContactOutcome` records now resolve through
`FTacticalGame::resolvePendingSeekerDetonationDamage()`, which creates
temporary parentless `FWeapon::SM` attacks, reuses
`ITacticalUI::runICMSelection(...)`, batches the resulting attacks into one
immediate `TRT_SeekerDamage` summary, and captures destroyed-ship IDs only
after the summary dialog path finishes. `FBattleScreen` then consumes that
destroyed-ship bookkeeping before its final redraw, preserving the same
summary-before-cleanup ordering used by the existing immediate mine-damage
flow.

TSM-010 integrates active seeker path contact into normal ship movement
finalization. `FTacticalGame::completeMovePhase()` now clears any leftover
`m_pendingSeekerContactOutcomes` at the start of finalization, then calls the
new `checkForActiveSeekersOnPath(FVehicle*)` for each moving ship alongside
the existing `checkForMines()` check. `checkForActiveSeekersOnPath` scans the
ship's finalized turn path and appends a `FTacticalSeekerContactOutcome` for
the first active opposing seeker encountered in each path hex; inactive seekers
are skipped via an explicit `active` flag guard so they never trigger movement
contact. After all ships are scanned, `completeMovePhase()` calls the new
`applyMovementSeekerDamage()` to resolve the collected contacts through the
same `resolvePendingSeekerDetonationDamage()` seam established by TSM-009,
removing each detonated seeker from `m_seekerMissiles` exactly once. Seeker
contact damage is therefore resolved before `applyMineDamage()` and before the
phase advances to `PH_DEFENSE_FIRE`.

The shipped TSM-005 wx follow-up consumes that forwarding seam. `FBattleBoard`
loads `icons/SeekerMissile.png` through `FGameConfig::resolveAssetPath(...)`
and applies phase-specific seeker visibility rules: only inactive stacks owned
by the moving player render during `PH_SEEKER_ACTIVATION`, while normal battle
phases render only active seekers. `FBattleBoard::onLeftUp()` also routes
activation-phase hex clicks to `FBattleScreen::selectSeekerActivationHex(...)`
instead of the generic tactical click handler. `FBattleDisplay` now adds the
activation instructions, selected-stack readout, per-seeker clickable rows, and
`Seeker Activation Done` button so board and lower-panel refresh stay aligned
after both activation-hex changes and one-way seeker activation clicks.

The shipped TSM-006 follow-up extends the same runtime delegation seam for
offensive-fire seeker deployment. `FTacticalGame` now tracks
`m_pendingOffensiveSeekerDeployments` keyed by `m_offensiveFirePhaseID`, exposes
`isOffensiveSeekerDeploymentMode()`,
`getSelectedOffensivePendingSeekerHexGroups()`, and
`recallSelectedOffensivePendingSeekerAtHex(...)`, and treats legal path-hex
clicks as additive pending seeker placement for the selected `SM` launcher.
`FBattleScreen` forwards those model APIs and `FBattleDisplay` renders grouped
per-hex recall rows, so same-hex repeat clicks keep adding pending seekers
while undo remains an explicit lower-panel action instead of a board-click
toggle.

SMC-06 extends `FTacticalSeekerMissileState` with a render-supporting
`movementPath` field (a `std::vector<FPoint>`) populated by
`resolveActiveSeekersForMovingPlayer()` as each greedy step is taken. Active
seekers for the moving player have their path cleared and then rebuilt from the
starting hex through every movement step; inactive or non-moving seekers have
their path cleared so no stale paths persist across turns. The field is not
persisted and carries no wx types. `FBattleScreen::getSeekerMissiles()` delegates
the complete seeker record collection (including `movementPath`) to the board
layer without copying.

SMC-07 builds on that path data to draw seeker movement on the board. During
`PH_MOVE`, `FBattleBoard::drawSeekerPaths()` iterates all seeker records with an
active flag and `movementPath.size() >= 2`, sets a cyan pen (color `#00CCCC`,
width 2, distinct from the ship-path red/gray tones), and draws stepped
`dc.DrawLine()` segments through each recorded hex center. Seeker icons rendered
by `drawSeekerMissiles()` during normal battle phases are now also rotated to
reflect the seeker's current heading (`heading * pi/3` radians), consistent with
the six-direction ship icon rotation used elsewhere on the board.

This leaves the originally shipped Milestone 8 runtime tactical wx path only
partially rewired: `FBattleDisplay` behaved as a wx renderer/input translator
for delegated fire/setup flows and `FBattleBoard` was narrowed to
renderer/hit-test responsibilities, but two blocking runtime seams remained.
First, fire cleanup ordering could clear destroyed-ship bookkeeping before wx
consumption. Second, the live move-done path still bypassed canonical
`FTacticalGame::completeMovePhase()` via `setPhase(PH_FINALIZE_MOVE)`.

These blocking runtime defects were remediated after Milestone 8 shipment.
They are distinct from the accepted non-blocking limitation that full tactical
GUI scenario playthrough still requires manual wx validation in addition to the
module's automated seam-level tests.

### Validation Completed

Milestone 5 production-fix validation confirmed that the additive
`FTacticalGame` model now removes destroyed ships from the active side list,
`m_hexData` occupancy, and `m_turnInfo` bookkeeping while keeping
`m_hasWinner`/`m_winnerID` consistent when a side is eliminated. The same
validation also confirmed that `fireICM()` clears any stale `m_ICMData`,
resolves target occupants by scanning `m_hexData`, and only queues actionable
interception entries with a non-null `vehicles` pointer for same-side defenders
that still have usable ICM capability. Runtime tactical wx rewiring remained
out of scope for this milestone.

Validation command:

```bash
cd tests/tactical && make -s && ./TacticalTests
```

Result: `OK (65 tests)`

Milestone 8 Subtask 1 validation then confirmed that the new model-side
interaction API surface and `FBattleScreen` forwarding layer compile and behave
as documented without introducing wx dependencies into `FTacticalGame`.

Validation commands:

```bash
make -C src/tactical clean && make -C src/tactical
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (48 tests)`

Milestone 8 Subtask 2 validation then confirmed that the `FBattleDisplay`
runtime path now delegates fire/setup interactions through the existing
`FBattleScreen` → `FTacticalGame` APIs and no longer carries its own local
fire-resolution helper.

Validation command:

```bash
cd tests/tactical && make -s && ./TacticalTests
```

Result: `OK (51 tests)`

The tactical source-inspection coverage for this step was updated at the same
time. `FTacticalBattleDisplayFireFlowTest` no longer expects a local
`FBattleDisplay::fireAllWeapons()` implementation or other transitional
source-string patterns from the pre-delegation flow; instead it checks the
delegation-oriented behavior that is now authoritative for weapon selection,
defense selection, fire-phase completion ordering, and mine-placement setup.

Milestone 8 Subtask 2 remediation validation then confirmed that fire-phase
destroyed-ship cleanup preserves cached destroyed IDs long enough for wx map
removal and still declares the correct winner by checking the destroyed side
(the active player's opponent) when model cleanup has already run. The same
runtime callback hardening now disables and hides the defensive-fire and
offensive-fire completion buttons before delegated fire-phase resolution runs,
while preserving the existing summary-dialog, cleanup, redraw, and
phase-advancement lifecycle.

The tactical GUI relayout follow-up now also locks in the sizer-managed action
button contract that those runtime paths depend on. Source-contract coverage in
`FTacticalBattleDisplayFireFlowTest` checks that the movement, defensive-fire,
offensive-fire, and mine-placement completion-button show/hide handlers call
`Layout()` immediately after toggling visibility, so a future regression that
leaves those controls at the panel origin is caught before live GUI review.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (65 tests)`

Milestone 8 remediation Subtask 1 then made the destroyed-ship lifecycle
contract itself explicit in both the model seam and the wx coordinator. The
documented ordering is now:

1. `FBattleDisplay::{onDefensiveFireDone(),onOffensiveFireDone()}` disables and
   hides the relevant fire-done button before calling
   `resolveCurrentFirePhase()`.
2. `FTacticalGame::fireAllWeapons()` resolves fire, clears any stale cached
   destroyed IDs, and captures the current destroyed-ship IDs while removing
   destroyed ships from model-owned tactical state.
3. `FBattleDisplay` shows the tactical summary dialog before any wx cleanup.
4. `FBattleScreen::clearDestroyedShips()` consumes
   `getLastDestroyedShipIDs()`, performs wx/runtime cleanup orchestration
   (including clearing a now-destroyed selected ship, redrawing the board/view,
   and preserving winner handling at the screen seam), and only then clears the
   bookkeeping via `clearLastDestroyedShipIDs()`. The screen seam does not call
   back into model destructive cleanup during this step.
5. `FBattleDisplay` advances the phase through
   `completeDefensiveFirePhase()` / `completeOffensiveFirePhase()`.

This preserves the Milestone 8 ownership split: fire resolution and destroyed
ID capture stay model-owned, while runtime view cleanup ordering remains a
wx-side responsibility owned by `FBattleScreen`.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
cd tests/tactical && make && ./TacticalTests
make -C src/tactical
```

Result: `OK (63 tests)`

Milestone 8 Subtask 3 validation then confirmed that `FBattleBoard` renders
from model-backed tactical state, forwards hex clicks through
`FBattleScreen::handleHexClick()` to `FTacticalGame`, and no longer owns
duplicate mechanics members such as `hexData`, `turnData`, `m_hexData`, or
`m_turnInfo`.

Validation commands:

```bash
make -C src/tactical && make -C tests/tactical && ./tests/tactical/TacticalTests
make -C tests/tactical clean && make -C src/tactical && make -C tests/tactical && ./tests/tactical/TacticalTests
```

Result: `OK (54 tests)`

Milestone 8 Subtask 3 remediation validation confirmed the model-side
post-move seam itself: `FTacticalGame::setPhase(PH_FINALIZE_MOVE)` routes into
model-owned `completeMovePhase()` behavior, with targeted tests covering mine
checks, relocation/removal, off-board or planet destruction, mine damage,
selected-ship clearing, and transition to defense fire. The live wx
`FBattleDisplay::onMoveDone()` callback still remained a separate runtime seam
for later remediation at that point.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (57 tests)`

Milestone 8 Subtask 4 validation then confirmed that the tactical
source-inspection suite is now anchored on the model-owned
`FTacticalGame` selection and hex-click delegation surface introduced by the
earlier Milestone 8 rewiring steps. The updated tactical tests now verify that:

- `FTacticalBattleDisplayFireFlowTest` no longer depends on any
  transitional `FBattleDisplay::fireAllWeapons()` helper and instead checks
  the click-flow forwarding APIs that remain authoritative on the wx side.
- `FTacticalBattleScreenDelegationTest` explicitly covers the selection and
  hex-interaction mutators (`selectShipFromHex()`, `assignTargetFromHex()`,
  `placeOrdnanceAtHex()`, and the legacy `placeMineAtHex()` wrapper) as
  redraw-gated forwarders into `FTacticalGame`.
- `FTacticalModelSelectionHexClickSurfaceTest` validates the canonical
  `FTacticalGame` header/implementation API for weapon selection, defense
  selection, ship selection, target assignment, generalized ordnance
  placement/source selection, occupancy queries, in-bounds checks, and
  `handleHexClick()` routing.
- Tactical test registration now includes that new suite in both
  `tests/tactical/TacticalTests.cpp` and `tests/tactical/Makefile`, matching
  repository test-runner conventions.

This validation also confirms that the tactical test coverage now reflects the
Milestone 8 architecture where `FBattleBoard` no longer owns duplicate
mechanics state and hex-click behavior is documented and tested through the
`FBattleScreen` → `FTacticalGame` delegation seam.

Validation command:

```bash
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (62 tests)`

Milestone 8 remediation Subtask 5 validation then confirmed that the live
wx move-done callback now follows the same canonical seam as the model-side
post-move finalization path. `FBattleDisplay::onMoveDone()` disconnects the
move-done button callback, calls `FBattleScreen::completeMovePhase()`, hides the
button, and restores `m_first=true` without falling back to the legacy direct
`setPhase(PH_FINALIZE_MOVE)` call. This means the actual wx runtime path now
reaches `FTacticalGame::completeMovePhase()` before post-move phase progression
continues.

The targeted tactical source-inspection coverage added for this remediation now
locks in three expectations for the live callback seam:

- `FBattleDisplay::onMoveDone()` delegates through
  `FBattleScreen::completeMovePhase()` rather than directly calling
  `setPhase(PH_FINALIZE_MOVE)`.
- The legacy direct finalize-phase call is absent from the move-done handler.
- The callback still performs the expected wx-side teardown flow around that
  seam delegation by disconnecting the move button handler, hiding the button,
  and resetting `m_first`.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
```

Result: tactical build passed and `OK (67 tests)`.

Milestone 8 remediation Subtask 7 then tightened the fire-flow coverage around
the runtime destroyed-ship lifecycle instead of the earlier duplicate-cleanup
source expectations. `FTacticalBattleDisplayFireFlowTest` now builds a minimal
`FTacticalGame`/fleet harness, resolves real defensive-fire and offensive-fire
kill scenarios, and asserts the same lifecycle the wx runtime depends on:
destroyed ship IDs are captured once by model fire resolution, the destroyed
ship is removed from the model-owned side list, and the cached IDs can then be
cleared exactly once without being repopulated during fire-phase completion.
The same suite still keeps the wx callback ordering checks that the fire-done
buttons disable and hide before delegated resolution starts.

The tactical test `Makefile` was updated alongside that coverage so the module
runner now builds the tactical model/library prerequisites first and links
against the tactical, weapons, defenses, ships, strategic, and core static
libraries. That linkage keeps the tactical test target capable of executing the
runtime model harness directly instead of limiting this seam to source-only
inspection.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
make -C src/tactical && cd tests/tactical && make && ./TacticalTests
```

Result: `OK (68 tests)` on both runs.

Milestone 8 remediation Subtask 8 then expanded automated move-completion seam
coverage instead of relying only on helper-level or source-inspection
assertions. `FTacticalBattleScreenDelegationTest` covers the live wx callback
delegation boundary (`FBattleDisplay` move-done flow delegating through
`FBattleScreen::completeMovePhase()`), while `FTacticalMineDamageFlowTest`
locks canonical post-move outcomes by calling
`FTacticalGame::completeMovePhase()` directly.

The validated runtime outcomes recorded by that coverage are:

- the live move-done callback reaches the canonical completion seam;
- post-move phase progression advances to `PH_DEFENSE_FIRE`;
- selected-ship state is cleared after move completion;
- the mover's starting hex is cleared from tactical occupancy;
- an off-board mover is reduced to zero HP during post-move resolution; and
- defender occupancy remains stable while the mover is finalized.

This keeps the Milestone 8 remediation framing consistent with the additive
wx/model split: `FBattleDisplay` still acts as the live runtime event source,
`FBattleScreen` remains the wx-side coordinator, and `FTacticalGame` remains
the authoritative post-move resolution seam whose observable outcomes are now
locked by tactical runtime coverage.

Validation commands:

```bash
make -C src/tactical
cd tests/tactical && make && ./TacticalTests
```

Result: tactical build passed and `OK (69 tests)`.

The final Milestone 8 warning-fix follow-up then replaced the remaining
selected-ship cleanup source inspection with direct lifecycle coverage at the
wx seam itself. `FBattleScreen.cpp` now exposes a minimal
`runDestroyedShipCleanupLifecycle(...)` helper plus
`FDestroyedShipCleanupLifecycle`, and `FBattleScreen::clearDestroyedShips()`
builds that context from the live screen/model objects without changing the
existing ownership contract. The focused
`FTacticalDestroyedShipCleanupLifecycleTest` suite now verifies that:

- a destroyed selected ship clears the active screen selection;
- redraw still happens on the destroyed-list cleanup path;
- `clearLastDestroyedShipIDs()` bookkeeping is triggered exactly once per
  cleanup lifecycle; and
- winner handling remains sequenced after bookkeeping clear.

`FTacticalBattleScreenDelegationTest` was tightened at the same time to assert
that `clearDestroyedShips()` wires the extracted seam instead of bypassing it,
while the tactical test runner continues linking the GUI objects needed to
exercise that boundary.

Validation command:

```bash
cd tests/tactical && make clean && make && ./TacticalTests
```

Result: `OK (72 tests)`.

Milestone 9 Subtask 1 validation then confirmed that the BattleSim tactical
launch paths still honor the supported `FBattleScreen` modal ownership and
constructor contract introduced by the tactical separation work. The expanded
`FTacticalBattleScreenElectricalFireTest` source-inspection coverage now
asserts that:

- `BattleSimFrame`, `LocalGameDialog`, `ScenarioDialog`, and
  `ScenarioEditorGUI` continue to reach tactical combat through caller-owned
  stack `ShowModal()` flows instead of heap-owned or bypassing launch patterns.
- BattleSim launcher coverage still identifies the tactical entry files that
  hand off into `FBattleScreen`.
- `FBattleScreen`'s standard constructor path still creates `FTacticalGame`,
  creates `WXTacticalUI`, and installs that UI on the tactical model rather
  than depending on caller-managed tactical wiring.

This keeps Milestone 9 Subtask 1 scoped to automated regression coverage rather
than architecture changes: the live BattleSim runtime path remains the existing
modal `FBattleScreen` path, while the tests now lock in the constructor-owned
model/UI pairing that later milestone validation depends on.

Validation command:

```bash
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (74 tests)`.

The station orbital-movement regression follow-up then documented the restored
model-owned runtime behavior for defender stations orbiting a planet.
`FTacticalGame::resetTurnInfoForCurrentMover()` now carries the orbital turn
heading into move finalization, and `resetMovementState()` immediately follows
that rebuild by routing through `checkMoveStatus()`. This lets pre-seeded
station orbital movement auto-complete the move phase as soon as the side enters
`PH_MOVE` when the station orbit is the only required movement. The refreshed
`FTacticalStationOrbitalMovementTest` suite verifies the runtime seam through
`resetMovementState()` and `completeMovePhase()`, while
`FTacticalGameMechanicsTest` keeps the mechanics-source assertion aligned with
the current `resetMovementState() -> checkMoveStatus()` path. That focused
regression coverage now confirms that:

- a station adjacent to a planet stays in orbit after move completion rather
  than drifting straight away;
- a moving side whose only required movement is the pre-seeded station orbit is
  already move-complete at movement-phase entry;
- the station heading reflects the orbital turn resolved during the move;
- station speed preservation remains intact across repeated orbit completion;
  and
- non-station movement semantics remain unchanged by the station-specific fix.

Validation command:

```bash
cd tests/tactical && make && ./TacticalTests
```

Result: `OK (84 tests)`.

The move-route selection regression follow-up then documented the restored
exact-click contract for model-owned tactical movement selection.
`FTacticalGame::handleMoveHexSelection()` now lands the route on the exact
highlighted hex the player clicks instead of stopping one hex early, and
clicking an earlier path node trims the route back to that exact hex without an
extra backward click. The new `FTacticalMoveRouteSelectionTest` runtime fixture
locks that behavior through `handleHexClick()` coverage for:

- the first highlighted movement hex extending the route by exactly one hex;
- later highlighted movement hexes becoming the actual route endpoint; and
- earlier path hexes trimming the current route back to the clicked hex while
  preserving `nMoved`, `curHeading`, and `finalHeading` invariants.

Subtask 4 then registered that move-route fixture together with the refreshed
station-orbit and final-orientation fixtures in `tests/tactical/TacticalTests.cpp`,
so the routine tactical module run now enforces all three runtime regressions.

No `FBattleBoard` renderer change was required for this fix; the existing move
overlay already matched the corrected model selection contract.

The stopped-ship free-rotation follow-up kept that same model-owned route
selection seam but broadened the zero-speed entry case. When a moving ship
enters `PH_MOVE` with `speed == 0`, an untouched one-hex path, and `MR > 0`,
`computeRemainingMoves()` now exposes every legal adjacent starting facing
instead of forcing movement projection from the prior-turn heading.
`FTacticalGame::handleMoveHexSelection()` treats those adjacent-hex clicks as
heading-only selections: it updates `startHeading`, `curHeading`, and
`finalHeading`, resets the pending path back to the current hex, and lets
`completeMovePhase()` commit an in-place rotation while preserving `speed == 0`.
The refreshed tactical regressions now lock that behavior through:

- eligible stopped ships being able to choose any legal starting facing before
  displacement;
- facing-only completion preserving the current hex and `speed == 0` while
  committing the selected final heading;
- the first traveled hex after that facing choice following the selected
  heading; and
- existing non-stopped routing plus `MR == 0` steering restrictions remaining
  unchanged.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
cd tests && make tactical-tests && ./tactical/TacticalTests
```

Result: `OK (136 tests)`.

The SSW-ZSR-002 tactical UI integration follow-up then made that stopped-ship
movement contract visible through the existing battle board and move prompt.
`FBattleBoard::drawShips()` now renders the selected moving-side ship with its
temporary `curHeading` during `PH_MOVE` when the model is in the zero-speed,
pre-displacement facing-selection state, so choosing an adjacent facing visibly
rotates the ship icon before move completion. `FBattleDisplay::drawMoveShip()`
branches its prompt copy for that same `speed == 0`, `nMoved == 0`, `MR > 0`
case so the HUD tells the player to pick an adjacent hex for facing and then
either continue plotting a route or press `Movement Done` to rotate in place.
That follow-up intentionally leaves the existing non-stopped route overlays
unchanged and adds focused tactical source-contract coverage for:

- temporary facing rendering on the selected stopped mover while other ships
  keep their persisted heading;
- move-phase prompt text that distinguishes stopped-ship facing selection from
  normal route selection and the no-ship-selected case;
- the unchanged movement/left-turn/right-turn route overlay accessors for
  nonzero-speed ships; and
- the existing `Movement Done` delegation path continuing through
  `FBattleScreen::completeMovePhase()` into normal post-move phase progression.

Validation commands:

```bash
cd tests && make tactical-tests && ./tactical/TacticalTests
cd tests && ./tactical/TacticalTests | tail -n 80
```

Result: `OK (138 tests)`.

A later additive stopped-ship preview-route follow-up kept the existing
movement/left-turn/right-turn accessors unchanged for non-stopped ships while
extending the `FTacticalGame` model surface for stopped-ship callers.
`FTacticalMovePreviewRoute` now pairs each legal starting heading, including
the current-heading forward route, with its adjacent facing hex and projected
route hexes, and
`getStoppedShipPreviewHeadingsForHex(...)` provides the reverse lookup needed
to map a clicked preview hex back to one or more legal starting facings.
`FTacticalGame` only rebuilds that preview metadata for the existing zero-speed
free-rotation case (`speed == 0`, `nMoved == 0`, one-hex pending path, and
`MR > 0`), so eligible stopped ships now expose the full six-heading preview
set while ships with `MR == 0` still expose no new facing options. The later
move-click follow-up keeps the original adjacent facing-only branch for
rotate-in-place selection, but `handleMoveHexSelection()` now also uses that
preview-heading lookup to infer the stopped ship's initial facing from a
non-adjacent highlighted preview hex before dropping back into the existing
movement append/trim flow. That keeps the first moved hex aligned with the
selected preview direction, preserves the existing route-trimming behavior
after preview-based selection, and leaves ordinary non-stopped move selection
unchanged. The refreshed tactical regressions now lock that additive contract
through runtime preview-route coverage, preview-hex heading lookup checks, and
model/API source-surface assertions while keeping non-stopped movement
highlight semantics unchanged.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
cd tests && make tactical-tests && ./tactical/TacticalTests
```

Result: `OK (141 tests)`.

The stopped-ship PH_MOVE preview-route UI follow-up then wired that additive
model contract into the tactical renderer without changing ordinary movement
overlays. `FBattleScreen` gained additive forwarding accessors for
`getStoppedShipPreviewRoutes()` and
`getStoppedShipPreviewHeadingsForHex(...)`, allowing `FBattleBoard::drawRoute()`
to render every legal preview route for the selected zero-speed, pre-
displacement mover (`speed == 0`, `nMoved == 0`, `MR > 0`) and then fall back
to the existing movement/left-turn/right-turn buckets once the player commits a
route. `FBattleDisplay::drawMoveShip()` now tells the player to select a
highlighted preview route to choose the starting facing while keeping the
adjacent-hex plus `Movement Done` rotate-in-place path available. The updated
tactical source-contract regressions now lock that UI-facing behavior through:

- renderer assertions that stopped-ship PH_MOVE selection draws every preview
  route while nonzero-speed route rendering stays on the existing path;
- prompt-copy assertions that remove the legacy adjacent-hex-only discovery
  wording in favor of the preview-route interaction text; and
- delegation assertions that the new `FBattleScreen` accessors remain additive
  forwarders over the tactical model.

Validation command:

```bash
cd tests && make tactical-tests && ./tactical/TacticalTests
```

Result: `OK (143 tests)`.

The forward-fire final-orientation regression follow-up then documented the
restored moving-ship fire-arc contract for model-owned range highlighting and
target validation. `FTacticalGame` now derives a shared per-path heading
sequence from `FTacticalTurnData`, so the endpoint of a move uses
`finalHeading` when the last movement step is a turn and both
`computeWeaponRange()` and `setIfValidTarget()` evaluate the same heading
progression. The new `FTacticalForwardFireFinalOrientationTest` runtime fixture
locks that behavior through tactical-model coverage for:

- endpoint forward-fire highlights using the post-turn orientation instead of
  the stale pre-turn heading;
- target acceptance and rejection staying aligned with the displayed
  final-orientation highlights for moving ships;
- straight-line movement preserving the pre-existing forward-fire behavior; and
- non-forward-firing battery weapons remaining heading-independent.

Validation commands:

```bash
cd tests/tactical && make && ./TacticalTests
cd tests/tactical && ./TacticalTests
```

Result: `OK (84 tests)`.

The strategic live-GUI follow-up then expanded the wx-backed regression surface
for top-level strategic UI. `tests/gui/StrategicGuiLiveTest` runs under the
shared `WXGuiTestHarness`, which can bootstrap against an existing `wxApp`,
tear down leftover top-level windows during shutdown, and now drive modal
dialogs deterministically through `showModalWithAction(...)` before a fallback
auto-dismiss closes the window. That live GUI suite now confirms that:

- `FMainFrame` constructs its `MapPanel` as an `FGamePanel` and starts with the
  expected disabled strategic menu items before a game is loaded;
- `FGamePanel` repaint behavior tracks its parent frame's client size in a live
  wx window;
- `SatharRetreatGUI`, `SystemDialogGUI`, `ViewFleetGUI`, and `SelectJumpGUI`
  can all be opened modally and closed deterministically by the harness; and
- parent-backed `WXStrategicUI` coverage now exercises retreat selection,
  system/fleet dialog entry points, and redraw-triggered paint handling in
  addition to the existing guarded no-parent paths;
- `UPFUnattachedGUI` and `SatharFleetsGUI` mutate fleet composition and system
  placement through live add/remove/update interactions, with assertions on the
  owning `FPlayer`, `FFleet`, and `FSystem` state after commit;
- `TransferShipsGUI` moves ships between fleets in a live dialog and commits the
  resulting fleet membership changes back to the model when Done is selected;
- `CombatFleetsGUI`, `CombatLocationGUI`, `TwoPlanetsGUI`, and
  `SelectResolutionGUI` now have live modal return-code coverage tied to their
  dialog-owned selection state instead of widget-existence-only checks; and
- `BattleResultsGUI` coverage now verifies both modal completion and ship-stat
  mutation through the dialog's update path.

The Subtask 2 remediation follow-up preserved that coverage but changed the
parent-backed informational message path inside `src/gui/WXStrategicUI.cpp`:
`WXStrategicUI::showMessage(...)` now uses `wxGenericMessageDialog` so the live
GUI harness can auto-dismiss those modal dialogs without hanging. The later
centering pass then normalized the rest of the strategic adapter launch surface:
`showRetreatConditions`, UPF/Sathar setup dialogs, system/fleet dialogs, and
combat selection now explicitly center on their parent window when one exists
and fall back to deterministic screen centering when they are launched without a
parent. That keeps
`StrategicGuiLiveTest::testWXStrategicUIParentBackedModalAndRedrawPaths`
covering `showMessage`, `notifyFailedJump`, `notifyVictory`, and
`showRetreatConditions` on the wx-owned GUI side while leaving the
strategic/model boundary unchanged.

No strategic/model boundary changed for this work: the added regression
coverage stays in `tests/gui/*` and exercises the wx-owned surface from the GUI
side rather than introducing new model dependencies.

The modal-launch follow-up extends that live coverage to the battle-board entry
chains. `StrategicGuiLiveTest` now drives `SelectCombatGUI::onAttack(...)`
through the real dialog chain into `FBattleScreen`, while `BattleSimGuiLiveTest`
covers the `BattleSimFrame` local-game launch path plus the `LocalGameDialog`,
`ScenarioDialog`, and `ScenarioEditorGUI` modal launch flows. The BattleSim
tests now wait for newly launched top-level or modal windows so they can prove
that `BattleSimFrame` really presented a `LocalGameDialog` parented by the
frame and that each `LocalGameDialog` button path really presented the concrete
downstream modal type (`ScenarioDialog` or `ScenarioEditorGUI`) owned by the
launching dialog. The same live fixture now also treats the launcher geometry as
part of the shipped contract: the default `BattleSimFrame` must content-fit its
three launch buttons on first show, keep the `Quit` button fully inside client
bounds, and appear centered on the active display even when callers do not
provide an explicit starting size. `LocalGameDialog` and `ScenarioDialog`
likewise preserve parent-relative centering for real launch chains while using a
deterministic screen-centered fallback when they are constructed without a
parent. `FBattleScreen` continues to expose
constructor/destructor/live-instance counters that those GUI tests use to
assert real launch ownership and deterministic teardown rather than relying on
source-structure checks alone.

The next harness remediation tightened those live-GUI expectations around
top-level ownership and cleanup. `WXGuiTestHarness` now exposes
`getTopLevelWindows(...)`, `findTopLevelWindow(...)`, `waitForTopLevelWindow(...)`,
`findModalDialog(...)`, `waitForModalDialog(...)`, and
`cleanupOrphanTopLevels(...)` so fixtures can prove that launched frames or
dialogs actually appeared, wait for modal ownership before acting, and enforce a
zero-orphan top-level state before teardown completes. `GuiHarnessTest` now
regresses those observation and cleanup paths directly, while the tactical and
BattleSim live fixtures switched from shown stack-lifetime assumptions to
explicit `Destroy()` plus event pumping on their parent frames and dialogs.
The BattleSim scenario-launch paths also preserve `FBattleScreen` lifecycle
accounting, explicitly hide and destroy shown parents before pumping events, and
compare the final shown top-level count to the pre-launch baseline after
stabilization and forced-close cleanup passes, so the live suite now proves
those launch chains return to a zero-residual shown-window state. The restored
`ScenarioDialog` predefined-scenario contract now keeps stack-owned
`FBattleScreen bb;` launch sites, hides the dialog before `bb.ShowModal()`, and
calls `Show()` after the modal battle closes instead of switching to heap
ownership or `bb.Show()`. The paired tactical source-contract test now checks
that `Hide()` → `bb.ShowModal()` → `Show()` ordering per handler, while the
live BattleSim fixture launches from a shown `ScenarioDialog` and verifies the
dialog is visible again after the battle window closes.

The tactical battle-board menu-bar follow-up keeps those launch contracts intact
while changing the underlying top-level type. `FBattleScreen` now inherits from
`wxFrame` so tactical flows can rely on frame-only menu APIs, but it preserves
the existing stack-owned `bb.ShowModal()` call sites through a class-owned modal
event-loop shim and still routes closure through modal-first `EndModal(...)`
before the non-modal destroy path. The close lifecycle is now explicitly locked
down for both `File -> Quit` and the native title-bar close vector: the menu
handler requests `Close(true)`, both close vectors converge in
`FBattleScreen::onClose(...)`, and accepted close events stay inside
`FBattleScreen::closeBattleScreen(...)` instead of continuing into default wx
frame close handling. Non-modal close requests call `Hide()` before `Destroy()`
so the screen disappears immediately while wx pending-delete cleanup completes,
modal close requests unwind the stack-owned `ShowModal()` shim through
`EndModal(...)` without destroying the frame object, and the screen-owned
`m_closeInProgress` guard is cleared if `Destroy()` does not immediately put the
frame into deletion so a first legitimate close request cannot leave the screen
stuck open. The focused tactical source-contract checks now guard that
non-modal reset behavior, preserve the single `ID_TacticalQuit` binding to
`Close(true)` so `onClose(...)` and `closeBattleScreen(GetReturnCode())` remain
the shared accepted close path, and reject `exit(...)` / `ExitMainLoop()` in
the tactical close path without freezing incidental event sequencing. The GUI
live regression now posts the real `ID_TacticalQuit` command and a separate
posted `wxEVT_CLOSE_WINDOW` title-bar vector against shown battle screens, then
uses `WXGuiTestHarness::waitForTopLevelWindowClosed(...)` as the bounded proof
seam for asynchronous frame deletion so the suite no longer depends on a human
click on the window-manager close button. Those fixtures still use the
constructed/destroyed/live lifecycle counters to prove that both BattleSim and
strategic launch chains finish with zero live battle screens after teardown and
that modal callers unwind back to their launch dialogs after closing the battle
screen.

The next dialog-sizing audit stayed evidence-driven instead of broadening the
implementation surface. Four additional strategic dialogs with confirmed
first-show clipping risk — `SatharRetreatGUI`, `CombatLocationGUI`,
`TwoPlanetsGUI`, and `SelectResolutionGUI` — now lock their constructor-time
geometry with `SetSizerAndFit(...)`, `SetMinSize(GetSize())`, and
`Centre(wxBOTH)`. `StrategicGuiLiveTest` still carries the live in-client
button-visibility checks for those flows and now also source-audits that those
four concrete files retain that first-show sizing contract. The paired
BattleSim follow-up in `BattleSimGuiLiveTest` was deliberately audit-only for
this subtask: it source-checks that `LocalGameDialog`, `ScenarioDialog`, and
`ScenarioEditorGUI` still keep their existing `Fit(...)` plus
`SetMinSize(GetBestSize())` sizing contract, documenting that no additional
BattleSim implementation churn was part of this pass. A later validation-only
recheck then reran `cd tests/tactical && make && ./TacticalTests` and
`cd tests/gui && make && ./GuiTests` without further code changes, confirming
the restored BattleSim modal-launch lifecycle still passes as `OK (92 tests)`
for tactical and `OK (29 tests)` for GUI when the canonical GUI command can run
directly.

The tactical follow-up extends that same live-dialog discipline inside
`TacticalGuiLiveTest`, which now registers direct
`testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior` and
`testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo` coverage under
`GuiTests`. The damage-summary test drives `TacticalDamageSummaryGUI` through
`showModalWithAction(...)`, asserts the dialog title plus populated and
empty-state report text, verifies the rendered report context, and dismisses
the real `Close` button through the dialog-owned modal harness instead of an
app-level callback race. It also proves that the dialog renders a labeled `Ship Damage
Summary` section for the existing per-ship rollups and, when
`showHitDetails == true`, a second labeled `Hit Details` section containing the
player-readable per-hit lines such as `Destroyer Alpha [Laser Battery] ->
Sathar Frigate: 4 hull damage`. The same live fixture now toggles
`showHitDetails` to `false` and verifies that the ship rollup still renders
while the hit-detail section is omitted entirely. The ICM test now drives
`ICMSelectionGUI` modally through row selection, spin-control assignment, and
the production `Done` completion path, then asserts the assigned interceptor
count and defender ammo decrements without bypassing the dialog's finalization
logic. Those direct tactical dialog tests finish by destroying any shown
parents or dialogs, pumping events, and calling `cleanupOrphanTopLevels(...)`
so the fixture proves zero orphaned top-level windows before teardown
completes.

That same fixture now also includes
`testTacticalActionButtonsRemainSizerPositionedWhenShown`, which checks that
the live `FBattleScreen` exposes movement, defensive-fire, offensive-fire, and
mine-placement completion buttons with nonzero geometry to the right of the
zoom/prompt area after the tactical states are shown. Because the harness does
not deterministically reproduce `FBattleScreen` paint timing strongly enough to
prove the runtime `Show()/Layout()` call order by itself, the test documents
that limitation inline and relies on the tactical source-contract coverage
above to lock down the post-show/post-hide relayout contract.

Canonical headless GUI validation command:

```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
```

Result: `OK (26 tests)`.

The next tactical lower-panel resize/reflow follow-up kept that same
player-facing layout contract but tightened the open-dialog resize path so the
runtime geometry updates deterministically instead of waiting for incidental
repaint timing. `FBattleScreen::onSize(...)` now forces
`FBattleDisplay::reflowLowerPanelLayout()` before the screen reapplies its
map-vs-display sizing policy, and `FBattleDisplay` now routes move-phase resize
recalculation through `reflowLowerPanelLayout()`,
`refreshMovePromptReservation(...)`, `buildMovePromptText(...)`, and
`getCurrentPromptMaxWidth(...)`. Those helper seams let the lower panel rebuild
its prompt reservation from the current selected-ship text and available width
before the parent layout decides how much height the panel needs, keeping the
selected-ship constrained-width movement path stable enough for downstream live
GUI checks.

The shrink-back remediation that followed removed the old "largest height seen"
retention from both layers of the tactical layout policy. `FBattleDisplay` now
recomputes `requestedDisplayHeight` from the current prompt/stats geometry
instead of preserving `previousMinHeight` or inflating the request to the
current panel height, while `FBattleScreen::applyLayoutPolicy()` treats the
display minimum as the current request and clamps only against the 120-pixel
baseline and the 60% map-floor rule. That keeps the resize behavior localized
to tactical layout only: a narrower move-phase window can still grow the lower
panel when the selected-ship reminder needs it, but widening the window again
lets the lower panel shrink back to its earlier compact height.

The paired tester coverage now locks that behavior through source-contract
assertions on the resize/reflow ordering, explicit checks that the old
max-height retention code is absent, and live resize checks that drive a
selected-ship move state narrow and then restore the wider size to prove the
lower panel returns to its previous height. Those checks keep the `Movement
Done` band below the reserved prompt region without changing any tactical rules
or fire-resolution flow.

The next lower-panel follow-up addressed a second layout failure in the same
surface: the selected-ship stats block had still been using the old
`SHIP_STATS_MIN_WIDTH` constant as the split-mode fit test, so larger ships
with longer weapon lists could be clipped at the right edge even though the
panel stayed in split mode. `FBattleDisplay` now measures the actual rendered
ship-stats block through `measureShipStatsLayoutRequirements(...)` before
`ensureLowerPanelLayoutState(...)` decides whether the right-side split can fit.
That same layout state now derives stacked placement from the button-row bottom
edge instead of from prompt text alone, so narrow tactical windows move the
ship-stats block below the completion controls rather than painting into the
same vertical band. `applyRequestedDisplayHeight()` continues to consume the
requested lower-panel height from that shared state, which keeps the existing
`FBattleScreen::onSize(...) -> reflowLowerPanelLayout() -> applyLayoutPolicy()`
resize seam intact while making the height contract content-based instead of
fixed-width.

The paired tester update added tactical source-contract coverage for measured
ship-stats width/height, split eligibility, and requested-height propagation,
plus a live GUI regression that narrows the tactical display until the lower
panel must stack the ship stats below the `Defensive Fire Done` row. Together
those checks lock in the player-facing rule that split mode is only used when
the full stats block fits and that the stacked fallback keeps the action
buttons and stats block separated after resize/reflow.

Validation commands:

```bash
cd tests && make tactical-tests && ./tactical/TacticalTests
cd tests/gui && make && xvfb-run -a ./GuiTests
```

Result: `OK (153 tests)` for the tactical runner and `OK (36 tests)` for the
GUI runner.

The tactical combat report hit-detail follow-up then documented the richer
player-facing summary shape used by `FTacticalCombatReportSummary`.
`include/tactical/FTacticalCombatReport.h` now adds
`FTacticalHitDetailSummary`, a `showHitDetails` toggle that defaults to `true`,
and a `hitDetails` collection alongside the existing per-ship rollups. The
summary builder only appends hit-detail entries for attacks where `hit == true`,
so reports with misses or report-level immediate events do not invent empty
detail rows. Each emitted hit detail keeps the attacker ship, weapon, target
ship, hull damage, and any effect labels/detail text in a player-readable
`outcome`/`displayLine` form, while suppressing only the redundant lowercase
placeholder note `Attack hit target` and preserving other meaningful note text
such as `rear arc` or `armor bypassed`. The existing ship rollup summaries
remain the canonical aggregate view for per-ship damage and effects.

That aggregate rollup contract now has a tighter player-facing shape as well.
Each summarized ship starts with a `<Ship Name>:` header line, then emits one
` - ...` bullet per grouped summary item. Hull-loss bullets keep the aggregated
total and attack count wording (for example the corrected
` - 4 hull damage from 1 attack` scenario), with attack-level hull damage
remaining the canonical player-facing total for the attacked ship. Standalone
report-level `TDET_HullDamage` events still stay in the raw report capture, but
the summary builder now suppresses them from the displayed hull total when that
ship already has `damagingAttacksReceived > 0`, preventing the old duplicate
`5 hull damage from 1 attack` inflation while still letting standalone hull
damage count for ships that were only touched by events. Defense-damage entries
render defense abbreviations in the form `Defense Hit: <abbr-list>` (for
example `Defense Hit: MS, PS`) using structured defense identity instead of
long-form names or a generic defense-damaged label, ship summaries with only
non-hull effects omit the old `0 hull damage` clause entirely, and hull-damage
summaries no longer repeat `Hull Damage xN` inside the effect bullets when the
hull-loss total is already shown in the dedicated hull bullet. Weapon-damage
rollups intentionally keep the prior comma-separated abbreviation format such
as `Weapon Hit: LB, LB, AR`, preserving duplicates and order, while repeated
non-hull effect categories continue to aggregate onto a single bullet with `xN`
counts.

The updated tactical regression coverage locks that contract in by checking:

- `FTacticalCombatReportSummary` starts with `showHitDetails == true` and can be
  toggled off by callers that want to suppress the detail section;
- `FTacticalGame` lifecycle APIs now carry the report activation, summary
  generation, nested-event normalization, and standalone immediate-event
  detachment coverage behaviorally through public state instead of tactical
  source-token inspection;
- hit-detail rows are emitted only for successful attacks and stay empty for
  no-hit summaries and immediate electrical-fire or mine-damage report shapes;
- player-readable detail text includes attacker, weapon, target, hull damage,
  any per-attack internal-effect outcome text, and meaningful non-placeholder
  notes while omitting the redundant `Attack hit target` suffix; and
- `FTacticalReportEvent` now preserves both weapon and defense damage metadata
  across immediate damage-resolution events and attack-effect construction, and
  both runtime conversion seams keep `previousValue`, `newValue`, and `amount`
  so exact ADF/MR point-loss totals survive into the ship-summary rollup;
- the ship-summary rollups now use a `<Ship Name>:` header plus one ` - ...`
  bullet per grouped summary item, show abbreviated defense hits derived from
  structured defense identity, retain the existing comma-separated weapon-hit
  abbreviation list, omit zero-hull clauses for defense-only summaries,
  preserve grouped non-hull effect counts, suppress duplicate hull-damage
  effect text across mixed-effect and hull-plus-effects cases, keep the exposed
  hull-loss scenario at `4` instead of `5` damage, and leave `rawEvents` /
  `internalEventsTriggered` available for detail/debugging even when the
  player-facing hull total suppresses duplicate standalone hull additions; and
- the touched tactical-runner fixtures no longer depend on architecture-coupled,
  non-convertible source-inspection assertions for report lifecycle,
  normalization, or damage-summary text wiring.

The dialog follow-up then made that summary contract visible to players without
changing the report model. `TacticalDamageSummaryGUI` now splits its text
construction into a `Ship Damage Summary` section and a conditional `Hit
Details` section. `buildSummaryText()` always renders the ship rollup text
first, preserves the existing `No ships sustained damage in this report.`
empty-state when no ship summaries exist, and appends the labeled detail section
only when `showHitDetails` is enabled and `hitDetails` contains rows.

The close-path hardening follow-up then tightened how that modal dialog is
dismissed on wxGTK. The `Close` button remains the dialog's affirmative/default
action, but the manual button callback that also forced `EndModal(wxID_OK)` was
removed so the dialog closes through one modal-safe path instead of a possible
double-close pattern. That change specifically covers the previously fragile
no-detail and fully empty report shapes that can travel through the runtime
`WXTacticalUI::showDamageSummary(...)` path. The same remediation also locks the
dialog's first-show geometry with `SetSizerAndFit(...)` plus `SetMinSize(...)`,
then centers the dialog on its parent battle window when one exists and falls
back to screen centering when it is launched parentless through the tactical UI
adapter.

The regression coverage now locks that behavior in at two levels. The tactical
runner coverage checks the built summary output directly, preserving content
assertions for the `<Ship Name>:` plus ` - ...` multiline summary contract
(including grouped `Weapon Hit: ...`, `Defense Hit: ...`, and exact `ADF (-N)` /
`MR (-N)` bullets), limiting hit-detail rows to per-attack internal events, and
keeping the empty-state path intact without reintroducing brittle
`TacticalDamageSummaryGUI.cpp` source-token checks. The live GUI regression
drives both the parent-backed `WXTacticalUI::showDamageSummary(...)` flow and
the direct parent-backed `TacticalDamageSummaryGUI` path with populated,
no-detail, and empty summaries. The direct modal path now routes through
`WXGuiTestHarness::showModalWithAction(...)`, pins the parent frame to a
deterministic starting position for the centering assertion, and gives the
close-button action a longer fallback timeout so the real Enter-key/default-
button interaction can complete without racing the harness auto-dismiss. The
placement contract still targets parent-relative centering, with the owning
top-level parent accepted only during the brief wxGTK first-show settle window,
so the test no longer falls back to generic display-centered acceptance for
parent-backed launches. The no-parent tactical adapter path still remains a
deterministic top-level modal on an active display for the same summary dialog.
Together with the strategic and BattleSim live fixtures, that leaves the GUI
suite covering the full placement policy matrix for this remediation cycle: a
representative top-level frame centered on the active display, representative
parent-backed tactical and strategic/BattleSim dialogs centered on their owning
window, and deterministic screen-centered fallback behavior for representative
parentless modal launches.

The startup-splash centering follow-up extended that deterministic placement
policy to the first top-level window shown by each application. The shared
`createStartupSplashAndFrame(...)` helper in `include/gui/WXStartupLaunch.h`
still owns the common wx startup flow for both `FApp` and `FBattleSimApp`, but
it now centers the created frame on screen before `Show(true)` and keeps the
splash on the framework-supported style path by combining
`wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP` with the centered splash timeout mode.
That preserves the intended "splash may coexist with the startup frame, but the
splash stays above it while visible" behavior without falling back to
window-manager-dependent placement. On the SSW side, `FApp::OnInit()` now uses
`wxDefaultPosition` instead of the old hard-coded startup coordinates and
`FMainFrame` also centers itself during construction, so both the shared helper
contract and the strategic frame constructor reinforce deterministic startup
placement. The paired GUI regression updates now cover both startup seams
directly: `StrategicGuiLiveTest` and `BattleSimGuiLiveTest` each call
`createStartupSplashAndFrame(...)`, assert that the splash and startup frame
exist and remain visible together, verify display centering for both windows,
and treat the splash window's `wxSTAY_ON_TOP` style bit as the narrowest
reliable observable that the splash remains above the frame while it is still
shown.

The AP-02 asset-path follow-up then aligned the remaining direct wx image loads
with the same resolver seam. `FBattleDisplay` now loads `data/zoom.png` through
`FGameConfig::resolveAssetPath(...)`, `FBattleBoard` resolves
`icons/MaskingScreen.png`, `FBattleScreen` resolves the tactical planet icon
set through the same helper, and `createStartupSplashAndFrame(...)` resolves
`data/splash.png` without reintroducing executable-relative concatenation. The
same follow-up also simplified `WXIconCache` so it now delegates to
`FGameConfig::resolveAssetPath(...)` instead of maintaining its own `../`
fallback path. The paired regression updates lock that policy down with
source-contract coverage in `FTacticalBattleDisplayFireFlowTest` and
`WXIconCacheTest`, plus startup helper assertions in `StrategicGuiLiveTest` and
`BattleSimGuiLiveTest` that reject hard-coded `/home/...` machine paths at the
affected call sites.

Validation command:

```bash
cd tests/tactical && make && ./TacticalTests && cd ../gui && make && xvfb-run -a ./GuiTests
```

Result: `OK (88 tests)` tactical, `OK (29 tests)` GUI.

The TACTICAL-FAIL-003 follow-up finished moving the remaining `WXTacticalUI`
adapter expectations onto stable behavioral seams. `WXTacticalUIAdapterTest`
in the tactical runner now stays focused on what the non-GUI adapter contract
can prove safely without inspecting implementation text: the adapter is usable
through `ITacticalUI`, null-parent redraw and message/winner paths remain safe,
and `runICMSelection(...)` still rejects unusable inputs. The stale runtime
"not rewired to `WXTacticalUI`" assertion was removed with the same cleanup so
the tactical runner no longer encodes architecture guesses that are outside the
fixture's real behavioral surface.

The live wx ownership checks for the remaining modal paths now live where the
runtime behavior actually occurs: `TacticalGuiLiveTest` under `GuiTests`.
That fixture drives parent-backed `WXTacticalUI::runICMSelection(...)` through
the modal `Done` path, asserts `showMessage(...)` title/body text from the live
dialog, and verifies `notifyWinner(true/false)` content for both Sathar and UPF
winner cases before dismissing the modal `OK` button. Together, those GUI
checks replace the stale tactical adapter source-token expectations with live
modal verification while preserving the standalone tactical runner for the
non-wx behavioral guardrails.

Validation commands:

```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
cd tests && make gui-tests
cd tests && make tactical-tests gui-tests && ./tactical/TacticalTests
cd tests && ./tactical/TacticalTests
```

Result: `OK (33 tests)` GUI, `OK (125 tests)` tactical.
