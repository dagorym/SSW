# Verifier Report — Subtask 1: Normalize FFleet Sentinel Contract

## Verdict: CONDITIONAL PASS

## Branch Reviewed
- `ffleet-destination-s1-tester-20260325` (tip: `70ef1f5bbe73cdb163066067d88d0680fbe39c99`)

## Acceptance Criteria Assessment

| # | Criterion | Result |
|---|-----------|--------|
| 1 | `NO_DESTINATION` / `NO_ROUTE` declared as `static constexpr unsigned int` = UINT_MAX | PASS |
| 2 | `setLocation()` defaults use named constants | PASS |
| 3 | Constructor initialises both fields to named sentinels | PASS |
| 4 | `decTransitTime()` arrival resets both fields to named sentinels | PASS |
| 5 | No signed/unsigned sentinel warnings for changed fields | PASS |
| 6 | No regressions outside 2 known FFleetTest failures (subtask 3) | PASS with caveat |

## Findings

### BLOCKING (1)
**`src/strategic/FFleet.cpp` — `load()` does not normalize legacy `m_jumpRouteID == 0` to `NO_ROUTE`.**
New runtime code emits `NO_ROUTE` (UINT_MAX) for "no route" state, but `load()` reads `m_jumpRouteID`
verbatim. Older persisted fleets that encoded "no route" as `0` will remain `0` after load, leaving
two encodings for the same logical state. This conflicts with the serialization-compatibility goal.
Fix: add normalization in `load()`: `if (m_jumpRouteID == 0) m_jumpRouteID = NO_ROUTE;`

### WARNING (1)
**`include/strategic/FFleet.h` — `getJumpRoute()` returns `int` while `m_jumpRouteID` is `unsigned int`.**
Returning `UINT_MAX` (NO_ROUTE) through a signed API is implementation-defined and weakens the
normalized unsigned contract. Fix: change return type to `unsigned int`.

### NOTE (1)
**`tests/strategic/FFleetTest.cpp` — serialization test does not assert sentinel round-trip.**
Existing save/load test only checks ship count; it does not verify `m_destination` or `m_jumpRouteID`
sentinel values survive round-trip. This gap means the legacy-load normalization fix is untested.
(Test improvements are scoped to subtask 3.)

## Serialization Compatibility Assessment
- **Destination sentinel**: compatible — UINT_MAX bit pattern matches former raw `-1` cast.
- **Route sentinel**: not fully compatible across legacy data — old "no route" was `0`; new is UINT_MAX.
  `load()` normalization is required for backward compatibility.

## Test Sufficiency
**Insufficient** — stale pre-sentinel assertions remain in FFleetTest.cpp (subtask 3 scope);
serialization sentinel round-trip is not covered.

## Security Assessment
No security-sensitive changes. No new bypass or validation risks identified.

## Remediation Required
Fix `load()` normalization and `getJumpRoute()` return type in `include/strategic/FFleet.h` and
`src/strategic/FFleet.cpp` before merging.
