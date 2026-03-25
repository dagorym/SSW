# Final Reviewer Report — FFleet Destination Sentinel Mismatch

**Plan:** `plans/ffleet-destination-sentinel-mismatch-plan.md`
**Branch:** `ffleet-destination` (all 3 subtasks merged)
**Overall Verdict:** CONDITIONAL PASS

---

## Feature Completeness

The merged work correctly implements the intended sentinel normalization across all three
planned subtasks. No confirmed production functionality gap was identified relative to the
approved plan.

---

## Per-Subtask Acceptance Criteria

### Subtask 1 — Normalize FFleet sentinel contract — COMPLETE
- `FFleet::NO_DESTINATION` and `FFleet::NO_ROUTE` declared as `static constexpr unsigned int`
  (UINT_MAX) in `include/strategic/FFleet.h`.
- `setLocation()` defaults, constructor, and `decTransitTime()` arrival path all use named sentinels.
- `getJumpRoute()` return type corrected from `int` to `unsigned int`.
- `load()` normalizes legacy `m_jumpRouteID == 0` to `NO_ROUTE` for backward compatibility.

### Subtask 2 — Update callers — COMPLETE
- `FGame.cpp`, `SystemDialogGUI.cpp`, `WXPlayerDisplay.cpp`, `ViewFleetGUI.cpp` all updated.
- No raw `-1` comparisons remain in targeted call sites.
- Null/sentinel guards added before all destination/route dereferences.

### Subtask 3 — Repair and extend FFleet tests — COMPLETE (with caveat)
- All stale `== 0` / `== -1` assertions replaced with `FFleet::NO_DESTINATION` / `FFleet::NO_ROUTE`.
- Coverage extended for construction, transit, arrival, and cancel-jump state transitions.
- 161 tests, 0 failures.
- Caveat: `testDecTransitTime` retains a pre-existing RNG dependency (not introduced by these changes).

---

## Findings

### BLOCKING
None.

### WARNING (2)

1. **Serialization sentinel round-trip not covered by tests.**
   `tests/strategic/FFleetTest.cpp:217-230` — The existing serialization test checks ship count
   only; it does not verify that `NO_DESTINATION`/`NO_ROUTE` survive `save()`/`load()`, nor that
   legacy `route == 0` loads as `NO_ROUTE`. This leaves the backward-compatibility normalization
   in `FFleet::load()` unverified by regression tests.

2. **`testDecTransitTime()` is RNG-dependent and may flake.**
   `tests/strategic/FFleetTest.cpp:147-170` — Jump-success probability is not controlled in the
   test; the arrival-reset sentinel assertions could intermittently be skipped or misfire.
   (Pre-existing; not introduced by this feature.)

### NOTE (2)

1. Implementation is coherent and convention-compliant across all subtasks. Named sentinel
   constants are applied consistently; guard logic is localized to the intended call sites.

2. `SystemDialogGUI.cpp` `"unknown"` fallback string is safe. Cosmetic refinement can be
   considered in a follow-up unrelated to this feature.

---

## Missed Functionality
None relative to the approved plan.

---

## Follow-up Recommendations for Planner

1. **Add FFleet serialization sentinel regression coverage.**
   Write tests that verify:
   - `NO_DESTINATION` and `NO_ROUTE` round-trip through `save()`/`load()`.
   - A fleet loaded with legacy `m_jumpRouteID == 0` has `getJumpRoute() == FFleet::NO_ROUTE` after load.

2. **Remove RNG dependence from `FFleetTest::testDecTransitTime()`.**
   Refactor the test so jump-success behavior is controlled (e.g., seed or stub), making
   the arrival-state sentinel assertions deterministic and stable in CI.

---

## Coherence and Convention Assessment
The delivered feature is coherent across subtasks, readable, and aligned with project conventions.
The sentinel contract is now explicit, caller guards are localized, and the test suite reflects
the new contract. The remaining concerns are test robustness, not production correctness.
