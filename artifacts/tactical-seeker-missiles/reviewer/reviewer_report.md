# Reviewer Report

Feature: Tactical Seeker Missiles
Reviewer date: 2026-05-28
Worktree / branch: `seekers-tsm-reviewer-20260528` (checked out from coordination base `seekers`, fully merged TSM-001..TSM-010)

## Feature plan reviewed

- `plans/tactical-seeker-missiles-plan.md` (10 subtasks TSM-001 .. TSM-010, mostly serial).

## Inputs reviewed

- Governing plan goals, per-subtask acceptance criteria, dependency ordering, and overall acceptance criteria (1-11).
- Per-subtask delivered-work artifacts under `artifacts/tactical-seeker-missiles/TSM-001 .. TSM-010/`
  (implementer/tester/documenter/verifier reports and result JSON).
- Merged feature source, reviewed directly rather than by verifier label:
  - `include/tactical/FTacticalGame.h`, `src/tactical/FTacticalGame.cpp`
  - `include/tactical/FTacticalCombatReport.h`
  - `src/weapons/FSeekerMissileLauncher.cpp`
- Documentation: `doc/UsersGuide.md`, `doc/DesignNotes.md`.
- Protected artifact non-edit check: `doc/rules/tactical_operations_manual.md`.
- Built and ran the tactical suite in this worktree:
  `cd tests && make tactical-tests && ./tactical/TacticalTests` -> `OK (187 tests)`, 0 regressions.

## Subtask verifier verdicts (for reference only; not relied on alone)

- TSM-001 PASS, TSM-002 CONDITIONAL PASS, TSM-003 PASS, TSM-004 PASS, TSM-005 PASS,
  TSM-006 PASS, TSM-007 PASS, TSM-008 PASS, TSM-009 PASS, TSM-010 PASS.

## Overall feature completeness

The feature is complete and internally consistent at the full-feature level. All 11 overall
acceptance criteria from the plan are satisfied by the merged code, and the cross-subtask
integration concerns called out for this review were verified directly against source:

- Single inactive-seeker collection. All three seeker-creation paths feed one model
  collection `m_seekerMissiles`:
  setup placement (`placeSeekerFromSelection`, FTacticalGame.cpp:1873),
  offensive-fire pending deployment (`placeOffensiveFirePendingSeekerAtHex`, :1646),
  and committed offensive-fire seekers (commit is `clearPendingOffensiveFireSeekers()` which
  only drops pending-tracking and leaves the seekers in `m_seekerMissiles`). Activation,
  movement, and damage all operate on that single collection.

- Seeker-vs-mine damage ordering. `completeMovePhase()` (:2889) runs
  `checkForActiveSeekersOnPath` per moving ship, then `applyMovementSeekerDamage()` (:2935)
  strictly before `applyMineDamage()` (:2936), then transitions to `PH_DEFENSE_FIRE`.
  Matches plan assumption #2 and TSM-010 AC3.

- Seeker report labels not mislabeled as mine damage. Seeker detonations use
  `TRT_SeekerDamage` -> label `"Seeker Damage"` (FTacticalCombatReport.h:30-31, 195-196),
  distinct from `TRT_MineDamage` -> `"Mine Damage"`. Mine damage path still uses
  `TRT_MineDamage` (FTacticalGame.cpp:2854).

- ICM allocation paths. Both seeker detonation paths route through the existing
  `ITacticalUI::runICMSelection(...)` seam (`resolvePendingSeekerDetonationDamage`, :1148-1151),
  mirroring the mine damage ICM flow. The seeker `-8/ICM` modifier is set on the launcher
  (`m_ICMMod=-8`, FSeekerMissileLauncher.cpp:18).

- Exactly-once seeker removal. Two distinct, correct removal mechanisms:
  - Activation-step resolution (`resolveActiveSeekersForMovingPlayer`, :1045-1100) rebuilds
    `m_seekerMissiles` via a `nextStates` swap; contacted and 12-hex-expired seekers are simply
    not copied forward (removed once).
  - Ship-movement contact (`applyMovementSeekerDamage`, :2794-2824) collects detonated IDs,
    resolves damage, then erases each ID once with a `break` after the first match.
  - Offensive-fire undo (`removeOffensiveFirePendingSeekerAtHex`, :1685-1691) also erases by ID
    with a `break`.

- Detonation target selection. Highest `getMaxHP()` (max/original HP, not current) with RNG
  tie-break, implemented identically in the same-hex contact path
  (`selectSeekerContactTargetAtHex`, :1208-1228) and the multi-ship helper
  (`collectHighestMaxHPSeekerTargets`, :1384-1402). Matches Resolved Design Decision #11 and
  TSM-009 AC.

- Seeker combat stats. `FSeekerMissileLauncher` sets 5 dice (5d10), `m_ICMMod=-8`,
  `m_damageTableMod=-20`, `m_baseToHitProb=75`. Stasis-screen 90% is handled by the shared
  weapon/defense mechanics rather than re-implemented. Matches TSM-009 AC.

- Movement allowance progression. `computeSeekerMovementAllowance` = `movementTurn*2` capped at
  12, yielding 2,4,6,8,10,12 (:1252-1258); 12-hex expiry removes harmless seekers
  (:1093-1097). Targeting ignores planets/stations and includes friendly + enemy ships
  (`isValidSeekerTarget`, closest-distance + RNG tie helpers). Matches TSM-007/008 AC.

- Inactive-seeker invisibility / no passive trigger. `checkForActiveSeekersOnPath` guards
  `!active` and skips owner-owned seekers (:2776-2781); inactive seekers are excluded from
  `checkForMines`/mine damage. Matches plan goals #3, #4.

- Protected artifact untouched. `doc/rules/tactical_operations_manual.md` is unchanged versus
  `master` (`git diff master...HEAD` for that path is empty). Overall AC #11 satisfied.

- Documentation obligations met. `doc/UsersGuide.md` describes setup mine/seeker placement and
  per-source undo (lines ~370-371), the seeker activation step and visibility rules (~373),
  detonation/max-HP/ICM/"Seeker Damage" labeling (~374), and offensive-fire deployment + recall
  (~407, ~431). `doc/DesignNotes.md` documents the `PH_SEEKER_ACTIVATION` pre-phase, the
  active-seeker resolution seam, and offensive-fire deployment (lines ~269-302, ~450). Header
  Doxygen is present on the new model/phase/placement/activation APIs.

## Findings

### BLOCKING
- None.

### WARNING
- None that block the feature. (See NOTE on test style carried over from the TSM-002 verifier.)

### NOTE
- Test style on TSM-002 highest-risk paths. The TSM-002 verifier issued CONDITIONAL PASS
  because same-ship multi-slot placement/undo and post-placement source reselection are
  covered by source-token/source-contract inspection rather than runtime behavioral
  regression. The implementation reads correct, the full tactical suite passes (187 tests),
  and later subtasks exercise the unified collection at runtime, so this is a coverage-strength
  observation, not a functional gap.
- Test header include guard convention. `tests/tactical/FTacticalSeekerMovementTest.h` uses the
  leading-underscore guard form (`_FTACTICALSEEKERMOVEMENTTEST_H_`) instead of the documented
  test-header `CLASSNAME_H_` convention. Cosmetic; flagged in the TSM-010 verifier as
  pre-existing.
- Two distinct seeker-removal mechanisms exist (nextStates-swap during activation resolution
  vs erase-by-ID during ship-movement contact). Both are individually correct and exactly-once;
  noted only for future maintainers since the divergence is non-obvious.

## Missed functionality / edge cases

No missed functionality was identified. The edge cases specifically required by the plan were
confirmed present in the merged code: multiple seekers per hex, exact-source ammo restore on
undo, inactive-seeker hiding and non-triggering, irreversible activation, max-HP target ties via
RNG, 12-hex harmless expiry, batched immediate seeker damage report distinct from mine damage,
seeker-before-mine ordering, exactly-once removal, and clean phase progression after
seeker-triggered destruction.

## Follow-up feature requests for planning

- None required. There are no actionable functional gaps. If the team wants to harden the suite,
  an optional (non-blocking) backlog item would be: "Add runtime behavioral regression tests for
  same-ship multi-slot seeker/mine placement-and-undo ammo restoration and post-placement source
  reselection, replacing the current source-token coverage for those TSM-002 paths." This is a
  quality enhancement, not a correctness gap.

## Final outcome

PASS
