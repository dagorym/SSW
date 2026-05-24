# Tactical Seeker Missiles Plan

## Feature Restatement

Implement the final advanced tactical weapon system, seeker missiles. Seeker missile launchers already exist as weapon stats, but deployed seekers need their own tactical model state. Seekers are deployed like mines, remain hidden while inactive, can be manually activated at the owning player's seeker activation step, then move autonomously before normal ship movement. Ships with remaining seeker ammo can also deploy seekers during their own offensive fire phase onto any hex in their current-turn path, with current-phase pending deployments undoable until offensive fire is completed. Active seekers detonate on contact with ships, use seeker missile combat stats, allow ICM defense, and expire when their movement allowance reaches 12 hexes.

## Confirmed Repository Facts

1. The rules source for seeker missiles is `doc/rules/tactical_operations_manual.md`, especially lines 972-1011, with related sequence/combat-table/minelayer rules around lines 863-880, 1076, 1147, and 1293-1298.
2. `doc/rules/tactical_operations_manual.md` is a protected external artifact and must not be edited.
3. `FSeekerMissileLauncher` exists in `include/weapons/FSeekerMissileLauncher.h` and `src/weapons/FSeekerMissileLauncher.cpp`, but it is only a launcher/weapon-stat object today.
4. Mine placement, mine damage, movement state, report state, and tactical phase control are owned by `FTacticalGame`, with `FBattleScreen` forwarding model APIs and `FBattleBoard` / `FBattleDisplay` handling wx rendering and input.
5. Current mine placement uses `BS_PlaceMines`, `beginMinePlacement()`, `placeMineAtHex(...)`, `completeMinePlacement()`, and display/board methods that are mine-specific.
6. Existing mine damage creates temporary `FWeapon::M` objects, runs ICM allocation through `ITacticalUI`, builds an immediate damage summary, and clears destroyed ships after the summary path.
7. `icons/SeekerMissile.png` is assumed to be added by the user before implementation starts.

## Resolved Design Decisions

1. Seeker deployment is handled like current mine placement: each ship/weapon row shows deployable ordnance and placement decrements the selected launcher ammo.
2. The placement model must remember the exact source ship and weapon slot for each placed mine or seeker so undo restores ammo to the correct source.
3. Seekers do not activate when a ship enters or passes through the hex while they are inactive.
4. Use a dedicated lightweight tactical seeker model owned by `FTacticalGame`, not `FVehicle` and not only per-hex counts.
5. Damage resolution does not need the launching ship. On detonation, create a temporary `FWeapon::SM`, set its target, leave parent `NULL`, and resolve the attack similarly to mines.
6. Inactive seekers are hidden during normal battle phases and shown only during their owning player's seeker activation phase.
7. Active seekers are rendered on the main board during normal battle phases, but not during seeker activation, because activation is one-way.
8. During seeker activation, board clicks select a hex stack and the lower panel lists inactive seekers in that hex as clickable rows; clicking a row activates that seeker irreversibly.
9. Seeker target selection and movement are pure model-side behavior. Use greedy closest-distance movement with up to three hexsides of pre-move facing adjustment and one hexside of turn after each entered hex.
10. All active seekers owned by the moving player resolve after seeker activation completes and before normal ship movement begins. Active seekers already sharing a ship hex detonate before movement.
11. Equidistant target ties and same-size detonation target ties use automatic RNG. Largest ship means highest original/max HP, not current HP.
12. Seeker detonations use the existing ICM allocation flow at detonation time, batched for all seeker detonations in the seeker movement step where practical.
13. Combat-phase seeker deployment is offensive-fire only. Selecting an `SM` launcher during offensive fire highlights the selected ship's current-turn path, including start, moved-through, and final/current hexes. Only seekers deployed by that launcher during the current offensive fire phase are highlighted and undoable; pre-battle seekers and earlier-turn seekers cannot be recalled.

## Assumptions

1. A player-facing seeker activation screen is only needed when the moving player has inactive seekers available to activate. If there are no inactive seekers, the model may auto-resolve already-active seekers for that moving player and continue into normal movement.
2. Active seeker contact during ship movement should be detected from the moving ship's planned path during `completeMovePhase()`. The report should resolve before mine damage because seeker contact is immediate while mine damage is currently end-of-movement.
3. Offensive-fire seeker deployments should decrement ammo as pending placements immediately and commit when offensive fire is completed. Undo before completion restores ammo to the selected launcher.
4. Existing tactical tests can provide most coverage through model and source-contract tests. GUI tests should cover visible interaction seams where the wx behavior itself matters.

## Likely Files To Modify

1. `include/Frontier.h`
2. `include/tactical/FTacticalGame.h`
3. `src/tactical/FTacticalGame.cpp`
4. `include/tactical/FBattleScreen.h`
5. `src/tactical/FBattleScreen.cpp`
6. `include/tactical/FBattleDisplay.h`
7. `src/tactical/FBattleDisplay.cpp`
8. `include/tactical/FBattleBoard.h`
9. `src/tactical/FBattleBoard.cpp`
10. `include/tactical/FTacticalCombatReport.h`
11. `include/weapons/FSeekerMissileLauncher.h`
12. `src/weapons/FSeekerMissileLauncher.cpp`

Optional new tactical model header/source files may be added if the seeker and placed-ordnance data structures become too large for `FTacticalGame.h`.

## Subtasks

### TSM-001 - Add Source-Tracked Ordnance And Seeker Model Records

Define the model-side records needed to represent placed ordnance and active/inactive seeker missiles without changing live behavior yet.

Acceptance Criteria:
- Tactical model state can represent a placed ordnance item with weapon type, owner ID, source ship ID, source weapon index, source weapon ID, placement hex, and display color/index metadata.
- Tactical model state can represent a seeker missile with unique seeker ID, owner ID, hex, heading, active flag, movement turn/allowance state, and optional source provenance.
- `FTacticalGame::reset()` clears the new ordnance and seeker collections.
- Public const accessors expose enough read-only state for later board/display rendering without leaking wx types.
- No wx headers or wx types are introduced into non-GUI tactical model state.

Documentation Impact:
- Update Doxygen comments for new tactical structs/classes and `FTacticalGame` accessors.
- Do not edit `doc/rules/tactical_operations_manual.md`.

### TSM-002 - Generalize Placement Model For Mines And Seekers

Replace mine-only placement bookkeeping with source-tracked placement APIs that support both mines and seeker missiles while preserving current mine behavior.

Acceptance Criteria:
- The placement model discovers ships that can deploy mines or seeker missiles, not only ships with mines.
- The current placement source can be selected by ship and weapon slot.
- Placing a mine decrements the selected mine launcher ammo and records a source-tracked mine item.
- Placing a seeker decrements the selected seeker missile launcher ammo and creates an inactive source-tracked seeker item.
- Clicking an already placed ordnance marker during placement can undo only the matching selected source item and restore ammo to the exact source weapon slot.
- Mine rules still allow only one mine group per hex.
- Seeker rules allow any number of seekers in a hex.
- Inactive seekers do not appear in mine-trigger checks and do not detonate when ships pass through their hex.
- Existing mine accessors remain compatible or are replaced by equivalent model accessors used by existing renderers/tests.

Documentation Impact:
- Update Doxygen comments for renamed or generalized placement APIs.
- User-facing docs may later describe seeker placement, but no rules-document edit is allowed.

### TSM-003 - Update Placement Display And Board Markers

Update the current mine placement wx surface so it displays mines and seekers by ship/weapon row and renders placed ordnance with source-specific colors.

Acceptance Criteria:
- The lower placement panel lists each deployable ship/weapon combo as a separate clickable row, including mine rows and seeker missile rows.
- Each row displays the current ammo count for that exact weapon slot.
- Selecting a row changes the active placement source used by map clicks.
- Placed mines and inactive seekers are rendered during placement with visually distinct colors per ship/weapon combo.
- Undoing a marker from the map restores the row's ammo count visibly.
- Current mine-only setup flow still reaches attacker setup after placement is completed.

Documentation Impact:
- Update `FBattleDisplay` / `FBattleBoard` Doxygen comments for any new placement rendering or row-selection helpers.
- No rules-document edit.

### TSM-004 - Add Seeker Activation Phase And Model APIs

Add a seeker activation phase before normal movement and expose model APIs for selecting inactive seeker stacks and activating individual seekers.

Acceptance Criteria:
- Tactical phases include a seeker activation phase before `PH_MOVE`.
- Normal movement for each player starts only after seeker activation completion and active seeker resolution for that moving player.
- If the moving player has inactive seekers, the activation phase becomes user-visible and waits for the activation done action.
- If the moving player has no inactive seekers, the model can skip visible activation UI while still resolving active seekers before movement.
- Model accessors expose inactive seeker hexes for the moving player and a selected activation hex stack.
- Activating a seeker changes only inactive owned seekers to active; active seekers cannot be deactivated.
- Activation and phase progression remain model-side and wx-free.

Documentation Impact:
- Update Doxygen comments for new phase helpers and activation APIs.
- User-facing docs may later mention the seeker activation step.

### TSM-005 - Implement Seeker Activation UI And Visibility Rendering

Add the wx activation surface: activation prompts, clickable lower-panel seeker rows, activation done button, and board rendering for active/inactive seeker visibility rules.

Acceptance Criteria:
- During seeker activation, the board renders only inactive seekers owned by the moving player.
- Active seekers are not rendered during seeker activation.
- During normal battle phases, the board renders active seekers and does not render inactive seekers.
- The activation lower panel provides instructional text, a `Seeker Activation Done` button, and clickable rows for the inactive seekers in the selected stack.
- Clicking a visible inactive seeker stack on the board selects that stack for lower-panel row display.
- Clicking a row activates that seeker and updates the board/lower-panel state.
- Same-hex multiple seekers remain distinguishable through the lower-panel rows.
- The activation button follows the existing action-button layout/visibility pattern and relayouts after show/hide.

Documentation Impact:
- Update Doxygen comments for new `FBattleDisplay` and `FBattleBoard` seeker rendering/input helpers.
- User-facing docs may later describe the activation interface.

### TSM-006 - Add Offensive-Fire Seeker Deployment And Pending Undo

Add selected-launcher battle deployment for seeker missiles during the moving player's offensive fire phase.

Acceptance Criteria:
- Selecting an undamaged `SM` launcher with ammo during `PH_ATTACK_FIRE` for the moving player enters seeker deployment mode rather than normal weapon targeting.
- `SM` launchers do not deploy seekers or perform normal target assignment during defensive fire.
- Legal deployment hexes are exactly the selected ship's current-turn path: start hex, moved-through hexes, and final/current hex.
- Board highlights legal deployment hexes while the `SM` launcher remains selected.
- Clicking a legal hex creates an inactive seeker in that hex, decrements selected launcher ammo, and records it as pending for that launcher/current offensive fire phase.
- Multiple pending seekers may be placed in the same legal hex if ammo allows.
- The lower/status panel shows pending seeker deployments for the selected launcher grouped by hex with counts or rows.
- Clicking a pending row/display item removes one current-phase pending seeker from that hex and restores ammo to the selected launcher.
- Pending seekers from the current phase reappear/highlight if the user reselects the same launcher before `Offensive Fire Done`.
- Seekers deployed pre-battle or in earlier turns are not highlighted and cannot be recalled.
- Completing offensive fire commits pending deployments so they become ordinary inactive seekers for future activation.

Documentation Impact:
- Update Doxygen comments for offensive-fire seeker deployment, pending placement, and undo APIs.
- User-facing docs may later describe combat-phase seeker deployment.

### TSM-007 - Implement Model-Side Seeker Targeting And Movement Helpers

Implement pure model helpers that choose seeker targets and compute one movement step without damage/report integration.

Acceptance Criteria:
- Target selection considers ships only, ignores planets and stations, and includes friendly and enemy ships.
- Closest target selection uses hex distance from the seeker to ship hexes.
- Equidistant target ties are resolved through existing RNG behavior or a narrow model-side helper using existing RNG.
- The seeker may adjust facing up to three hexsides before movement.
- After each entered hex, movement may turn one hexside using greedy distance reduction toward the selected target.
- Movement allowance is 2 on the first active seeker move, then 4, 6, 8, 10, and 12.
- A seeker with no valid ship target on its 12-hex movement expires/detonates harmlessly.
- Helpers are testable without wx runtime objects.

Documentation Impact:
- Add Doxygen comments for targeting/movement helper declarations.
- No user docs required for this isolated helper work.

### TSM-008 - Resolve Active Seeker Movement Step And Contact Events

Wire model-side seeker movement into the activation completion path, producing contact/expiry outcomes while deferring actual damage reporting to the seeker damage subtask.

Acceptance Criteria:
- Completing seeker activation resolves all active seekers owned by the moving player before normal ship movement.
- Active seekers already in a ship-occupied hex produce a detonation/contact outcome before attempting movement.
- Active seekers that enter a ship-occupied hex during movement stop and produce a detonation/contact outcome.
- Active seekers that move 12 hexes expire at the end of that seeker move; if no ship is in range/contact, they are removed without damage.
- Active seekers that survive movement update their hex, heading, and next movement allowance state.
- Enemy active seekers are not moved during the current player's seeker step.
- Movement outcomes are model-owned and do not depend on wx rendering.

Documentation Impact:
- Update Doxygen comments for active seeker resolution methods.
- No rules-document edit.

### TSM-009 - Add Seeker Detonation Damage, ICM, And Reports

Resolve seeker detonation outcomes through existing weapon damage, ICM allocation, and tactical report summary surfaces.

Acceptance Criteria:
- A seeker detonation creates a temporary `FWeapon::SM` attack against the selected target.
- If a seeker detonates in a hex with multiple ships, it targets the ship with highest max HP.
- Same-max-HP target ties use automatic RNG.
- ICM allocation is offered for seeker attacks using the existing `ITacticalUI::runICMSelection(...)` path and seeker `-8/ICM` modifier.
- Seeker damage uses seeker stats: base 75%, stasis screen modifier to 90%, damage table modifier -20, and 5d10 hull damage rating when hull damage is rolled.
- Multiple seeker detonations in one seeker step are batched into one immediate seeker damage report where practical.
- Destroyed-ship cleanup happens after the immediate summary path, consistent with mine damage and fire reports.
- The report type/labels distinguish seeker damage from mine damage.

Documentation Impact:
- Update `FTacticalCombatReport` labels for seeker damage if new report/event enum values are added.
- User-facing docs may later mention seeker damage summaries.

### TSM-010 - Resolve Active Seeker Contact During Ship Movement

Detect active seeker contact when ships move through active seeker hexes, and integrate that damage path with existing movement finalization and mine damage ordering.

Acceptance Criteria:
- During normal movement finalization, a ship path entering a hex containing an active seeker triggers seeker detonation against that ship or the largest ship in that hex, as applicable.
- Inactive seekers are ignored by ship movement contact checks.
- Ship-triggered seeker detonations resolve before mine damage for the same movement completion.
- Detonated seekers are removed exactly once.
- Seeker contact can destroy a ship without leaving stale occupancy, turn-info, or selected-ship state.
- Existing mine damage still applies exactly once after movement and after seeker-contact resolution.
- Normal movement phase progression to defensive fire remains correct after seeker-triggered reports and cleanup.

Documentation Impact:
- Update Doxygen comments for movement-finalization seeker contact helpers.
- User-facing docs may later mention active seeker contact timing.

## Dependency Ordering

1. `TSM-001` must complete first because all later work depends on the new model records.
2. `TSM-002` depends on `TSM-001` because placement needs source-tracked ordnance and seeker records.
3. `TSM-003` depends on `TSM-002` because the UI needs model placement sources and source colors.
4. `TSM-004` depends on `TSM-001` and should run after `TSM-002` so inactive deployed seekers exist before activation APIs are wired.
5. `TSM-005` depends on `TSM-004` because it renders and forwards activation state.
6. `TSM-006` depends on `TSM-002` and `TSM-003` because offensive-fire deployment reuses source-tracked seeker creation and board/display deployment patterns.
7. `TSM-007` depends on `TSM-001` because movement helpers operate on seeker records.
8. `TSM-008` depends on `TSM-004` and `TSM-007` because it resolves active seekers at activation completion.
9. `TSM-009` depends on `TSM-008` and should run after `TSM-006` so all seeker creation paths feed the same inactive seeker collection before damage/reporting is finalized.
10. `TSM-010` depends on `TSM-009` because ship movement contact uses the same seeker damage/report path.

Parallelization note: Treat this plan as mostly serial. `TSM-003` and `TSM-007` can begin in parallel once their dependencies are stable, but conservative coordinator execution should run these subtasks sequentially because they share `FTacticalGame` and phase/rendering contracts. `TSM-006` is not a good parallel candidate because it touches the same fire-phase selection, board-highlight, and lower-panel display code as multiple surrounding subtasks.

## Overall Acceptance Criteria

1. Ships with seeker missile launchers can deploy inactive seekers through the same setup/drop style used for mines, and deployment consumes launcher ammo.
2. Placement undo restores ammo to the exact source ship/weapon slot.
3. Multiple seekers can be deployed in the same hex.
4. Inactive seekers remain hidden except during their owner activation phase.
5. Activation presents inactive same-hex seekers as explicit lower-panel rows and activates individual seekers irreversibly.
6. Active seekers move autonomously before their owning player's ship movement and render on the board during normal battle phases.
7. Ships can deploy remaining seekers during their own offensive fire phase onto current-turn path hexes, and only current-phase pending deployments are highlighted/undoable before `Offensive Fire Done`.
8. Seeker target selection, movement, detonation, expiry, ICM, and damage resolution are model-owned and wx-free.
9. Active seekers detonate when they enter ship hexes or ships enter active seeker hexes.
10. Seeker damage uses the established `FWeapon::SM` stats and tactical report machinery.
11. No implementation change requires editing `doc/rules/tactical_operations_manual.md`.

## Documentation Impact

Overall documentation impact is moderate. Header Doxygen must be updated for any new tactical model records, phase helpers, placement APIs, activation APIs, offensive-fire pending deployment APIs, and rendering/input helpers. The Documenter stage should update `doc/UsersGuide.md` to describe seeker setup deployment, offensive-fire deployment, activation, visibility, and active-seeker movement/detonation behavior. `doc/DesignNotes.md` may be updated if the project tracks the final weapons-system milestone there. `doc/rules/tactical_operations_manual.md` must not be edited.

## Implementer Prompts

### TSM-001 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalSeekerMissile.h`
- `src/tactical/FTacticalSeekerMissile.cpp`

Task to implement:
Add the tactical model records needed for source-tracked placed ordnance and active/inactive seeker missiles. Prefer lightweight structs in `FTacticalGame.h` unless a small new non-wx tactical header/source is clearer. Do not change live placement, activation, movement, or damage behavior in this subtask. Keep all new model types free of wx headers/types.

Acceptance criteria:
- Model state can represent placed ordnance with weapon type, owner ID, source ship ID, source weapon index, source weapon ID, hex, and display color/index metadata.
- Model state can represent a seeker missile with unique seeker ID, owner ID, hex, heading, active flag, movement turn/allowance state, and optional source provenance.
- `FTacticalGame::reset()` clears the new collections.
- Read-only accessors expose the new collections or filtered views needed by later renderers.
- Non-GUI tactical model code remains free of direct wx includes.

Validation guidance:
- Build with `cd tests && make tactical-tests`.
- Run `cd tests && ./tactical/TacticalTests`.

Tester test-file location guidance:
- Model state and reset/accessor coverage belongs in `tests/tactical/FTacticalGameMechanicsTest.*` or a focused new tactical seeker fixture.
- Boundary checks for no wx leakage belong near existing tactical header/source-contract tests.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-001/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-002 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/weapons/FSeekerMissileLauncher.h`
- `src/weapons/FSeekerMissileLauncher.cpp`

Task to implement:
Generalize the model placement flow so the current mine placement/drop mechanics can place both mines and seeker missiles from a selected ship/weapon source. Placement must decrement the selected source weapon ammo. Undo must restore ammo to the exact source ship/weapon slot. Preserve mine behavior and keep inactive seekers out of mine-trigger logic. Do not implement the wx placement UI redesign in this subtask beyond any forwarding APIs needed to compile.

Acceptance criteria:
- The model discovers deployable mine and seeker missile weapon slots.
- The model can select the current deployment source by ship and weapon slot.
- Placing a mine records a source-tracked mine and decrements the selected mine ammo.
- Placing a seeker creates an inactive seeker and decrements the selected seeker missile ammo.
- Undoing a placed item restores ammo to the exact source weapon slot.
- Mines remain limited to one mine group per hex.
- Seekers can stack without limit in a hex.
- Inactive seekers are not considered by `checkForMines(...)` or mine damage.
- Existing mine placement behavior remains available through compatibility methods or updated forwarding methods.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build weapon tests if seeker launcher metadata changes with `cd tests/weapons && make`.

Tester test-file location guidance:
- Placement model coverage belongs in `tests/tactical/FTacticalGameMechanicsTest.*` or a focused seeker placement fixture.
- Weapon launcher behavior coverage belongs in `tests/weapons/FSeekerMissileLauncherTest.*` if metadata or selectable behavior changes.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-002/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-003 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task to implement:
Update the existing mine placement wx interface so it displays all deployable mine and seeker missile sources as separate ship/weapon rows. Each row should show the source ammo, use a distinct placement color, and select the source used by map placement clicks. The board should render placed mines and inactive seekers during placement using the source-specific colors. Keep the existing placement done lifecycle intact.

Acceptance criteria:
- The placement panel lists mine and seeker missile rows per ship/weapon slot.
- Each row shows the current ammo for that exact weapon slot.
- Clicking a row changes the active placement source.
- Board placement markers use distinct colors per source ship/weapon combo.
- Map-click placement and undo update the visible ammo counts.
- Existing mine-only cases still work when no seeker launchers are present.
- Placement controls relayout after visibility changes following the existing action-button pattern.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build GUI tests with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Source-contract coverage for placement UI forwarding belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*`.
- Live placement/visibility behavior can be covered in `tests/gui/TacticalGuiLiveTest.*` if needed.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-003/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-004 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/Frontier.h`
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task to implement:
Add a model-owned seeker activation phase before normal movement. The phase should expose owned inactive seeker stacks, allow selecting an activation hex, allow activating individual inactive seekers, and complete into normal movement only after active seekers for the moving player have been resolved by later subtasks. It is acceptable in this subtask for active seeker resolution to be a stub/no-op seam that later subtasks fill in.

Acceptance criteria:
- A seeker activation phase exists before `PH_MOVE`.
- Movement phase entry can skip visible activation when the moving player has no inactive seekers while still calling the active-seeker resolution seam.
- Model APIs expose inactive seeker hexes and selected activation stack state for the moving player.
- Model APIs can activate an individual inactive seeker from the selected stack.
- Active seekers cannot be deactivated.
- `FBattleScreen` forwards the new model APIs needed by the display and board.
- Existing movement/fire phase progression remains source-compatible.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.

Tester test-file location guidance:
- Phase progression and activation API coverage belongs in `tests/tactical/FTacticalGameMechanicsTest.*` or a new seeker activation fixture.
- Battle-screen forwarding source-contract coverage belongs near existing `tests/tactical/FTacticalBattleScreenDelegationTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-004/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-005 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task to implement:
Implement the seeker activation wx surface and visibility rules. During seeker activation, render only inactive seekers owned by the moving player, let board clicks select a seeker stack, list inactive seekers in that selected stack as clickable lower-panel rows, and provide a `Seeker Activation Done` action button. During normal battle phases, render active seekers using `icons/SeekerMissile.png` and hide inactive seekers.

Acceptance criteria:
- Activation phase board rendering shows only inactive seekers owned by the moving player.
- Active seekers are not rendered during activation.
- Normal battle rendering shows active seekers and hides inactive seekers.
- The activation lower panel has instructional text, clickable rows for the selected inactive seeker stack, and a `Seeker Activation Done` button.
- Clicking a row activates exactly that seeker and refreshes the panel/board.
- Multiple inactive seekers in one hex are handled through distinct rows.
- `icons/SeekerMissile.png` is loaded through the shared asset resolution policy.
- The new activation button follows existing show/hide/disconnect/relayout patterns.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build GUI tests with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Source-contract checks for activation UI and asset loading belong in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*` or `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`.
- Live interaction coverage belongs in `tests/gui/TacticalGuiLiveTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-005/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-006 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`
- `include/weapons/FSeekerMissileLauncher.h`
- `src/weapons/FSeekerMissileLauncher.cpp`

Task to implement:
Add offensive-fire-only seeker missile deployment for selected `SM` launchers. When a moving player's ship selects an undamaged seeker launcher with ammo during offensive fire, use the selected ship's current-turn path as the legal placement area. Placements made in this mode are pending until `Offensive Fire Done`, decrement launcher ammo immediately, and can be undone only for the same launcher/current offensive fire phase. Do not make pre-battle seekers or earlier-turn seekers recallable.

Acceptance criteria:
- Selecting an undamaged `SM` launcher with ammo during `PH_ATTACK_FIRE` for the moving player enters seeker deployment mode rather than normal weapon targeting.
- `SM` launchers do not deploy seekers or perform normal target assignment during defensive fire.
- Legal deployment hexes are exactly the selected ship's current-turn path: start hex, moved-through hexes, and final/current hex.
- Board highlights legal deployment hexes while the `SM` launcher remains selected.
- Clicking a legal hex creates an inactive seeker in that hex, decrements selected launcher ammo, and records it as pending for that launcher/current offensive fire phase.
- Multiple pending seekers may be placed in the same legal hex if ammo allows.
- The lower/status panel shows pending seeker deployments for the selected launcher grouped by hex with counts or rows.
- Clicking a pending row/display item removes one current-phase pending seeker from that hex and restores ammo to the selected launcher.
- Pending seekers from the current phase reappear/highlight if the user reselects the same launcher before `Offensive Fire Done`.
- Seekers deployed pre-battle or in earlier turns are not highlighted and cannot be recalled.
- Completing offensive fire commits pending deployments so they become ordinary inactive seekers for future activation.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build GUI tests with `cd tests/gui && make`.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Offensive-fire seeker deployment model coverage belongs in a focused tactical seeker placement fixture or `tests/tactical/FTacticalGameMechanicsTest.*`.
- Source-contract coverage for selected weapon forwarding, board highlights, and lower-panel pending rows belongs in `tests/tactical/FTacticalBattleDisplayFireFlowTest.*` or adjacent battle-board/display tests.
- Live interaction coverage, if needed, belongs in `tests/gui/TacticalGuiLiveTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-006/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-007 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/core/FHexMap.h`
- `src/core/FHexMap.cpp`

Task to implement:
Implement pure model-side seeker targeting and movement helper logic. Use closest ship by hex distance, ignore planets and stations, allow friendly and enemy ships as targets, apply RNG tie-breaking for equal-distance target candidates, allow up to three hexsides of pre-move facing adjustment, then move greedily toward the target with at most one hexside turn after each entered hex. Do not wire damage or UI in this subtask.

Acceptance criteria:
- Closest target selection considers non-station ships from both sides.
- Planets, stations, and non-ship objects are ignored by seeker targeting.
- Equal-distance target ties use automatic RNG through a narrow helper.
- Initial facing can adjust up to three hexsides toward the selected target.
- Per-hex movement turns at most one hexside and greedily reduces distance where possible.
- Movement allowances progress as 2, 4, 6, 8, 10, and 12.
- Helpers can be tested without wx runtime.
- Any new `FHexMap` helper remains generic and non-wx.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- If `FHexMap` changes, also build/run core tests with `cd tests && make && ./SSWTests` when practical.

Tester test-file location guidance:
- Seeker targeting/path coverage belongs in a focused `tests/tactical/FTacticalSeekerMovementTest.*` fixture or existing tactical mechanics tests.
- Generic hex helper coverage belongs under `tests/core/*` if `FHexMap` is changed.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-007/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-008 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task to implement:
Wire active seeker movement resolution into seeker activation completion. This subtask should move active seekers owned by the moving player, remove harmless expired seekers, and produce detonation/contact outcomes for later damage integration. Keep the actual damage/ICM/report resolution limited to a clear seam that TSM-009 will implement or complete.

Acceptance criteria:
- Completing seeker activation resolves active seekers owned by the moving player before normal ship movement.
- Active seekers already sharing a ship hex produce contact outcomes before movement.
- Seekers entering a ship hex during movement stop and produce contact outcomes.
- Seekers that survive update hex, heading, and next movement allowance state.
- Seekers reaching the 12-hex movement step expire and are removed if they do not contact a ship.
- Enemy active seekers are not moved during the current player's seeker step.
- The resolution seam is model-owned and wx-free.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.

Tester test-file location guidance:
- Active seeker resolution coverage belongs in `tests/tactical/FTacticalSeekerMovementTest.*` or a focused new tactical seeker fixture.
- Phase transition coverage belongs in existing tactical mechanics/source-contract tests.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-008/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-009 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalCombatReport.h`
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `src/weapons/FSeekerMissileLauncher.cpp`

Task to implement:
Resolve seeker detonation outcomes through the existing weapon, ICM, and tactical report machinery. A detonation should create a temporary `FWeapon::SM`, target the selected ship, run eligible ICM allocation, fire the weapon, append seeker damage reports, show an immediate summary through the tactical UI seam, then clear destroyed ships after the summary path.

Acceptance criteria:
- Seeker detonations create temporary `FWeapon::SM` attacks with parent `NULL`.
- Same-hex target selection chooses highest `getMaxHP()` and uses RNG for same-max-HP ties.
- ICM allocation uses the existing `ITacticalUI::runICMSelection(...)` path.
- The seeker `-8/ICM` modifier is applied through existing weapon/defense mechanics.
- Seeker damage reports have seeker-specific report labels and are not mislabeled as mine damage.
- Multiple seeker detonations from one seeker step are batched into one immediate report where practical.
- Destroyed-ship cleanup happens after the immediate summary path.
- Detonated seekers are removed exactly once.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Build GUI tests with `cd tests/gui && make` if UI summary behavior is touched.
- If a display or xvfb is available, run `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Seeker damage/report coverage belongs in a focused tactical seeker damage fixture and `tests/tactical/FTacticalCombatReportTest.*` if report labels change.
- ICM interaction coverage can mirror mine damage flow tests in `tests/tactical/FTacticalMineDamageFlowTest.*`.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-009/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

### TSM-010 Prompt

Your role is 'implementer'. Your task is as follows:

Allowed files:
- `include/tactical/FTacticalGame.h`
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

Task to implement:
Integrate active seeker contact into normal ship movement finalization. When a ship path enters a hex containing an active seeker, resolve seeker detonation through the TSM-009 damage/report seam before mine damage is applied. Inactive seekers must remain ignored by movement contact checks.

Acceptance criteria:
- Ship movement paths are checked for active seeker hex contact during movement finalization.
- Inactive seeker hexes do not trigger contact or damage.
- Ship-triggered seeker damage resolves before mine damage in `completeMovePhase()`.
- Detonated seekers are removed exactly once.
- Destroyed ships do not leave stale occupancy, turn-info, or current selection state.
- Existing mine damage still applies once per movement completion after seeker-contact resolution.
- Phase progression to defensive fire remains correct after seeker contact and mine damage.

Validation guidance:
- Build and run `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Run the GUI suite under xvfb when available if movement UI behavior changes: `cd tests/gui && xvfb-run -a ./GuiTests`.

Tester test-file location guidance:
- Ship-path contact coverage belongs in a focused tactical seeker contact fixture or `tests/tactical/FTacticalMineDamageFlowTest.*` adjacent movement-finalization tests.
- Cleanup/order source-contract checks can extend existing tactical movement and damage-flow tests.

Artifact guidance:
Write implementation artifacts under `artifacts/tactical-seeker-missiles/TSM-010/` using the active implementer role conventions.

Startup behavior:
Continue past preflight when blockers are absent.

Completion gate:
Do not report success unless all required artifacts exist and all changes are committed.

## Output Artifact Path

`plans/tactical-seeker-missiles-plan.md`
