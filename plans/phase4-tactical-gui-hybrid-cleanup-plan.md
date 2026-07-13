# Phase 4 — Tactical GUI Hybrid Cleanup Plan

**Roadmap source:** `doc/synthesized-roadmap.md` line 701–702 (Part III "Now" item 4):
> *Bind-once buttons + `updateForPhase()` in `FBattleDisplay`; paint-DC fix in
> `FBattleBoard`; delete dead files/commented-out code.*

This phase is the near-term slice of §3.5 option **C (Hybrid)** cleanup for the tactical
GUI. It resolves finding **H9** (button connect/show lifecycle driven by `m_first` inside the
paint path) and finding **H7** (paint-DC mixing), plus the §3.6 dead-code / commented-out-code
cleanup. It deliberately excludes the follow-on hybrid items (shared `drawClickableRowList(...)`
row-list renderer, DPI/`FromDIP` pass) and the facade slimming, which remain later work.

**Baseline:** current `master` (clean). All file:line references below were verified directly
against the working tree during planning.

## Output Artifact Path

This plan is written to the repository-root-relative path
`plans/phase4-tactical-gui-hybrid-cleanup-plan.md`. Per-subtask Coordinator/Implementer artifacts
go under `artifacts/phase4-tactical-gui-hybrid-cleanup/<subtask-id>/` (e.g.
`artifacts/phase4-tactical-gui-hybrid-cleanup/P4-1/`).

---

## Confirmed Repository Facts

- **H9 — `FBattleDisplay` button lifecycle runs inside paint.**
  - `m_first` is declared at `include/tactical/FBattleDisplay.h:111` and is read/written at ~20
    sites in `src/tactical/FBattleDisplay.cpp` (initialised `true` in the ctor at line 105;
    toggled inside `drawGetSpeed`, `drawMoveShip`, `drawDefensiveFire`, `drawAttackFire`,
    `drawPlaceMines`, `drawPlaceSeekers`, `drawSeekerActivation`, and their Done handlers).
  - Seven phase buttons are created in the ctor (`src/tactical/FBattleDisplay.cpp:128-134`) and
    hidden (`:171-177`): `m_button1` ("Set Speed"), `m_buttonMoveDone`,
    `m_buttonDefensiveFireDone`, `m_buttonOffensiveFireDone`, `m_buttonMinePlacementDone`,
    `m_buttonSeekerPlacementDone`, `m_buttonSeekerActivationDone`
    (`include/tactical/FBattleDisplay.h:109,113,115,117,123,125,127`).
  - Each button's click handler is `Connect(...)`ed + `Show()`n **inside** the paint-driven
    `draw*()` method for its phase (gated by `m_first`), and `Disconnect(...)`ed + `Hide()`n
    inside the corresponding Done handler (e.g. `:1025-1027`, `:1043-1047`, `:1157-1160`,
    `:1359-1362`, `:1395-1398`, `:1420-1423`, `:1711-1714`, `:1848-1851`, `:1940-1947`).
  - The two end-of-move turn buttons `m_buttonTurnLeft`/`m_buttonTurnRight` (children of
    `m_turnButtonPanel`, `include/tactical/FBattleDisplay.h:129,131,133`; created
    `src/tactical/FBattleDisplay.cpp:139-148`) follow a parallel `IsShown()`-gated
    Connect/Show / Disconnect/Hide pattern, also inside paint (`draw()` at `:736-741`,
    `:794-799`; `drawMoveShip` at `:1236-1246`; `drawCurrentShipStats` at `:1364-1369`).
  - `FBattleScreen` owns `m_display` (`include/tactical/FBattleScreen.h:683`) and exposes the
    transition seams `setState(int)` (`:158`), `setPhase(int)` (`:208`), `onSize(...)` (`:678`),
    and `reDraw()` (`:210`). There is **no** `updateForPhase()` today.
  - Existing GUI live coverage asserts the observable button states: **TMF-05** (turn-button
    shown/enable states in/out of `PH_MOVE`) and **TMF-06** (Done-button dialog suppression) in
    `tests/gui/TacticalGuiLiveTest.cpp`.

- **H7 — paint-DC mixing.**
  - `FBattleBoard::onPaint` (`src/tactical/FBattleBoard.cpp:134-135`) constructs a `wxPaintDC`
    and calls `draw(dc)` (`:95`). `draw()` calls `drawShips()` (`:106`, **takes no DC**) and
    `drawSeekerMissiles(dc)` (`:108`).
  - `drawShips()` (`:269`) renders ship images through `drawCenteredOnHex(...)` (`:248`), which
    constructs a **throwaway `wxClientDC dc(this)` at line 249** — so ships/images bypass the
    paint DC, the update-region clip, and z-order. This is the single biggest GUI portability
    landmine (undefined on wxGTK3-Wayland / wxOSX).
  - `FGamePanel::onPaint` (`src/FGamePanel.cpp:18-32`) is the sibling bug: on `LINUX` it uses
    `wxClientDC`, only using `wxPaintDC` on other platforms, and carries a dead commented-out
    draw block (`:27-31`). (User confirmed this file is in-scope.)

- **Dead code / commented-out code (§3.6).**
  - `src/FMainBattleSimFrame.cpp` is **0 bytes**, has no header, and is **not** referenced by
    `src/Makefile` (explicit `OBJS = FApp.o Frontier.o FMainFrame.o FGamePanel.o`,
    `BS_OBJS = FBattleSimApp.o BattleSim.o`), nor by `SSW.sln`/vcxproj. Its only mention is a
    17-line commented-out event-table ghost block at `src/BattleSim.cpp:16-34`.
  - ~120 commented-out `std::cerr` lines exist across ~22 files (top offenders:
    `src/gui/SatharFleetsGUI.cpp` 23, `src/gui/UPFUnattachedGUI.cpp` 15,
    `src/strategic/FGame.cpp` 12, `src/gui/SelectCombatGUI.cpp` 11, `src/strategic/FPlayer.cpp`
    9, `src/tactical/FBattleDisplay.cpp` 7, `src/gui/BattleResultsGUI.cpp` 7, …).
  - Live `std::cerr` debug traces: `src/FMainFrame.cpp:188` ("Entered onShowPlayers()") and
    `src/gui/SelectCombatGUI.cpp:253,261` ("Adding Fleet …").
  - User-visible string typos: "destoryed" + "Statisitics" (`src/gui/BattleResultsGUI.cpp:35`),
    "woud" (`src/gui/TransferShipsGUI.cpp:104`), "captial" (`src/gui/SatharFleetsGUI.cpp:41`).

## Resolved Design Decisions (confirmed with user)

1. **Paint-DC scope:** fix **both** `FBattleBoard` **and** `FGamePanel::onPaint` (fully close H7).
2. **Buffering:** **include** `wxAutoBufferedPaintDC` in `FBattleBoard` (§3.5 option C(iv)).
3. **Cleanup breadth:** core dead-code removal **plus** removing the live `std::cerr` debug traces
   **plus** fixing the user-visible string typos. **Excluded:** the `placeMineAtHex` dead branch
   (§3.4 point 7) — it is retained to satisfy source-inspection tests and must wait for the
   Phase 7 test burn-down.

## Assumptions

- `updateForPhase()` will be a new **public, non-paint** method on `FBattleDisplay`, invoked by
  `FBattleScreen::setState`/`setPhase`/`onSize` (the existing transition seams). *Assumed* wiring
  location; the implementer confirms against the actual call sites.
- Removing double-`Refresh()`/paint-time mutation will not change the *observable* per-phase
  button state; the TMF-05/TMF-06 contracts encode the required invariants and must stay green.
- The VS project files use wildcard globs and need no manual edit when the empty
  `FMainBattleSimFrame.cpp` is deleted; *assumed* — implementer confirms no dangling reference.

## Security Review

None of the three subtasks touches trust boundaries, authentication/authorization, secret
handling, untrusted-input parsing, sensitive-data exposure, or destructive operations. This is a
GUI rendering/lifecycle refactor plus dead-code deletion. **No subtask requires specialist
security review.**

---

## Subtasks

Artifact base directory (repository-root-relative): `artifacts/phase4-tactical-gui-hybrid-cleanup/`
Per-subtask directory: `artifacts/phase4-tactical-gui-hybrid-cleanup/<subtask-id>/`

### P4-1 — Bind-once buttons + `updateForPhase()` in `FBattleDisplay` (H9)

**Goal:** Bind all `FBattleDisplay` button handlers exactly once at construction and drive only
Show/Enable/position per phase from a new non-paint `updateForPhase()`, removing the `m_first`
paint-time connect/show/hide machinery.

**Allowed files:**
- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`
- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`

**Acceptance criteria (implementation outcomes):**
1. `m_first` is removed from `include/tactical/FBattleDisplay.h`; no `m_first` reference remains
   in `src/tactical/FBattleDisplay.cpp`.
2. All seven phase buttons (`m_button1`, `m_buttonMoveDone`, `m_buttonDefensiveFireDone`,
   `m_buttonOffensiveFireDone`, `m_buttonMinePlacementDone`, `m_buttonSeekerPlacementDone`,
   `m_buttonSeekerActivationDone`) and both turn buttons (`m_buttonTurnLeft`,
   `m_buttonTurnRight`) have their click handlers bound **exactly once** during construction. No
   `Connect(...)`/`Disconnect(...)` of any of these handlers remains inside any `draw*()` method
   or the paint path.
3. A new public non-paint method `FBattleDisplay::updateForPhase()` exists that sets every
   button's Show/Hide + Enable state and the turn-panel visibility/position based solely on the
   current `FBattleScreen` state/phase and model queries (`isMoveComplete()`,
   `canApplyEndOfMoveTurnLeft()/Right()`, ship-selected). It performs no drawing and creates no
   DC.
4. `updateForPhase()` is invoked from `FBattleScreen::setState`, `FBattleScreen::setPhase`, and
   the resize path (`FBattleScreen::onSize`), and **not** from `FBattleDisplay::draw()` or any
   `draw*()` method.
5. Per-phase parity holds: in each phase exactly the correct Done/Set-Speed button is shown and
   enabled and all others are hidden, matching pre-refactor behavior; clicking a phase button
   while its phase is active routes to the same handler and produces the same model effect
   (Set Speed advances setup; Movement Done finalizes movement; Defensive/Offensive Fire Done
   complete their phases; Mine/Seeker Placement + Seeker Activation Done complete theirs;
   Turn Left/Right apply the end-of-move single facing change via
   `FBattleScreen::applyEndOfMoveTurn(±1)`).
6. Turn-button invariants (TMF-05) preserved: `m_turnButtonPanel` shown with `m_buttonTurnLeft`/
   `m_buttonTurnRight` enable states mirroring `canApplyEndOfMoveTurnLeft()/Right()` only in
   `PH_MOVE` with a ship selected and minimum move satisfied; hidden outside `PH_MOVE` or when
   state is not `BS_Battle`. Done-button dialog-suppression (TMF-06) behavior unchanged.
7. Doxygen headers per `AGENTS.md`: add a full header block to the new `updateForPhase()` and
   update `Last Modified` + `@author` on every function whose body changes (author =
   `<model_name> (<reasoning level>)`).

**Validation guidance:**
- Build from repo root and run the full gate: `make check` (builds `all`, then runs `SSWTests`,
  `TacticalTests`, and `GuiTests` under `xvfb-run`, stopping on first failure).
- Targeted GUI check: `cd tests/gui && make && xvfb-run -a ./GuiTests` — `TacticalGuiLiveTest`
  (TMF-05/TMF-06) must pass. Targeted tactical: `cd tests/tactical && make && ./TacticalTests`.

**Tester handoff (test-file locations):** behavioral GUI coverage lives in
`tests/gui/TacticalGuiLiveTest.{h,cpp}` (registered in `tests/gui/GuiTests.cpp`); tactical model
coverage in `tests/tactical/*` (registered in `tests/tactical/TacticalTests.cpp`). New behavioral
tests must drive real phase transitions (via `setState`/`setPhase` and offscreen `wxMemoryDC`
paint) and assert observed `IsShown()`/`IsEnabled()`/model state — source-string inspection may
only supplement, never substitute (per `AGENTS.md` behavioral-verification policy).

**Documentation Impact:** Likely `AGENTS.md` Contributor-Notes bullet on `FBattleDisplay` (add the
bind-once + `updateForPhase()` lifecycle contract; note that button Connect/Show no longer occurs
in paint). Possibly `doc/DesignNotes.md`. `doc/test-contracts.md` only if a TMF contract's wording
changes. (Documenter owns these; no doc subtask.)

**Security review: not required.**

---

### P4-2 — Paint-DC fix + `wxAutoBufferedPaintDC` in `FBattleBoard` and `FGamePanel` (H7)

**Goal:** Render the entire tactical scene through a single buffered paint DC and eliminate
throwaway `wxClientDC` usage in the paint path, in both `FBattleBoard` and `FGamePanel`.

**Allowed files:**
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleBoard.cpp`
- `include/FGamePanel.h`
- `src/FGamePanel.cpp`

**Acceptance criteria (implementation outcomes):**
1. `FBattleBoard::drawCenteredOnHex(...)` no longer constructs a `wxClientDC`; it draws onto a
   `wxDC&` supplied by its caller. The paint DC is threaded
   `onPaint` → `draw(wxDC&)` → `drawShips(wxDC&)` → `drawCenteredOnHex(..., wxDC&)`, and any other
   `drawCenteredOnHex` caller in the paint path passes its DC through.
2. No `wxClientDC` remains anywhere in `src/tactical/FBattleBoard.cpp`.
3. `FBattleBoard::onPaint` constructs a `wxAutoBufferedPaintDC`, and the board calls
   `SetBackgroundStyle(wxBG_STYLE_PAINT)` (in its constructor) so buffering is valid; all scene
   elements — grid, ships, seeker missiles, routes, ranges, targets, mines, overlays — render
   through that single DC in one pass.
4. `FGamePanel::onPaint` uses a paint DC on all platforms (the `#ifdef LINUX` `wxClientDC` branch
   is removed), and the trailing dead commented-out draw block (`src/FGamePanel.cpp:27-31`) is
   deleted. The `SetClientSize(...)` call may remain if functionally required, but no `wxClientDC`
   is used in the paint handler.
5. Ships, seeker missiles, and centered images still render at the correct hex positions and
   rotations; no scene element is dropped or mis-clipped relative to pre-refactor rendering.
6. Doxygen headers per `AGENTS.md`: update `Last Modified` + `@author` on every changed function
   (`onPaint`, `draw`, `drawShips`, `drawCenteredOnHex`, `FGamePanel::onPaint`, ctors as touched);
   update signatures' `@param` where a `wxDC&` parameter is added.

**Validation guidance:**
- Full gate from repo root: `make check` (GUI suite runs under `xvfb-run`).
- Deterministic render coverage: `cd tests/gui && make && xvfb-run -a ./GuiTests` — offscreen
  `wxMemoryDC` assertions for tactical board rendering must pass. Also `cd tests/tactical && make
  && ./TacticalTests`.
- Manual smoke (optional, if a display is available): launch the tactical flow and confirm ships/
  seekers paint and survive expose/resize without flicker.

**Tester handoff (test-file locations):** tactical rendering behavioral coverage belongs in
`tests/gui/TacticalGuiLiveTest.{h,cpp}` (offscreen `wxMemoryDC` paint assertions) and, where the
renderer delegation is model-observable, `tests/tactical/FTacticalBattleBoardRendererDelegationTest.*`.
Behavioral pixel/region assertions are required; source-string checks may only supplement.

**Documentation Impact:** Likely `AGENTS.md` (note the single-buffered-paint-DC rendering contract
for `FBattleBoard` and that `FGamePanel::onPaint` uses a paint DC on all platforms). Possibly
`doc/DesignNotes.md` rendering notes. (Documenter owns; no doc subtask.)

**Security review: not required.**

---

### P4-3 — Delete dead files / commented-out code, live debug traces, and fix typos (§3.6)

**Goal:** Remove the empty dead source file and its ghost reference, strip commented-out and live
debug `std::cerr` output, and correct user-visible string typos — with no behavior change beyond
removed debug output and corrected display text.

**Allowed files:**
- `src/FMainBattleSimFrame.cpp` (delete)
- `src/BattleSim.cpp`
- `src/FMainFrame.cpp`
- `src/gui/SelectCombatGUI.cpp`
- `src/gui/BattleResultsGUI.cpp`
- `src/gui/TransferShipsGUI.cpp`
- `src/gui/SatharFleetsGUI.cpp`
- `src/gui/UPFUnattachedGUI.cpp`
- `src/strategic/FGame.cpp`
- `src/strategic/FPlayer.cpp`
- `src/strategic/FFleet.cpp`, `src/strategic/FMap.cpp`, `src/strategic/FSystem.cpp`,
  `src/strategic/FPlanet.cpp`, `src/strategic/FJumpRoute.cpp`
- `src/gui/ICMSelectionGUI.cpp`, `src/gui/SystemDialogGUI.cpp`, `src/gui/WXPlayerDisplay.cpp`,
  `src/gui/WXMapDisplay.cpp`
- `src/ships/FVehicle.cpp`
- `src/core/FHexPath.cpp`, `src/core/FHexMap.cpp`, `src/core/FGameConfig.cpp`
- `src/tactical/FBattleScreen.cpp` (commented-`std::cerr` removal only)

> **Do NOT edit** `src/tactical/FBattleDisplay.cpp`, `src/tactical/FBattleBoard.cpp`,
> `src/FGamePanel.cpp`, or `include/tactical/FBattleDisplay.h` in this subtask — those files are
> owned by P4-1/P4-2. Their in-file commented-`std::cerr` / dead-comment removal is handled there
> (P4-1 may drop the 7 commented `std::cerr` lines in `FBattleDisplay.cpp`; P4-2 removes the dead
> block in `FGamePanel.cpp`). This subtask runs after both to avoid merge overlap.

**Acceptance criteria (implementation outcomes):**
1. `src/FMainBattleSimFrame.cpp` is deleted from the repository; the build still links (no
   Makefile/vcxproj reference was pointing at it — confirm no dangling reference remains).
2. The commented-out `FMainBattleSimFrame` event-table ghost block in `src/BattleSim.cpp`
   (~lines 16-34) is removed.
3. All commented-out `std::cerr` debug lines in the allowed files are removed (the ~120-line
   corpus, excluding the P4-1/P4-2-owned files listed above). No live/functional code is altered
   by these removals.
4. The live `std::cerr` debug-trace statements are removed: `src/FMainFrame.cpp:188`
   ("Entered onShowPlayers()") and `src/gui/SelectCombatGUI.cpp:253,261` ("Adding Fleet …").
5. User-visible string typos are corrected: "destoryed"→"destroyed" and "Statisitics"→"Statistics"
   in `src/gui/BattleResultsGUI.cpp:35`; "woud"→"would" in `src/gui/TransferShipsGUI.cpp:104`;
   "captial"→"capital" in `src/gui/SatharFleetsGUI.cpp:41`. The internal identifier
   `captialShipCount` and comment-only typos are out of scope (leave unchanged to keep the diff
   tight; do not rename symbols).
6. The project compiles and `make check` passes; the only behavior change is removed debug output
   and corrected on-screen text. No signature or logic change.

**Validation guidance:**
- Full gate from repo root: `make check`. Also confirm a clean build of both executables:
  `make` (builds `SSW` and `BattleSim`).
- Grep-verify no regression: no remaining commented-out `std::cerr` in the allowed files; no
  live debug-trace `std::cerr` at the four listed sites; the four corrected strings render.

**Tester handoff (test-file locations):** this subtask changes no runtime behavior beyond removed
debug output, so it needs no new behavioral test; if the Tester adds any coverage it belongs with
the owning module under `tests/<module>/`. Do not create a subtask whose purpose is routine test
updates.

**Documentation Impact:** None expected in `doc/` or `AGENTS.md` for the deletions/typo fixes.
(The Phase-4 RESOLVED roadmap update is captured under Overall Documentation Impact below.)

**Security review: not required.**

---

## Dependency Ordering & Parallelization

- **P4-1** (files: `FBattleDisplay.{h,cpp}`, `FBattleScreen.{h,cpp}`) and **P4-2** (files:
  `FBattleBoard.{h,cpp}`, `FGamePanel.{h,cpp}`) touch **disjoint** file sets and share no design
  decisions → **safe to run in parallel.**
- **P4-3** must run **after both P4-1 and P4-2 complete and merge.** Its commented-`std::cerr`
  cleanup spans `src/tactical/FBattleScreen.cpp` (touched by P4-1) and it deliberately excludes
  the P4-1/P4-2-owned files; sequencing it last avoids merge conflicts and lets P4-2's
  `FGamePanel` dead-block removal settle first.

**Recommended sequence:** `{ P4-1 ∥ P4-2 } → P4-3`.
Conservative fully-serial fallback (if parallel execution is undesirable): `P4-1 → P4-2 → P4-3`.

---

## Overall Documentation Impact

- **`doc/synthesized-roadmap.md`:** mark Phase 4 resolved — annotate finding **H9** (line 127),
  finding **H7** (line 125), the §3.6 dead-code bullet (lines ~433-435), and Part III "Now" item 4
  (lines 701-702) as **RESOLVED**, each referencing `artifacts/phase4-tactical-gui-hybrid-cleanup/`
  (mirroring how Phase 1/2/3 items were annotated).
- **`AGENTS.md`:** update the `FBattleDisplay`/`FBattleBoard` Contributor-Notes bullets to record
  the bind-once + `updateForPhase()` button-lifecycle contract and the single buffered-paint-DC
  rendering contract (paint mutation no longer occurs; no `wxClientDC` in the paint path).
- **`doc/DesignNotes.md`:** optional — note the paint-DC / button-lifecycle change if the design
  notes describe tactical rendering.
- **`doc/test-contracts.md`:** only if a TMF-05/TMF-06 contract's *wording* changes (the behavior
  must not).

All documentation work is performed by the Documenter stage of the Coordinator workflow — no
standalone documentation subtask is created here.

---

## Out of Scope (explicitly deferred)

- Shared `drawClickableRowList(...)` renderer for the four hand-rolled row lists (§3.5 C(iii)).
- DPI/`FromDIP` awareness pass (§3.5 option A/C).
- `FBattleScreen` facade slimming / expose-model refactor (§3.5 (a)/(b)).
- Migrating `FBattleScreen`/`FBattleBoard`/`FBattleDisplay` to `src/gui` (roadmap "deliberate
  decision", churn-heavy).
- Removing the `placeMineAtHex` dead branch (§3.4 point 7) — depends on the Phase 7 test
  burn-down.
- Renaming the internal `captialShipCount` identifier and fixing comment-only typos.

---

## Implementer Prompts (Coordinator-ready)

### P4-1 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Resolve finding H9: bind all FBattleDisplay button handlers exactly once at construction
and drive only Show/Enable/position per phase from a new non-paint updateForPhase(),
removing the m_first paint-time connect/show/hide machinery.

Allowed files:
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp

Context (verified against the working tree):
- m_first is declared at include/tactical/FBattleDisplay.h:111 and read/written at ~20 sites
  in src/tactical/FBattleDisplay.cpp (ctor init at :105).
- Seven phase buttons are created at src/tactical/FBattleDisplay.cpp:128-134 and hidden at
  :171-177: m_button1 ("Set Speed"), m_buttonMoveDone, m_buttonDefensiveFireDone,
  m_buttonOffensiveFireDone, m_buttonMinePlacementDone, m_buttonSeekerPlacementDone,
  m_buttonSeekerActivationDone. Each is currently Connect()+Show()n inside its phase's
  draw*() method (gated by m_first) and Disconnect()+Hide()n in the matching Done handler
  (e.g. :1025-1027, :1043-1047, :1157-1160, :1359-1362, :1395-1398, :1420-1423, :1711-1714,
  :1848-1851, :1940-1947).
- The two turn buttons m_buttonTurnLeft/m_buttonTurnRight (children of m_turnButtonPanel,
  created :139-148) use a parallel IsShown()-gated Connect/Show pattern inside paint
  (draw() :736-741, :794-799; drawMoveShip :1236-1246; drawCurrentShipStats :1364-1369).
- FBattleScreen owns m_display (include/tactical/FBattleScreen.h:683) and exposes setState(int)
  (:158), setPhase(int) (:208), onSize(...) (:678). There is no updateForPhase() today.

Implement:
1. Remove m_first from the header and all use sites in the .cpp.
2. Bind all seven phase-button click handlers AND both turn-button click handlers exactly once
   during construction; remove every Connect()/Disconnect() of these handlers from draw*() and
   the paint path.
3. Add a new public non-paint method FBattleDisplay::updateForPhase() that sets each button's
   Show/Hide + Enable and the turn-panel visibility/position purely from the current
   FBattleScreen state/phase and model queries (isMoveComplete(), canApplyEndOfMoveTurnLeft()/
   Right(), ship-selected). It must not draw and must not create a DC.
4. Invoke updateForPhase() from FBattleScreen::setState, FBattleScreen::setPhase, and the
   resize path (onSize) — never from draw()/draw*().

Acceptance criteria:
- No m_first anywhere; handlers bound once in the ctor; no button Connect/Disconnect in paint.
- updateForPhase() exists (public, non-paint) and is wired into setState/setPhase/onSize.
- Per-phase parity: exactly the correct Done/Set-Speed button shown+enabled per phase, all
  others hidden; clicking a phase button routes to the same handler and same model effect as
  before (Set Speed advances setup; Movement Done finalizes movement; Defensive/Offensive Fire
  Done and Mine/Seeker Placement + Seeker Activation Done complete their phases; Turn Left/Right
  apply the end-of-move facing via FBattleScreen::applyEndOfMoveTurn(+/-1)).
- TMF-05 turn-button invariants preserved (shown/enabled only in PH_MOVE with a ship selected
  and min-move satisfied; hidden outside PH_MOVE or when state != BS_Battle); TMF-06 Done-button
  dialog-suppression unchanged.
- Add a full Doxygen header to updateForPhase(); update Last Modified + @author (=
  <model_name> (<reasoning level>)) on every function whose body changed, per AGENTS.md.

Validation:
- From repo root: make check (builds all, runs SSWTests, TacticalTests, GuiTests under
  xvfb-run). Targeted: cd tests/gui && make && xvfb-run -a ./GuiTests (TacticalGuiLiveTest
  TMF-05/TMF-06 must pass); cd tests/tactical && make && ./TacticalTests.

Tester handoff: behavioral GUI coverage lives in tests/gui/TacticalGuiLiveTest.{h,cpp}
(registered in tests/gui/GuiTests.cpp); tactical model coverage in tests/tactical/* (registered
in tests/tactical/TacticalTests.cpp). New tests must drive real phase transitions (setState/
setPhase + offscreen wxMemoryDC paint) and assert observed IsShown()/IsEnabled()/model state;
source-string inspection may only supplement behavioral assertions, per AGENTS.md.

Artifacts: write implementer artifacts to artifacts/phase4-tactical-gui-hybrid-cleanup/P4-1/
(repository-root-relative).

Continue past preflight and proceed with implementation when no blocking input is missing.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P4-2 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Resolve finding H7: render the entire tactical scene through a single buffered paint DC and
eliminate throwaway wxClientDC usage in the paint path, in both FBattleBoard and FGamePanel.

Allowed files:
- include/tactical/FBattleBoard.h
- src/tactical/FBattleBoard.cpp
- include/FGamePanel.h
- src/FGamePanel.cpp

Context (verified against the working tree):
- FBattleBoard::onPaint (src/tactical/FBattleBoard.cpp:134-135) builds a wxPaintDC and calls
  draw(dc) (:95). draw() calls drawShips() (:106, takes NO DC) and drawSeekerMissiles(dc) (:108).
- drawShips() (:269) renders ship images via drawCenteredOnHex(...) (:248), which constructs a
  throwaway wxClientDC dc(this) at line 249 — ships bypass the paint DC, the update-region clip,
  and z-order.
- FGamePanel::onPaint (src/FGamePanel.cpp:18-32) uses wxClientDC on LINUX and only wxPaintDC on
  other platforms, and has a dead commented-out draw block at :27-31.

Implement:
1. Change drawCenteredOnHex(...) to draw onto a wxDC& passed by the caller; delete its internal
   wxClientDC. Thread the paint DC onPaint -> draw(wxDC&) -> drawShips(wxDC&) ->
   drawCenteredOnHex(..., wxDC&); update any other paint-path caller of drawCenteredOnHex to
   pass its DC through.
2. Ensure no wxClientDC remains anywhere in src/tactical/FBattleBoard.cpp.
3. Change FBattleBoard::onPaint to construct a wxAutoBufferedPaintDC, and call
   SetBackgroundStyle(wxBG_STYLE_PAINT) in the FBattleBoard constructor so buffering is valid;
   all scene elements (grid, ships, seekers, routes, ranges, targets, mines, overlays) must
   render through that single DC in one pass.
4. Change FGamePanel::onPaint to use a paint DC on all platforms (remove the #ifdef LINUX
   wxClientDC branch) and delete the trailing dead commented-out draw block. SetClientSize(...)
   may remain if functionally required, but no wxClientDC in the paint handler.

Acceptance criteria:
- drawCenteredOnHex draws on a passed-in wxDC&; no wxClientDC in FBattleBoard.cpp.
- FBattleBoard::onPaint uses wxAutoBufferedPaintDC with SetBackgroundStyle(wxBG_STYLE_PAINT);
  whole scene renders in one pass through that DC.
- FGamePanel::onPaint uses a paint DC on all platforms; dead comment block removed.
- Ships, seeker missiles, and centered images render at correct hex positions and rotations; no
  scene element dropped or mis-clipped vs pre-refactor.
- Update Last Modified + @author (= <model_name> (<reasoning level>)) on every changed function;
  add @param for any new wxDC& parameters, per AGENTS.md.

Validation:
- From repo root: make check (GUI suite under xvfb-run). Deterministic render coverage:
  cd tests/gui && make && xvfb-run -a ./GuiTests (offscreen wxMemoryDC tactical-board render
  assertions must pass); cd tests/tactical && make && ./TacticalTests.

Tester handoff: tactical rendering behavioral coverage belongs in
tests/gui/TacticalGuiLiveTest.{h,cpp} (offscreen wxMemoryDC paint assertions) and, where renderer
delegation is model-observable, tests/tactical/FTacticalBattleBoardRendererDelegationTest.*.
Behavioral pixel/region assertions are required; source-string checks may only supplement, per
AGENTS.md.

Artifacts: write implementer artifacts to artifacts/phase4-tactical-gui-hybrid-cleanup/P4-2/
(repository-root-relative).

Continue past preflight and proceed with implementation when no blocking input is missing.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P4-3 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Remove dead code and debug output and fix user-visible string typos, with no behavior change
beyond removed debug output and corrected on-screen text. This subtask must run AFTER P4-1 and
P4-2 are merged.

Allowed files (edit unless noted):
- src/FMainBattleSimFrame.cpp  (DELETE this empty file)
- src/BattleSim.cpp
- src/FMainFrame.cpp
- src/gui/SelectCombatGUI.cpp
- src/gui/BattleResultsGUI.cpp
- src/gui/TransferShipsGUI.cpp
- src/gui/SatharFleetsGUI.cpp
- src/gui/UPFUnattachedGUI.cpp
- src/strategic/FGame.cpp, src/strategic/FPlayer.cpp, src/strategic/FFleet.cpp,
  src/strategic/FMap.cpp, src/strategic/FSystem.cpp, src/strategic/FPlanet.cpp,
  src/strategic/FJumpRoute.cpp
- src/gui/ICMSelectionGUI.cpp, src/gui/SystemDialogGUI.cpp, src/gui/WXPlayerDisplay.cpp,
  src/gui/WXMapDisplay.cpp
- src/ships/FVehicle.cpp
- src/core/FHexPath.cpp, src/core/FHexMap.cpp, src/core/FGameConfig.cpp
- src/tactical/FBattleScreen.cpp  (commented-std::cerr removal only)

DO NOT edit src/tactical/FBattleDisplay.cpp, src/tactical/FBattleBoard.cpp, src/FGamePanel.cpp,
or include/tactical/FBattleDisplay.h — those are owned by P4-1/P4-2.

Context (verified against the working tree):
- src/FMainBattleSimFrame.cpp is 0 bytes, has no header, and is not referenced by src/Makefile
  (OBJS = FApp.o Frontier.o FMainFrame.o FGamePanel.o; BS_OBJS = FBattleSimApp.o BattleSim.o)
  or by SSW.sln/vcxproj. Its only mention is a commented-out event-table ghost block at
  src/BattleSim.cpp:16-34.
- ~120 commented-out std::cerr lines exist across the allowed files.
- Live std::cerr debug traces: src/FMainFrame.cpp:188 ("Entered onShowPlayers()") and
  src/gui/SelectCombatGUI.cpp:253,261 ("Adding Fleet ...").
- User-visible typos: "destoryed" + "Statisitics" at src/gui/BattleResultsGUI.cpp:35; "woud" at
  src/gui/TransferShipsGUI.cpp:104; "captial" at src/gui/SatharFleetsGUI.cpp:41.

Implement:
1. Delete src/FMainBattleSimFrame.cpp; confirm nothing in the build references it.
2. Remove the commented-out FMainBattleSimFrame event-table ghost block in src/BattleSim.cpp
   (~lines 16-34).
3. Remove all commented-out std::cerr debug lines in the allowed files (do not touch live/
   functional code); leave the P4-1/P4-2-owned files alone.
4. Remove the live std::cerr debug-trace statements at src/FMainFrame.cpp:188 and
   src/gui/SelectCombatGUI.cpp:253,261.
5. Fix user-visible string typos: "destoryed"->"destroyed" and "Statisitics"->"Statistics" in
   src/gui/BattleResultsGUI.cpp:35; "woud"->"would" in src/gui/TransferShipsGUI.cpp:104;
   "captial"->"capital" in src/gui/SatharFleetsGUI.cpp:41. Do NOT rename the internal
   captialShipCount identifier and do NOT touch comment-only typos.

Acceptance criteria:
- src/FMainBattleSimFrame.cpp deleted; build still links with no dangling reference.
- BattleSim.cpp ghost block removed; all commented-out std::cerr in the allowed files removed;
  the four live debug-trace std::cerr sites removed; the four display-string typos corrected.
- Project compiles; make check passes; only behavior change is removed debug output and
  corrected on-screen text. No signature or logic change.
- Update Last Modified + @author on any function header whose body is meaningfully changed, per
  AGENTS.md (pure comment/string-line deletions in a function may update the header author/date).

Validation:
- From repo root: make check; also make (builds SSW and BattleSim). Grep-verify: no remaining
  commented-out std::cerr in the allowed files; no live debug-trace std::cerr at the four listed
  sites; the four corrected strings present.

Tester handoff: this subtask changes no runtime behavior beyond removed debug output, so it needs
no new behavioral test; any coverage the Tester adds belongs with the owning module under
tests/<module>/.

Artifacts: write implementer artifacts to artifacts/phase4-tactical-gui-hybrid-cleanup/P4-3/
(repository-root-relative).

Continue past preflight and proceed with implementation when no blocking input is missing.
Do not report success unless all required artifacts exist and all changes are committed.
```
