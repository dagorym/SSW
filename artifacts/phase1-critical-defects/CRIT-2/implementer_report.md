# CRIT-2 Implementer Report

## Defect

Critical defect C2: In `FGame::checkForVictory()` the Sathar retreat
conditions 4 and 5 were evaluated **swapped** relative to the displayed
condition text produced by `FGame::showRetreatConditions()`.

- The displayed text (`src/strategic/FGame.cpp`, `showRetreatConditions()`):
  - **case 4** = "A tenday passes without the Sathar destroying at least two
    stations, two fortresses or a station and a fortress." — a
    station-destruction condition tracked by `m_stationsDestroyed`.
  - **case 5** = "At the end of any tenday, the Sathar have lost more ships in
    combat than the UPF." — a ship-loss condition tracked by
    `m_lostTendaySathar` / `m_lostTendayUPF`.

- `checkForVictory()` evaluated the ship-loss test under `case 4` and the
  station test under `case 5` — the inverse of the displayed text. A player who
  selected retreat condition 4 was actually judged against the ship-loss rule,
  and vice versa.

## Fix

Swapped the two `case` bodies in the `switch (m_satharRetreat)` block of
`FGame::checkForVictory()` so each case number evaluates the condition that
matches its displayed text:

- **case 4** now evaluates the STATIONS condition: at a tenday boundary
  (`m_round % 10 == 0 && m_round > 0`), UPF victory is set when
  `m_stationsDestroyed < 2`, and carries the `m_stationsDestroyed = 0` reset.
- **case 5** now evaluates the SHIP-LOSS condition: at a tenday boundary, UPF
  victory is set when `m_lostTendaySathar > m_lostTendayUPF`, and carries the
  `m_lostTendaySathar = 0; m_lostTendayUPF = 0;` resets.

The tenday gate (`m_round % 10 == 0 && m_round > 0`) is preserved on both
cases. Cases 1, 2, 3 (assault carriers / heavy cruisers / total ship losses),
the station-count Sathar-victory scan, and the fighter/militia counting
(S2/S3, out of scope) are unchanged.

The Doxygen header on `checkForVictory()` in `include/strategic/FGame.h` was
updated: the description now documents that each case number must match the
displayed condition text (case 4 = station/fortress destruction via
`m_stationsDestroyed`; case 5 = ship-loss via `m_lostTendaySathar` /
`m_lostTendayUPF`) and records the C2 swap fix; `@date Last Modified` set to
Jul 11, 2026; `@author` now reads `Tom Stephens, Claude Sonnet 5 (medium)`
(original author preserved, new author appended per repository convention).

## Files Changed

- `src/strategic/FGame.cpp` — swapped case 4 / case 5 bodies in
  `checkForVictory()`.
- `include/strategic/FGame.h` — updated Doxygen header on `checkForVictory()`.

## Validation

- `make` (repo root): builds cleanly, exit code 0, no new warnings in
  `FGame.cpp` / `FGame.h`.
- `cd tests && make && ./SSWTests` (run with cwd = `tests/`): full build
  succeeds; the top-level suite reports **OK (210 tests)**.

## Notes for downstream roles

- `checkForVictory()` is a private method; it is reached via the private
  `endTurn()` / turn-processing path (`checkForVictory()` is called at
  `src/strategic/FGame.cpp:521`). Behavioral tests should either exercise a
  public turn-processing entry point that reaches it, or the Tester may add a
  narrow accessor/test seam if the existing `FGameTest` fixture cannot reach it
  directly. Set `m_satharRetreat`, `m_round`, `m_stationsDestroyed`,
  `m_lostTendaySathar`, `m_lostTendayUPF` to controlled values, drive the
  victory check at a tenday boundary for each selected condition, and assert
  the exact outcome plus counter resets.
