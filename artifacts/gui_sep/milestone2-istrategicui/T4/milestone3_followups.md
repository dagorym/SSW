# Milestone 3 Follow-ups - T4

- No Milestone 3 compile/include-cycle follow-up items were identified during this verification run.
- Non-blocking legacy warning observed during `src/strategic` build:
  - `src/strategic/FJumpRoute.cpp`: `-Wint-to-pointer-cast` in `FJumpRoute::load(std::istream&)`.
  - This warning is outside the Milestone 2 IStrategicUI header-boundary scope.
