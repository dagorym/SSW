# Reviewer Report — Phase 2: Rules-Correctness Fixes + Phase-1 Reviewer Follow-ups

**Reviewer:** Reviewer Agent (Claude Opus 4.8 (1M context), medium)
**Date:** 2026-07-11
**Branch reviewed:** `plan-phase2-rules-correctness-followups`
**Comparison base:** plan commit `37fb564b` → current `HEAD` (`1561188f`)
**Scope:** Final, read-only, feature-level review across all seven merged subtasks.

---

## Feature plan reviewed

- `plans/phase2-rules-correctness-followups-plan.md` (authoritative scope).

Two related bodies of correctness/robustness work, no new game mechanics, no
intentional save-format break:
- **A. Rules-correctness** (roadmap "Now" #2): T1 (disruptor range), T2/T3
  (to-hit defense resolution), S2/S3 (loss counters), RV (retreat-condition
  validation).
- **B. Phase-1 reviewer follow-ups:** F2 (`m_destroyed` leak), F4/H15 (save/open
  dialog cancel + path), F1 (deterministic RNG seed seam). F5 was applied by the
  planner with the plan commit (not a subtask). H2 and F2-serialization are
  explicitly **deferred** (save-format break).

## Inputs reviewed

- Governing plan (full).
- Delivered production diff (`37fb564b..HEAD`) across all 14 changed source/doc
  files.
- Per-subtask artifacts (implementer / tester / documenter / verifier reports +
  result JSON) under `artifacts/phase2-rules-correctness/P2-1..P2-7/`, including
  `history/pass-1/` for P2-1 and P2-4 remediation cycles.
- `doc/deferred-tasks.md`, `doc/synthesized-roadmap.md` annotations.
- Behavioral test fixtures across `tests/weapons`, `tests/ships`,
  `tests/strategic`, `tests/core`, `tests/gui`.
- Independent build + test execution (see below).

## Independent verification performed

- **Build:** `make` from repo root → exit 0 (clean, includes test binaries).
- **Full suite:** `./SSWTests` run **from `tests/`** → **OK (245 tests)**, exit 0.
- **GUI suite:** `cd tests/gui && make && xvfb-run -a ./GuiTests` → **OK (75 tests)**,
  exit 0 (independently confirms the four new P2-6 Save/Open live tests).
- **Non-editable files unchanged:** `git diff --name-only 37fb564b..HEAD` for
  `doc/rules/tactical_operations_manual.md`, `doc/rules/second-sathar-war-rules.md`,
  `AGENTS.md`, `CLAUDE.md`, `doc/DesignNotes.md` → empty (untouched).
- **No save wire-format change:** no subtask touched `save()`/`load()`; verified
  by diff inspection of `FGame.cpp`, `FPlayer.cpp`, `FVehicle.cpp`.
- **All seven verifier verdicts:** PASS (P2-1..P2-7, `verifier_result.json`).

---

## Overall feature completeness

**Complete.** All seven subtasks are delivered, behaviorally tested, documented,
verified (PASS), and merged. Each plan acceptance criterion maps to observable
behavioral coverage, the two remediation cycles (P2-1 cosmetic citation, P2-4
unbounded-loop bound) landed correctly, the deferred items are recorded as
deferred (not gaps), and the roadmap annotations from all owning subtasks
coexist after the Coordinator's union merge with none lost.

### Per-subtask confirmation

- **P2-1 (T1):** `FDisruptorCannon` ctor `m_range=9` (was 12). Behavioral
  coverage: `FDisruptorCannonTest::testConstructor`,
  `testSetTargetAcceptsRangeAtMax`, `testSetTargetRejectsRangeBeyondMax`
  (9-accept / 10-reject clamp), plus `FTacticalBatteryRangeClampTest`. Pass-2
  remediation corrected a stale line citation only; code correct.

- **P2-2 (T2/T3):** New `FVehicle::resolveToHitModifier(FWeapon::Weapon)`
  enumerates operating defenses (RH + currently-raised, deduplicated), returns
  the minimum (most-effective) modifier unless an operating defense attracts the
  weapon (PS↔EB, ES↔PB, SS↔T/SM/M), in which case the attracting modifier is
  returned. `FWeapon::fire()` applies the masking-screen "fired out of the
  screen" override to lasers only and guards `m_parent` null (mines/seekers).
  Behavioral coverage: 7 `FWeaponDefenseResolutionTest` cases (most-effective,
  three attract overrides, laser-out, laser-in, parentless-mine) plus the
  required red/green discriminator
  `FTacticalAttackIntegrationTest::testMaskingScreenTorpedoOutResolvesAgainstTargetDefenseNotAttackerMSAndDoesNotHalveDamage`
  and `testParentlessLaserFireDoesNotCrashAndDoesNotHalveDamageAgainstNonMaskedTarget`.
  Null-safety confirmed: `m_currentDefense` invariant guarantees a non-null
  live defense (rebuilt on load), and the surviving deref sites are all
  `m_parent`-guarded.

- **P2-3 (S2/S3):** `cleanUpShips()` now counts fighters toward
  `m_lostSatharShips` (condition 3) and excludes fighters + militia from
  `m_lostTendaySathar`/`m_lostTendayUPF` on both sides via
  `countsTowardTenday = (getType()!="Fighter") && !fleet->isMilitia()`.
  Conditions 1/2 (`m_lostHC`/`m_lostAC`) and removal/ownership logic untouched.
  Behavioral coverage: 9 dedicated `FGameTest` cases (Sathar-fighter counting,
  tenday fighter/militia exclusion both sides, non-fighter/non-militia counting,
  and two condition-5 victory-dispatch cases with fighter/militia noise).

- **P2-4 (RV):** `FGame::init()` validates `selectRetreatCondition()` against
  `1..5`, re-prompting via `IStrategicUI` until valid, only when `m_ui != NULL`,
  bounded by `kMaxRetreatConditionPrompts = 1000`; on cap exhaustion
  `m_satharRetreat` is left at its default rather than a bogus value.
  Behavioral coverage: `FGameRetreatConditionValidationTest` (reprompt-until-
  valid, valid-immediately-no-extra-prompts, stored-value-drives-victory-
  dispatch, and the pass-2 `testInitBoundsRepromptLoopAndLeavesRetreatConditionUnset`
  cap test). Pass-2 remediation correctly added the bound flagged in pass-1.

- **P2-5 (F2):** `~FPlayer()` deletes and clears `m_destroyed`; sole-ownership
  contract documented on `addDestroyedShip()`/`m_destroyed`. Behavioral
  coverage: `FPlayerTest::testDestroyedShipFreedExactlyOnceOnPlayerDestruction`
  and `testSurvivingFleetShipNotDoubleDeletedByDestroyedList`. `doc/deferred-tasks.md`
  created with H2 and F2-serialization entries. No serialization added.

- **P2-6 (F4/H15):** `onSave()`/`onOpen()` check `ShowModal()`, no-op on Cancel,
  use `GetPath()` instead of `GetFilename()`; `onOpen()` defers `FGame::create`/
  `load` to the confirmed-OK path; post-load menu/turn logic preserved.
  Behavioral coverage: 4 `StrategicGuiLiveTest` cases (save-cancel-untouched,
  save-confirm-full-path, open-cancel-no-game, open-confirm-full-path-restores-
  state). GUI suite passes (75 tests). See NOTE-1 on discriminator strength.

- **P2-7 (F1):** `include/Frontier.h` adds `seedRandomExplicit(seed)` and
  `seedRandomFromClock()` (thin `srand` wraps); `FGame` ctor switches
  `srand(time(NULL))` → `seedRandomFromClock()`; `irand()` unchanged. Behavioral
  coverage: `FrontierTest` (seed→reproducible / differs-across-seeds) and the
  deterministic `FFleetTest::testDecTransitTime` seeded via the seam.

### Cross-subtask integration (serial `FGame.cpp` lane)

P2-3, P2-4, P2-7 all edit `src/strategic/FGame.cpp` / `include/strategic/FGame.h`
and were serialized in the α lane. Confirmed they coexist without regression:
P2-3's edit is confined to the `cleanUpShips()` counting block, P2-4's to the
`init()` retreat-condition assignment, P2-7's to the `FGame(IStrategicUI*)`
ctor seed call — three disjoint regions. The full suite (245) and GUI suite (75)
both pass on the merged branch, and the roadmap annotations for all three lane
subtasks (§5.4 S2/S3, "Notable Medium" RV, §3.7 Randomness F1) are present and
non-conflicting alongside the independent-lane annotations (T1/T2/T3 §6.1, F2
"Notable Medium", H15/F4).

### Deferred items and constraints

- H2 and F2-serialization are correctly recorded in `doc/deferred-tasks.md` as
  intentionally deferred (save-format break), **not** gaps in this plan. No
  save wire-format change was introduced by any subtask. Correct.
- The `FGameHeaderDependencyTest` cwd-relative "failure when launched outside
  `tests/`" is a known harness artifact, not a regression; running from `tests/`
  yields OK (245). Confirmed.

### Security judgment

The plan marked no subtask as requiring specialist security review. This is
reasonable: every change is local correctness / memory-safety / robustness with
no attacker-controlled trust boundary. The file dialogs read local, user-chosen
paths only (P2-6 in fact *reduces* risk by rejecting Cancel and using the
resolved full path); the RNG seam is a deterministic-test convenience over the
existing process RNG, not a security-relevant randomness source; the freed
`m_destroyed` fixes a leak/UAF class of bug in-process. No specialist Security
stage was warranted.

---

## Findings

### BLOCKING
- None.

### WARNING
- None.

### NOTE
- **NOTE-1 (P2-6 discriminator strength — disclosed, adequate).** The Tester
  transparently disclosed that of the four new Save/Open GUI tests, only
  `testMainFrameOnOpenCancelCreatesNoGameAndLeavesFrameConsistent` is a true
  red/green discriminator (it segfaults against the pre-fix `onOpen()`); the
  other three coincide pre/post-fix in the harness-observable scenario and
  therefore act as behavioral contract-locks rather than discriminators. This is
  inherent to the harness (a bare `GetFilename()` and a full `GetPath()` resolve
  identically when the chosen file is in the cwd, and an empty-name stream also
  fails to open). Combined with the unambiguous diff (explicit `wxID_OK` checks +
  `GetPath()`) and the genuine crashing discriminator on the highest-risk path,
  coverage is adequate at the feature level. No action required.
- **NOTE-2 (deferred save-format items remain open — by design).** H2 (persist
  active defense across save/load) and F2-serialization (serialize
  `FPlayer::m_destroyed`) are recorded in `doc/deferred-tasks.md` and remain
  open for the future save-format-hardening phase. They are out of scope for
  this plan; listed below as Planner handoffs so they are not lost.
- **NOTE-3 (RH-ownership assumption in `resolveToHitModifier`).** The helper
  treats every `FDefense` of type `RH` in `m_defenses` as always operating, per
  the plan's confirmed §2b decision-1 modeling ("reflective hull always operating
  if the ship owns one"). This matches the manual and the plan; noted only for
  traceability, not as a defect.

## Follow-up feature requests for planning

These restate the already-recorded deferred items in Planner-ready form. No
**new** actionable gaps were found in this plan's scope.

1. **Persist the active defense across save/load (H2).** Extend
   `FVehicle::save()`/`load()` to serialize which defense is currently raised so
   a save/reload cycle preserves the to-hit resolution corrected by P2-2 (T2/T3).
   Coordinate as part of a single save-format-hardening phase (roadmap "Next" #5);
   requires a versioned wire-format bump. Origin: C3 + Phase-2; tracked in
   `doc/deferred-tasks.md`.

2. **Serialize `FPlayer::m_destroyed` (F2-serialization).** Extend
   `FPlayer::save()`/`load()` to persist the destroyed-ship list (now correctly
   owned/freed in memory by P2-5) so it survives save/reload, as required to
   back the optional Replacements rule. Bundle with H2 in the save-format-
   hardening phase behind the same format version bump. Origin: Phase-1 F2;
   tracked in `doc/deferred-tasks.md`.

## Final outcome

- **PASS**

All seven subtasks match the governing plan at the feature level; the repo
builds clean; the full suite (245) and GUI suite (75) pass on the merged branch;
non-editable rules docs and the save wire format are unchanged; deferred items
are correctly recorded and not treated as in-scope gaps; behavioral coverage
(including the plan-mandated red/green discriminators) is present and adequate;
and the no-specialist-security-review judgment is sound. No blocking or warning
findings.
