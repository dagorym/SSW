# Codebase Deep Review

Date: July 4, 2026

Scope: read-only architecture, structure, patterns, correctness-risk, and rules-coverage review of the current repository. The only repository change made for this pass is this report.

## Executive Summary

This codebase has a strong domain foundation for a Star Frontiers Second Sathar War / Knight Hawks implementation. The module split (`core`, `strategic`, `ships`, `weapons`, `defenses`, `tactical`, `gui`, `battleSim`) maps well to the game domain, the major types are recognizable from the rules, and the current direction of isolating strategic and tactical UI behind `IStrategicUI` and `ITacticalUI` is the right long-term architecture. The tactical work in particular has moved a lot of behavior into `FTacticalGame`, and the structured attack/reporting types are a good base for better UX, testing, and save/load.

The main risks are concentrated in four areas:

1. Strategic campaign correctness is incomplete. Some core campaign mechanics from `doc/rules/second-sathar-war-rules.md` are absent or only manual: strategic repair, supply/rearming, replacements, shatter drones, break-off consequences, and full attack-declaration ordering.
2. There are several high-priority bugs in current strategic and persistence behavior, especially `FGame::cleanUpShips()` mutating copied fleet/ship lists, retreat condition 4/5 checks being swapped, fragile save/load, and dangling/ambiguous ownership around destroyed ships.
3. Tactical mechanics have good coverage for advanced combat, mines, seekers, ICMs, screens, electrical fire, and movement, but the advanced tactical Repair Turn is still missing, and the tactical model is now a very large multi-responsibility class.
4. Module boundaries are only partially achieved. `FTacticalGame` is pure C++, but `src/tactical` still builds wx-facing `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`, so the tactical module still carries wx/GTK build flags and GUI ownership.

## What Is Working Well

### Domain Module Shape

The top-level module layout is sensible. Ship classes live in `ships`, weapon behavior lives in `weapons`, defensive systems live in `defenses`, campaign state lives in `strategic`, tactical battle mechanics live in `tactical`, and wx surfaces live mostly in `gui`. That makes the codebase understandable even before reading details.

The class names also match the board-game language. Types such as `FGame`, `FMap`, `FSystem`, `FFleet`, `FVehicle`, `FWeapon`, `FTacticalGame`, `FTacticalCombatReport`, and `ITacticalUI` are easy to tie back to rules and gameplay.

### UI Seams Are The Right Direction

`IStrategicUI` and `ITacticalUI` are valuable seams. They let model code ask for user decisions, redraws, tactical damage summaries, ICM allocation, and victory notifications without directly depending on wx dialog classes. This is the correct direction for testability and future front ends.

The tactical stack is not fully separated yet, but the existence of `FTacticalGame` plus `WXTacticalUI` is a major improvement over direct wx ownership inside every mechanics path.

### Tactical Reporting Is A Strong Foundation

`include/tactical/FTacticalCombatReport.h` defines structured report context, attack summaries, damage effects, hit details, and `weaponsFired` semantics. The `weaponsFired` contract is particularly useful because it distinguishes "no weapons fired" from "weapons fired but all missed or were intercepted" (`include/tactical/FTacticalCombatReport.h:261`, `include/tactical/FTacticalCombatReport.h:716`).

This should be preserved and extended. It can become the shared event stream for damage summaries, logs, tactical save/load, replay/debugging, and eventually AI or automated battle resolution.

### Behavioral Test Culture Is A Major Asset

The repository has 71 test fixture source files across core, strategic, ships, weapons, tactical, and GUI tests. The AGENTS policy explicitly requires behavioral tests for behavior claims, and the recent tactical test additions show that culture is active.

The GUI harness work is also valuable: live modal tests, offscreen `wxMemoryDC` rendering tests, damage-summary checks, and tactical interaction tests are much stronger than simple source-string checks. Source-contract tests still exist, but many of the important tactical flows now have behavioral coverage too.

### Recent Tactical Work Has Improved Mechanics Coverage

The current user guide and tests indicate the tactical system now covers:

- Advanced combat damage table effects: ADF/MR loss, weapon/defense hits, power system damage, combat control damage, navigation hits, fires, and DCR loss.
- Mines and seeker missiles, including setup placement, recall/undo, activation, movement, ICM allocation, and damage summaries.
- ICM sharing for same-hex defenders.
- Screen selection and combat modifiers.
- End-of-move facing change behavior.
- Station orbit movement and planet/gravity interactions.

That is a substantial functional surface, and it is supported by a growing model/adapter architecture rather than only wx event handlers.

## High-Priority Correctness Findings

### 1. Strategic Cleanup Mutates Copies, Not The Owning Fleets

`FGame::cleanUpShips()` copies the player's fleet list and each fleet's ship list:

- `FleetList fList = (*pItr)->getFleetList();` at `src/strategic/FGame.cpp:1187`
- `VehicleList sList = (*fItr)->getShipList();` at `src/strategic/FGame.cpp:1189`

It then erases destroyed ships from `sList` and empty fleets from `fList` (`src/strategic/FGame.cpp:1208`, `src/strategic/FGame.cpp:1217`). Those are local copies, so the owning `FFleet` and `FPlayer` are not actually updated. The code records destroyed ships in the player's destroyed list, increments loss counters, and reloads surviving ships, but the active fleet still keeps the original ship pointers.

Impact:

- Destroyed ships can remain in active fleets after combat cleanup.
- Empty fleets may remain owned by the player.
- Surviving ships reload unconditionally through the copied pointer list, bypassing the supply/rearm rules.
- Destroyed-list ownership becomes ambiguous because the destroyed list may hold a pointer that is still owned and later deleted by the fleet.

Recommendation:

- Make cleanup mutate owners through `FFleet::removeShip(id)` and `FPlayer::removeFleet(id)`, similar to the manual paths in `BattleResultsGUI`.
- Add behavioral strategic tests that set a ship HP to zero in a real fleet, call cleanup through a test seam, then assert the ship is absent from the fleet, present in destroyed records, counters are updated, and an empty fleet is removed from both player and system.

### 2. Sathar Retreat Conditions 4 And 5 Are Swapped In Victory Checks

The display text maps condition 4 to "a tenday passes without destroying at least two stations" and condition 5 to "Sathar lost more ships than UPF" (`src/strategic/FGame.cpp:980`, `src/strategic/FGame.cpp:987`). This matches the rules (`doc/rules/second-sathar-war-rules.md:397`, `doc/rules/second-sathar-war-rules.md:398`).

`checkForVictory()` applies the opposite checks:

- Case 4 checks `m_lostTendaySathar > m_lostTendayUPF` (`src/strategic/FGame.cpp:1046`).
- Case 5 checks `m_stationsDestroyed < 2` (`src/strategic/FGame.cpp:1056`).

Impact:

- Two of the five UPF victory/retreat conditions resolve under the wrong rule.
- The user-facing text can tell the player one condition while the engine enforces another.

Recommendation:

- Swap the case 4 and case 5 checks.
- Add behavioral tests that set each retreat condition with controlled tenday counters and assert the exact return code and counter reset behavior.

### 3. Strategic Close/Save/Open Flow Is Fragile

`FMainFrame::onClose()` asks "Do you wish to save" with `wxYES_NO|wxCANCEL`, but checks for `wxID_OK` before saving (`src/FMainFrame.cpp:298`). A Yes button returns `wxID_YES`, so choosing Yes likely does not save.

`onSave()` and `onOpen()` also call `ShowModal()` without checking for cancel, then use `GetFilename()` instead of `GetPath()` (`src/FMainFrame.cpp:128`, `src/FMainFrame.cpp:143`). Because the dialogs use `wxFD_CHANGE_DIR`, this may work only by changing the process working directory. It is brittle and easy to break.

Impact:

- Closing with "Yes, save" may silently discard the save.
- Canceling a save/open dialog can still lead to an attempted file operation.
- Save/open location depends on process current directory rather than an explicit file path.

Recommendation:

- Check `ShowModal()` return values.
- Use `wxID_YES` for close-save confirmation.
- Use `GetPath()` for file dialogs.
- Add GUI or adapter-level tests around save/cancel/close decision paths if feasible; otherwise factor file-dialog behavior behind a small seam that can be tested without wx modal automation.

### 4. `FVehicle::load()` Leaves `m_currentDefense` Dangling

The constructor sets `m_currentDefense` to the default `FNone` defense (`src/ships/FVehicle.cpp:130`). `FVehicle::load()` deletes all existing defenses and clears `m_defenses`, then loads new defense objects (`src/ships/FVehicle.cpp:218` through `src/ships/FVehicle.cpp:228`). It does not reset `m_currentDefense`.

Impact:

- After loading a vehicle, `m_currentDefense` may point to freed memory.
- Combat code frequently reads `getCurrentDefense()`, so loaded saves can crash or apply undefined defense behavior.

Recommendation:

- After defense load, set `m_currentDefense` to `m_defenses[0]` if available.
- Longer term, persist the selected/current defense by type or index rather than implicitly resetting it.
- Add a behavioral serialization test that saves a ship, loads it, and immediately calls `getCurrentDefense()->getType()` and fires at it.

### 5. Ammo And Target Invariants Are Too Weak

`FWeapon::setCurrentAmmo(int)` and `FDefense::setCurrentAmmo(int)` clamp only to max, not to zero (`include/weapons/FWeapon.h:77`, `include/defenses/FDefense.h:58`). Negative ammo is therefore possible. `FWeapon::fire()` only treats exact zero as no ammo (`src/weapons/FWeapon.cpp:111`) and decrements any nonzero ammo (`src/weapons/FWeapon.cpp:185`), so negative ammo can still fire and become more negative.

`FWeapon::setTarget()` ignores out-of-range targets but leaves any previous target intact (`src/weapons/FWeapon.cpp:191`). If a valid target was selected and then an invalid selection is attempted, the old target remains.

There is also a null-parent risk in the laser/masking-screen damage reduction branch: `m_parent->getCurrentDefense()` is used without a parent check (`src/weapons/FWeapon.cpp:169`). Current mine/seeker temporary weapons avoid that branch by type, but the invariant is fragile.

Recommendation:

- Clamp ammo to `[0, maxAmmo]`.
- Make invalid target assignment clear the target, or return `bool` and require callers to handle failure explicitly.
- Guard parent-dependent branches or make `FWeapon` require a parent for all normal ship-mounted fire and use explicit ordnance attack helpers for mines/seekers.

### 6. Factories And Lookup Helpers Encode Ambiguous Failure

`createShip(type, name)` dereferences the result of `createShip(type)` without a null check (`src/ships/FVehicle.cpp:324`). Unknown ship types can crash.

`FVehicle::hasDefense()` and `FVehicle::hasWeapon()` return an unsigned index, but return `0` for both "found at index zero" and "not found" (`src/ships/FVehicle.cpp:378`, `src/ships/FVehicle.cpp:388`). This works only when callers know index zero is a default `None` defense, and it is dangerous for weapons or future defense layouts.

Recommendation:

- Return `int` with `-1`, or a small result object, from `findWeaponIndex` / `findDefenseIndex`.
- Keep compatibility wrappers only where needed, and migrate callers.
- Make ship/weapon/defense factories fail explicitly with null-safe callers or typed errors.

### 7. Save/Load Appends To Singleton State

`FGame::load()` reads into the existing game object and assigns `m_universe = &(FMap::create())`, then calls `m_universe->load(is)` (`src/strategic/FGame.cpp:593`). It does not clear existing players or map state first. `FMap::load()` appends systems and jump routes to `m_systems` and `m_jumps` (`src/strategic/FMap.cpp:336`). `FMap::getMap()` returns `*m_map` without a null guard (`src/strategic/FMap.cpp:31`).

`FMap::load()` also reconstructs jump endpoints by treating a stored pointer-like field as an ID via a low-bit cast (`src/strategic/FMap.cpp:350`). That is a serialization smell and should be eliminated before save files become compatibility-sensitive.

Impact:

- Loading into an already-created singleton can duplicate systems, routes, and players.
- Save format is brittle and has no visible versioning or validation.
- A bad or partial save can produce null dereferences deep in load code.

Recommendation:

- Introduce explicit `clear()`/`resetForLoad()` paths for `FGame` and `FMap`.
- Add a save-format version header.
- Serialize IDs as IDs, never as pointer values.
- Add round-trip tests that load twice into the same process and assert counts remain stable.

### 8. Destroyed Ship Ownership Is Undefined

`FPlayer::~FPlayer()` deletes fleets and unattached ships, but not `m_destroyed` (`src/strategic/FPlayer.cpp:27`; `include/strategic/FPlayer.h:314`). `addDestroyedShip()` pushes raw pointers into `m_destroyed` (`src/strategic/FPlayer.cpp:75`). Depending on whether a destroyed ship was removed from a fleet or only recorded, the destroyed list may either leak ownership or contain a dangling pointer.

This matters because the optional replacement rules require a reliable destroyed-ship ledger.

Recommendation:

- Decide whether `m_destroyed` owns ships or is a non-owning historical ledger.
- If it owns ships, remove ships from active owners before adding and delete them in `FPlayer::~FPlayer()`.
- If it is non-owning, store immutable destroyed-ship records instead of raw `FVehicle*`.
- For replacements, prefer immutable records plus a factory type/name snapshot; do not resurrect an object still tied to old fleet ownership.

## Architecture And Maintainability Findings

### Tactical Model Is A Hotspot

`FTacticalGame` is the largest production class: 1,637 lines in the header and 3,602 lines in the source. It currently owns:

- Battle state and phase state.
- Setup state.
- Ship and fleet pointer lists.
- Selection state.
- Tactical board occupancy (`m_hexData[100][100]`).
- Movement paths, turn data, gravity, target ranges.
- Mines, placed ordnance, seekers, and pending seeker contacts.
- ICM data and combat reports.
- Winner/destroyed-ship bookkeeping.
- UI callback coordination through `ITacticalUI`.

See the dense state block in `include/tactical/FTacticalGame.h:1565` through `include/tactical/FTacticalGame.h:1628`.

This is understandable as an extraction step from wx owners, but it should not be the final shape. The class is already carrying several independent subdomains that can be separated without changing gameplay.

Suggested split options:

Option A: Extract services inside the tactical module, keep one facade.

- Keep `FTacticalGame` as the public facade used by `FBattleScreen`.
- Move internals into `FTacticalBoardState`, `FTacticalMovementEngine`, `FTacticalFireResolver`, `FTacticalOrdnanceState`, and `FTacticalCombatReporter`.
- Pros: lower external churn; incremental; compatible with current tests.
- Cons: `FTacticalGame` may remain a pass-through coordinator for a while.

Option B: Introduce a command/state-machine core.

- Represent tactical phases as commands/events: `BeginMove`, `ActivateSeeker`, `MoveShip`, `CommitMovement`, `ResolveFire`, `ResolveRepair`.
- Pros: better for undo/replay/save/load and AI automation.
- Cons: larger redesign; likely more test updates.

Option C: Leave `FTacticalGame` public API stable but split private implementation by composition.

- Use a PIMPL-like internal `FTacticalGameState` plus helper classes.
- Pros: reduces header churn and compile coupling.
- Cons: more indirection; still needs careful ownership work.

Recommended path: Option A first. It matches the current direction and is easiest to verify behaviorally.

### Tactical Board Dimensions Are Inconsistent

`isHexInBounds()` accepts a 100x100 coordinate space (`src/tactical/FTacticalGame.cpp:976`), while movement and targeting clamp to 55x39 visible-map limits (`src/tactical/FTacticalGame.cpp:2697`, `src/tactical/FTacticalGame.cpp:2749`, `src/tactical/FTacticalGame.cpp:2753`, `src/tactical/FTacticalGame.cpp:3345`). `include/tactical/FTacticalGame.h:1602` stores `FTacticalHexData m_hexData[100][100]`.

The design notes already identify fixed board storage as not implemented for expandable maps (`doc/DesignNotes.md:153` through `doc/DesignNotes.md:166`).

Recommendation:

- Define named constants for storage bounds and playable/visible board bounds immediately.
- Then replace fixed arrays with a sparse `std::map<FPoint, HexData>` or `std::unordered_map` once advanced off-map play is prioritized.

### Tactical Model Still Has UI-Dependent Mechanics

`applyMovementSeekerDamage()` only resolves pending seeker detonation damage if `m_ui != NULL`; otherwise it clears pending outcomes (`src/tactical/FTacticalGame.cpp:3222`). That means model-only callers can exercise seeker movement and remove seekers without applying damage.

Recommendation:

- Resolve mechanics regardless of UI.
- Make UI callbacks optional observers of already-resolved model results.
- Return a combat summary/event bundle from the model so tests and headless callers can assert damage without installing a UI object.

### `src/tactical` Still Carries wx Ownership

The repository policy says non-GUI model modules should remain non-wx, including `src/tactical`. Current `src/tactical/Makefile` has active wx/GTK flags (`src/tactical/Makefile:19`), and tactical headers such as `FBattleScreen.h`, `FBattleBoard.h`, and `FBattleDisplay.h` include `gui/GuiTypes.h`.

This is partly historical: `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` are wx runtime classes currently located under tactical. But it means "tactical" is not yet a pure model module.

Options:

Option A: Move `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` to `gui`.

- Pros: tactical model becomes cleanly non-wx; build policy aligns with architecture.
- Cons: many include paths and tests need updates.

Option B: Split into `src/tactical` and `src/tactical_wx`.

- Pros: preserves conceptual tactical ownership while making build boundaries explicit.
- Cons: adds another module and link target.

Recommended path: Option B if minimizing churn matters; Option A if the project wants a strict "all wx under gui" rule.

### Header-Only Tactical Report Implementation Increases Coupling

`include/tactical/FTacticalCombatReport.h` is 816 lines and includes `ships/FVehicle.h` and `weapons/FWeapon.h`. It also contains formatting and summary-building implementation inline.

Recommendation:

- Keep structs/enums in the header.
- Move non-trivial formatting and summary builders into `src/tactical/FTacticalCombatReport.cpp`.
- This will reduce rebuild cost and make the report module easier to test and evolve.

### Raw Pointer Ownership Is The Dominant Long-Term Risk

Raw pointers are widespread and consistent with the legacy code style, but ownership is frequently implicit:

- `FFleet` owns its `FVehicle*` and deletes them (`src/strategic/FFleet.cpp:42`).
- `FPlayer` owns fleets and unattached ships, but not destroyed ships (`src/strategic/FPlayer.cpp:27`).
- `FTacticalGame` owns only copied side lists, not the ships those lists point at (`src/tactical/FTacticalGame.cpp:344`).
- Temporary weapons and ICM structures are manually allocated and deleted in mine/seeker paths (`src/tactical/FTacticalGame.cpp:1399`, `src/tactical/FTacticalGame.cpp:3244`).

Recommendation:

- Without doing a full smart-pointer rewrite, document ownership on the typedefs and public methods.
- Add "remove returns ownership" comments where a caller must delete or transfer.
- Use stack values or small RAII wrappers for temporary tactical attack data first.
- Later, move fleet ownership to `std::unique_ptr<FVehicle>` internally while preserving raw pointer views for legacy call sites.

### Randomness Is Global And Hard To Test

`FGame` seeds the global C RNG in its constructor (`src/strategic/FGame.cpp:44`), and `irand()` wraps `rand()` globally (`include/Frontier.h:47`). Tests reseed `srand()` in multiple places, which is a symptom that global random state leaks across tests and features.

Recommendation:

- Introduce an `IRandom` or `FRandom` seam with deterministic seed support.
- Pass it into tactical and strategic mechanics, defaulting to a process-wide implementation for the app.
- Keep `irand()` temporarily as a compatibility wrapper.

### Build System Is Functional But Repetitive And Fragile

The Makefiles repeat flags and old wx comments across modules. `src/Makefile` links libraries in an order that does not match the documented dependency order (`src/Makefile:14`). The tactical module has hard-coded wx include paths and `pkg-config` flags (`src/tactical/Makefile:19`). There is also a cleanup typo in `src/defenses/Makefile:64` (`rm  -f*.gcov`).

Recommendation:

- Extract common flags/rules into a shared `make/common.mk`.
- Keep GUI/wx flags only in GUI-facing modules.
- Add a CI target that runs `make all_clean && make tests`.
- Consider CMake only if Windows/Linux project generation becomes a recurring burden; otherwise a shared Make include is a lower-risk cleanup.

## Rules Coverage And Missing Game Parts

This section compares the current code to:

- `doc/rules/second-sathar-war-rules.md`
- `doc/rules/tactical_operations_manual.md`

The tactical rules document was reviewed only as input; it was not edited.

### Strategic Rules: Implemented Or Partially Implemented

The following strategic areas appear implemented or at least partially represented:

- Initial UPF/Sathar/militia order of battle and stations are hard-coded in `FGame::initFleets()` and related creation helpers.
- Sathar start circles and player-known jump routes are represented in `FMap`.
- Strike Force NOVA placement exists through `FGame::placeNova()`.
- Fleet movement and risk-jump travel exist through `FFleet`, `FJumpRoute`, `SelectJumpGUI`, and `FGame::moveFleets()`.
- Militia fleets have home-system flags and limited connected-system lookup (`src/strategic/FMap.cpp:271`).
- Combat detection and manual/tactical resolution paths exist.
- Sathar retreat condition selection and display exist.
- Sathar victory / UPF victory checks exist, but with the condition 4/5 bug noted above.

### Strategic Rules Missing Or Incomplete

#### Strategic Repair

Rules reference:

- Campaign repair overview: `doc/rules/second-sathar-war-rules.md:316`
- UPF SCC repair capacity and one-full-day docking: `doc/rules/second-sathar-war-rules.md:320`
- Sathar return-to-base and captured SCC repair: `doc/rules/second-sathar-war-rules.md:342`

Current status:

- There is no first-class strategic repair turn/action/state machine.
- `BattleResultsGUI` can manually edit HP/ADF/MR/DCR (`src/gui/BattleResultsGUI.cpp:605`), but that is not a rules engine.
- There is no SCC capacity tracking, docked-for-repair state, captured-SCC state, or Sathar six-day repair return.

Needed changes:

- Add persistent damage records and repair-blocking flags for systems that require station repair.
- Add SCC capacity data to systems/planets.
- Add repair orders: docked at SCC, hull points allocated per day, one non-hull system per day.
- Add Sathar "enter Void for repair, return six days later" state.
- Add captured SCC detection: all stations destroyed and no UPF/militia ships in system.

#### Supply And Rearming

Rules reference:

- Supply path definition and post-battle rearming: `doc/rules/second-sathar-war-rules.md:304`
- One full day at supplied planet, no movement/attack: `doc/rules/second-sathar-war-rules.md:312`
- Fighters can rearm twice from carrier/station out of supply: `doc/rules/second-sathar-war-rules.md:314`

Current status:

- `FGame::cleanUpShips()` reloads surviving ships unconditionally, with a TODO to check supply (`src/strategic/FGame.cpp:1209`).
- No supply-path evaluator was found.
- No "spent full day rearming" state was found.
- No fighter out-of-supply rearm counters were found.

Needed changes:

- Add supply graph queries: UPF path to a fortress without Sathar-occupied systems; Sathar path to start circle without UPF ship/station.
- Add rearm order state to fleet/ship.
- Make reloading consume a full strategic day and fail if the ship moves or attacks.
- Track fighter carrier/station out-of-supply rearm allowances.

#### Strategic Attack Declaration Order

Rules reference:

- Strategic turn sequence: `doc/rules/second-sathar-war-rules.md:189`
- Sathar declares attacks first, then UPF declares attacks; UPF cannot attack Sathar ships that already attacked: `doc/rules/second-sathar-war-rules.md:274`

Current status:

- `endUPFTurn()` calls `checkForCombat()` after UPF movement (`src/strategic/FGame.cpp:512`).
- `checkForCombat()` auto-detects systems with both sides and calls `resolveCombat()`, then always returns 0 (`src/strategic/FGame.cpp:1102`).
- There is no explicit campaign state for Sathar attack declarations followed by UPF counter-declarations.

Needed changes:

- Model strategic turn phases explicitly: Sathar reinforcements/move, UPF reinforcements/move, Sathar attack declarations/resolution, UPF attack declarations/resolution, end-day upkeep.
- Track systems/fleets that already attacked so UPF declaration restrictions can be enforced.
- Make `checkForCombat()` return meaningful results or become a pure query feeding a declaration UI.

#### Break-Off Combat Consequences

Rules reference:

- Breaking off combat places fleeing ships in an adjacent transit box; militia must fire at least one weapon before breaking off from home defense: `doc/rules/second-sathar-war-rules.md:298`

Current status:

- Tactical ships can leave/die based on map bounds, but no campaign-level escape-to-transit-box flow was found.
- No militia "must attack once before breaking off" state was found.

Needed changes:

- Add battle outcome records for escaped ships and target transit boxes.
- Add tactical/campaign handoff for break-off.
- Track militia attack participation before allowing break-off.

#### Replacements Optional Rule

Rules reference:

- Replacements every 20 days: `doc/rules/second-sathar-war-rules.md:350`

Current status:

- `FPlayer::addDestroyedShip()` documentation says destroyed ships will be used for reinforcements if the optional rule is used (`include/strategic/FPlayer.h:164`).
- No replacement phase or replacement rolls were found.

Needed changes:

- Convert destroyed ships to stable replacement records.
- Add 20-day replacement phase.
- Implement Sathar 35% destroyed-ship replacement roll.
- Implement UPF replacement capacity based on surviving fortresses and fortified stations.
- Prevent replacement beyond original force limits.

#### Shatter Drones Optional Rule

Rules reference:

- Shatter drones: `doc/rules/second-sathar-war-rules.md:364`

Current status:

- No shatter-drone ship state or detonation mechanic was found.

Needed changes:

- Add optional scenario/campaign setting.
- Mark two frigates per side as drones before the first turn.
- Add end-of-movement detonation action that applies torpedo-hit damage to every other ship in hex.
- Exclude detonated drones from victory/replacement counts.

#### Risk-Jump Limits

Rules reference:

- Sathar start/red transit limit and unrepaired hull damage limit: `doc/rules/second-sathar-war-rules.md:252`

Current status:

- `SelectJumpGUI` gates risk jumps by fleet max ADF only (`src/gui/SelectJumpGUI.cpp:171`, `src/gui/SelectJumpGUI.cpp:185`).
- `FFleet::getMaxSpeed()` is based on ship ADF (`src/strategic/FFleet.cpp:209`).
- I did not find checks for unrepaired hull damage or Sathar start/red-transit restrictions.

Needed changes:

- Add fleet-level "has unrepaired hull damage" check.
- Add strategic route-position categories for Sathar start/red transit boxes, or an equivalent route state if the current map abstraction keeps using actual star systems.

#### Station Status Dialog

The user guide explicitly says the Station Status button is not implemented (`doc/UsersGuide.md:180`), and the button is disabled in `SystemDialogGUI` (`src/gui/SystemDialogGUI.cpp:65`).

Needed changes:

- Reuse or adapt `ViewFleetGUI` style status display for station `FVehicle`.
- Include HP, ADF/MR/DCR, weapons, ammo, defenses, active defense, and damage flags.

#### Sathar Retreat Execution

Rules reference:

- Once retreat condition is met, Sathar ships retreat to the nearest start circle and enter the Void: `doc/rules/second-sathar-war-rules.md:400`

Current status:

- The code can detect victory/retreat conditions and notify victory, but I did not find a forced retreat movement state.

Needed changes:

- Add a campaign "Sathar retreating" mode.
- Restrict Sathar orders to shortest path/start-circle return.
- End the game when all surviving Sathar ships enter the Void, resolving draw/victory based on fortress count.

### Tactical Rules: Implemented Or Partially Implemented

The following tactical areas appear implemented or substantially represented:

- Basic and advanced movement phases, including ADF/MR, speed, turning, orbit/station handling, and gravity interactions.
- Weapon phase restrictions for moving-player-only weapons (`FTacticalGame::selectWeapon()` at `src/tactical/FTacticalGame.cpp:2908`; display hint at `src/tactical/FBattleDisplay.cpp:1494`).
- Forward-fire and battery targeting ranges.
- Advanced damage table effects.
- Electrical fire damage at combat phase start (`src/tactical/FTacticalGame.cpp:529`).
- Navigation-hit forced turns are represented in movement (`src/tactical/FTacticalGame.cpp:2303`, `src/tactical/FTacticalGame.cpp:2470`).
- Mines, seeker missiles, ICMs, and same-hex ICM defense.
- Active screens and one-screen selection constraints.
- Tactical damage summaries and hit details.

### Tactical Rules Missing Or Incomplete

#### Advanced Tactical Repair Turn

Rules reference:

- Repair Turn after every 3 turns: `doc/rules/tactical_operations_manual.md:878`
- DCR allocation, simultaneous repairs, hull/system/fire repair, automatic failures: `doc/rules/tactical_operations_manual.md:1215`

Current status:

- No tactical repair phase, DCR allocation UI, or model repair resolution was found.
- Damage can be applied and displayed, but not repaired through the advanced Repair Turn rules.

Needed changes:

- Track full damaged-system state in a repairable form. Current fields such as ADF/MR/DCR/on-fire/damaged weapons/defenses exist, but repair needs to know what is damaged, what is permanently unrepairable in space, and what can be restored.
- Add tactical turn counter and insert a Repair Turn after every three full game turns.
- Add model API for repair allocations per ship.
- Resolve simultaneous repair attempts using DCR percentages.
- Implement 90-00 automatic failure and 99/00 station-airdock-only lockouts.
- Add UI for allocating DCR among hull, ADF, MR, weapons, defenses, fire, combat control, navigation, power, and DCR.

#### Severe Hull Break-Apart Checks

Rules reference:

- More than half hull damage creates a break-apart chance when accelerating, decelerating, or turning: `doc/rules/tactical_operations_manual.md:1153`

Current status:

- I did not find a break-apart calculation in movement. Movement updates speed/heading and may destroy a ship for map/planet bounds, but not from severe hull stress.

Needed changes:

- Add a movement-stress calculation at the point a ship commits acceleration/deceleration or MR turns.
- Include navigation-forced turns in the MR usage count.
- Add report events when a ship breaks apart.

#### Tactical Evasive Maneuvers

Rules reference:

- Fighters and assault scouts may evade torpedoes, reducing hit chance by MR x 5%, with required next-move turning: `doc/rules/tactical_operations_manual.md:274`

Current status:

- I found no model/UI path for declaring evasion or applying the next-turn movement obligation.

Needed changes:

- Add evasion declaration when a torpedo targets an eligible ship.
- Apply hit modifier.
- Persist next-move evasion obligation requiring turns in each hex until MR is used.

#### Tactical Docking And Docked-Ship Rules

Rules reference:

- Fighters and assault scouts rearm by docking; station docking restrictions and damage interactions: `doc/rules/tactical_operations_manual.md:514`, `doc/rules/tactical_operations_manual.md:606`
- Docked ships can contribute batteries/ICMs but not forward-firing weapons; station destruction damages docked ships and destroyed docked ships damage station: `doc/rules/tactical_operations_manual.md:616`

Current status:

- Scenario setup can place ships at stations, and carrier/fighter ratios are checked strategically, but I did not find first-class tactical docked state.

Needed changes:

- Add docked state to tactical placement/ship state.
- Enforce docked weapon restrictions.
- Implement rearming after a full turn docked.
- Apply station/docked-ship destruction damage transfer.

#### Advanced Off-Map Movement

Rules reference:

- Advanced game allows map extension or wrap/shift handling rather than immediate loss: `doc/rules/tactical_operations_manual.md:898`

Current status:

- The board remains a fixed playable area with 55x39 checks and 100x100 storage.
- Design notes already call fixed storage not implemented (`doc/DesignNotes.md:153`).

Needed changes:

- Decide whether to implement map extension, wrap, or board shifting.
- Replace fixed storage assumptions first, or isolate them behind board geometry APIs.

#### Drifting As A First-Class State

Rules reference:

- Ships with both ADF and MR at zero drift until repaired: `doc/rules/tactical_operations_manual.md:1171`

Current status:

- Damage can set ADF/MR to zero, and movement constraints partially fall out of those values. I did not find a named drift state or drift-specific behavior/reporting.

Needed changes:

- Add drift state derived from ADF=0/MR=0 or explicit damage state.
- Ensure drift movement continues in prior direction/speed and cannot be voluntarily changed until repair.

#### Tactical Save/Load, Damage Details, Help/About

The tactical menu creates Load Game, Save Game, Damage Details, User's Guide, and About entries but disables them (`src/tactical/FBattleScreen.cpp:153`). The user guide calls them disabled placeholders (`doc/UsersGuide.md:323`).

Needed changes:

- Tactical save/load should wait until tactical state is decomposed enough to serialize safely.
- Damage Details can probably be backed by `FTacticalCombatReport` history.
- Help/About are low-risk UI tasks once asset/document path handling is settled.

#### P/E/S Screen Visual Overlays

The design notes say Masking Screen visual replacement exists, but Proton/Electron/Stasis screen letter/circle overlays are not implemented (`doc/DesignNotes.md:232` through `doc/DesignNotes.md:239`).

Needed changes:

- Add overlay rendering in `FBattleBoard` for active non-masking screens.
- Back it with offscreen pixel tests similar to existing tactical GUI rendering tests.

## Suggested Improvement Roadmap

### Phase 1: Correctness Stabilization

Focus on bugs that can invalidate campaign results or saved games.

1. Fix `FGame::cleanUpShips()` to mutate owning fleets and players.
2. Fix retreat condition 4/5 checks.
3. Fix close/save/open dialog return handling and paths.
4. Fix `FVehicle::load()` current-defense reset.
5. Clamp ammo and clear invalid weapon targets.
6. Add targeted behavioral tests for each fix.

Pros:

- High user-visible value.
- Low architectural risk.
- Builds confidence before larger refactors.

Cons:

- Does not address missing repair/supply mechanics yet.

### Phase 2: Ownership And Serialization Contracts

1. Define destroyed-ship ownership.
2. Convert destroyed ships to records or clear ownership transfer.
3. Add save-format versioning.
4. Make load reset existing state instead of appending.
5. Add round-trip serialization tests.

Pros:

- Required for replacements, repair orders, and reliable long-running campaigns.

Cons:

- Touches core strategic object lifetimes and may expose hidden assumptions.

### Phase 3: Tactical Model Split

1. Extract board/occupancy state.
2. Extract movement engine.
3. Extract fire/ordnance resolver.
4. Move report builders to `.cpp`.
5. Make seeker/mine damage resolve without requiring UI.

Pros:

- Makes Repair Turn and tactical save/load much easier.
- Shrinks the primary hotspot.

Cons:

- Requires careful migration because tactical tests cover many call paths.

### Phase 4: Missing Rules Mechanics

Strategic:

1. Supply graph and one-day rearm orders.
2. Strategic repair at SCCs and Sathar repair return.
3. Explicit strategic attack declaration phases.
4. Break-off consequences.
5. Optional replacements.
6. Optional shatter drones.

Tactical:

1. Repair Turn after every three turns.
2. Severe hull break-apart checks.
3. Evasive maneuvers.
4. Docking/rearming/docked combat restrictions.
5. Advanced off-map movement.
6. Drift state.

Pros:

- Moves the project toward full rules completeness.

Cons:

- Should wait for ownership/serialization cleanup where mechanics need persistent state.

### Phase 5: Build And Boundary Cleanup

1. Split tactical model from tactical wx runtime.
2. Move shared Makefile rules to a common include.
3. Remove hard-coded wx/GTK paths from non-GUI modules.
4. Add CI-style build/test targets.

Pros:

- Reduces platform fragility.
- Makes pure model tests cheaper and easier to run.

Cons:

- Mostly engineering leverage; less immediate gameplay benefit than Phases 1 and 4.

## Suggested Test Additions

High-value behavioral tests:

- Destroyed ship cleanup removes ships from the owning fleet and system, updates counters, and does not double-own pointers.
- Retreat condition 4 and 5 each trigger only under their rule text.
- Save/open/close dialog seam handles cancel, yes-save, and full paths.
- Vehicle save/load preserves valid current defense.
- Negative ammo cannot fire.
- Invalid target assignment cannot preserve a stale target.
- Loading a game twice in one process does not duplicate map/player state.
- Supply path allows/disallows rearm based on occupied systems.
- Tactical Repair Turn restores exactly the selected system/hull/fire state and handles automatic failure.
- Severe hull movement stress can destroy a ship and emits a report event.
- Evasive maneuver modifies torpedo chance and imposes next-move obligations.
- Docked ships obey weapon restrictions and station destruction damage transfer.

## Closing Assessment

The codebase is in a better architectural position than a typical legacy wx game because it already has domain modules, model-facing UI seams, and a real behavioral test culture. The biggest issue is not that the code is unstructured; it is that the tactical model has absorbed too many responsibilities during extraction, while the strategic campaign layer still has several incomplete or incorrect rules-critical paths.

The most pragmatic next move is to fix the high-priority correctness bugs first, then settle ownership/serialization, and only then implement the missing campaign and tactical repair systems. Those features depend on reliable object lifetime and persistent damage state, so doing them before cleanup would likely add more special cases to already fragile areas.
