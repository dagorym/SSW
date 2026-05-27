# Verifier Report — TSM-007

## Scope Reviewed

- **Implementer commit:** bcd76a7 — `Implement seeker model targeting and movement helpers`
- **Tester commit:** 0329a87 — `Add tactical seeker movement regression tests`
- **Documenter commit:** cc5b19e — `docs: capture TSM-007 seeker helper contracts`
- **Files reviewed:**
  - `include/tactical/FTacticalGame.h` (implementation + documentation)
  - `src/tactical/FTacticalGame.cpp` (implementation)
  - `tests/tactical/FTacticalSeekerMovementTest.h` (new test fixture header)
  - `tests/tactical/FTacticalSeekerMovementTest.cpp` (new test fixture source)
  - `tests/tactical/TacticalTests.cpp` (suite registration)
- **Test run re-executed:** `cd tests && make tactical-tests && ./tactical/TacticalTests` — **OK (173 tests)**
- **Protected file unchanged:** `doc/rules/tactical_operations_manual.md` diff is empty (0 lines).

## Acceptance Criteria / Plan Reference

- Source: `plans/tactical-seeker-missiles-plan.md`, subtask TSM-007 acceptance criteria
- Evaluation items reproduced for traceability:
  1. Closest target selection considers non-station ships from both sides.
  2. Planets, stations, and non-ship objects are ignored by seeker targeting.
  3. Equal-distance target ties use automatic RNG through a narrow helper.
  4. Initial facing can adjust up to three hexsides toward the selected target.
  5. Per-hex movement turns at most one hexside and greedily reduces distance where possible.
  6. Movement allowances progress as 2, 4, 6, 8, 10, and 12.
  7. Helpers can be tested without wx runtime.
  8. Any new `FHexMap` helper remains generic and non-wx.

## Convention Files Considered

- `AGENTS.md` — Doxygen comment requirements, author/date metadata, function comment rules, naming conventions, module boundary rules
- `plans/tactical-seeker-missiles-plan.md` — Design decisions 9, 11; resolved RNG tie-breaking; accepted movement algorithm

---

## Acceptance Criteria Assessment

**AC 1 — Cross-side targeting:** `resolveActiveSeekersForMovingPlayer()` concatenates both `m_attackShips` and `m_defendShips` into `candidateTargets` before passing to `selectClosestSeekerTarget(...)`. ✅

**AC 2 — Stations/planets ignored:** `isValidSeekerTarget()` checks `ship != NULL`, `ship->getHP() > 0`, and `!isStationType(const_cast<FVehicle*>(ship))`. Planets are not stored in vehicle lists; the `isStationType()` guard covers the station case. ✅

**AC 3 — RNG tie-breaking:** `chooseRandomSeekerIndex()` calls `irand(candidateCount) - 1`. `irand(N)` is defined in `include/Frontier.h` as returning `[1, N]`, making the subtract-1 form a correct zero-based index in `[0, N-1]`. ✅

**AC 4 — At most three hexsides of initial facing adjustment:** `adjustSeekerHeadingTowardTarget()` computes `delta` via `shortestHeadingDelta(...)`. Given hexagonal modular arithmetic, `shortestHeadingDelta` returns values in `{-2, -1, 0, 1, 2, 3}`, so the maximum adjustment is inherently bounded at 3. The explicit clamp guards (`if (delta > 3)` / `if (delta < -3)`) are therefore always no-ops but do not cause incorrect behavior. ✅ (see NOTE-1 below)

**AC 5 — Greedy one-hexside turn per step:** `moveSeekerTowardTarget()` iterates `turn ∈ {-1, 0, 1}`, evaluates the three candidate hexes, and picks the one with minimum hex distance. On distance ties, the smaller turn magnitude wins. This is a correct greedy approach with at most one hexside of turn per step. ✅

**AC 6 — Movement allowance progression:** `computeSeekerMovementAllowance(movementTurn)` returns `movementTurn * 2` capped at 12, producing the sequence 2, 4, 6, 8, 10, 12. `resolveActiveSeekersForMovingPlayer()` increments `movementTurn` before calling this helper. ✅

**AC 7 — Wx-free testing:** All four test cases in `FTacticalSeekerMovementTest` use source-inspection (file reading + string matching) and require no wx runtime objects. The tactical test binary builds and runs cleanly headless. ✅

**AC 8 — No new FHexMap helpers:** `FHexMap.h` and `FHexMap.cpp` diffs are empty — the implementation reuses existing `computeHexDistance`, `computeHeading`, and `findNextHex` static methods. No new FHexMap surface was introduced. ✅

---

## Findings

### WARNING

**W1 — Stale placeholder comment in `resolveActiveSeekersForMovingPlayer()`**
- Location: `src/tactical/FTacticalGame.cpp:1020–1021`
- Code: `// TSM-004 placeholder seam:` / `// active seeker movement/detonation resolution is intentionally deferred.`
- The function now contains substantial real implementation: target candidate collection, active-seeker iteration, allowance computation, target selection, heading adjustment, and movement. The retained comment says resolution is "intentionally deferred," which is actively misleading for future maintainers reviewing this function. This is a documentation accuracy defect in the source code comments.
- Severity: **WARNING** — does not affect behavior but creates incorrect maintenance expectations.

**W2 — Test coverage is source-inspection only; no behavioral exercise**
- Location: `tests/tactical/FTacticalSeekerMovementTest.cpp` (all four test methods)
- All tests verify correctness by asserting that specific code tokens appear in extracted function bodies (file read → regex-like string search). They do not instantiate any game objects, place seekers, place ships, call helpers with concrete inputs, or observe concrete outputs. While source-contract tests have precedent in this codebase, the absence of any behavioral test means:
  - An off-by-one in `computeSeekerMovementAllowance` that produced 3/5/7/9/11/12 instead of 2/4/6/8/10/12 would pass all tests as long as the source token `movementTurn * 2` appears.
  - An incorrect hex-distance comparison inequality direction (e.g., `>` vs `<`) would also pass as long as the token appears.
  - Edge behaviors (seeker at boundary, no valid targets, seeker already at target hex, movement allowance overflow past 12) are not covered.
- Severity: **WARNING** — the accepted coverage approach works given the source-contract pattern elsewhere in the codebase, but the risk of undetected logic regressions in later subtasks (TSM-008+) is elevated. Recommend adding at least a few behavioral unit tests in TSM-008 when seekers are exercised with real game state.

### NOTE

**N1 — Dead negative-clamp guard in `adjustSeekerHeadingTowardTarget()`**
- Location: `src/tactical/FTacticalGame.cpp:1138–1139`
- Code: `} else if (delta < -3) { delta = -3; }`
- `shortestHeadingDelta` returns values in the set `{-2, -1, 0, 1, 2, 3}` for any valid headings 0–5. The value −3 is never returned (because when `ccw < cw`, `ccw ≤ 2` by the property `cw + ccw = 6`). The positive `> 3` guard is similarly never triggered for the same reason. Both guards are harmless dead code. Not worth a fix in isolation, but may be removed if the function is revisited.
- Severity: **NOTE**

---

## Test Sufficiency Assessment

The four new test cases in `FTacticalSeekerMovementTest` provide source-contract coverage for all named acceptance criteria:

| AC | Test case |
|----|-----------|
| Cross-side targeting, station filter, RNG tie-breaking | `testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides` |
| Heading adjustment and greedy one-hexside turn | `testSeekerHeadingAndMovementGreedyTurnLimits` |
| Movement allowance progression and cap | `testSeekerMovementAllowanceProgression` |
| Non-wx helpers and generic FHexMap boundary | `testSeekerHelpersRemainModelOnlyAndFHexMapGeneric` |

Coverage is structurally complete relative to each acceptance criterion. However, no behavioral inputs/outputs are exercised (see W2). The tests will detect code deletions or signature renames but will not catch logic errors that preserve the observable token structure. This is a **marginal but acceptable** coverage posture for a helper-only subtask with no damage or UI wiring, on the condition that TSM-008 introduces behavioral tests when movement is exercised end-to-end.

---

## Documentation Accuracy Assessment

Doxygen comment blocks added to `include/tactical/FTacticalGame.h` for the following declarations are accurate and complete:

- `resolveActiveSeekersForMovingPlayer()` — correctly describes cross-side targeting, random tie-breaking, three-hexside adjustment, and greedy movement.
- `computeSeekerMovementAllowance(int)` — correctly documents the `2, 4, 6, 8, 10, 12` sequence and clamping behavior.
- `chooseRandomSeekerIndex(unsigned int)` — correctly describes zero-based RNG index and wx-free contract.
- `findShipHex(unsigned int, FPoint &)` — correctly documents the occupancy scan and output parameter.
- `isValidSeekerTarget(const FVehicle *)` — correctly describes live non-station filter.
- `selectClosestSeekerTarget(...)` — correctly describes combined-candidate closest-ship logic.
- `adjustSeekerHeadingTowardTarget(...)` — correctly describes the shortest-delta pre-move rotation.
- `moveSeekerTowardTarget(...)` — correctly describes greedy one-step turn loop.

All author and Last Modified metadata follows AGENTS.md conventions. The file header `Last Modified` date was updated to May 25, 2026.

`doc/rules/tactical_operations_manual.md` was not modified (confirmed by empty diff).

The stale `// TSM-004 placeholder seam:` comment in `src/tactical/FTacticalGame.cpp` (W1) is the only documentation accuracy defect found.

---

## Verdict

**CONDITIONAL PASS**

All eight TSM-007 acceptance criteria are satisfied. The tactical test suite passes cleanly at 173/173 tests including the four new `FTacticalSeekerMovementTest` cases. The protected rules document was not modified.

The conditional qualification reflects two WARNING-level findings: a stale and actively misleading "intentionally deferred" comment in the now-implemented `resolveActiveSeekersForMovingPlayer()` function body, and source-inspection-only test coverage with no behavioral exercise of the targeting and movement logic. Neither finding blocks correctness of the delivered helpers, but both elevate regression risk in later subtasks. Recommend addressing W1 before TSM-008 is submitted and adding behavioral tests alongside TSM-008's seeker resolution wiring.
