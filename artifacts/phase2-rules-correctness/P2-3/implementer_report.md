# P2-3 Implementer Report — Strategic loss-counting rules deviations S2 & S3

## Scope

Fix two strategic rules deviations in the post-combat destroyed-ship counting
inside `FGame::cleanUpShips()`:

- **S2** — Retreat condition 3 ("The Sathar have lost 40 ships, including
  fighters"): fighters must be counted toward `m_lostSatharShips`. The prior code
  guarded the increment with `if (ship->getType() != "Fighter")`, wrongly
  excluding fighters.
- **S3** — Retreat condition 5 ("At the end of any tenday, the Sathar have lost
  more ships in combat than the UPF. Fighters and Militia ships are not counted"):
  the tenday counters `m_lostTendaySathar` / `m_lostTendayUPF` must NOT count
  fighters or ships belonging to militia fleets, on BOTH sides. The prior code
  incremented both counters for every destroyed ship unconditionally.

## Files changed (allowed set only)

- `src/strategic/FGame.cpp` — `FGame::cleanUpShips()` counting block.
- `include/strategic/FGame.h` — `cleanUpShips()` Doxygen header.

## Change detail

In the destroyed-ship second pass of `cleanUpShips()`, a single predicate now
gates the tenday counters for both sides:

```cpp
bool countsTowardTenday = (ship->getType() != "Fighter") && !fleet->isMilitia();
```

- UPF side: `m_lostTendayUPF++` only when `countsTowardTenday`.
- Sathar side: `m_lostTendaySathar++` only when `countsTowardTenday`;
  `m_lostSatharShips++` now runs unconditionally (S2 — fighters included).
- Conditions 1/2 (`m_lostAC` / `m_lostHC`), the removal/ownership logic, and
  `save()`/`load()` are unchanged.

`fleet` is the in-scope `FFleet*` for the ship being removed, so `fleet->isMilitia()`
correctly reflects militia membership (militia set via `FFleet::setMilitia(true, ...)`).

## Doxygen

`cleanUpShips()` header (in `include/strategic/FGame.h`) updated: `@date Last
Modified: Jul 11, 2026`, added `Claude Sonnet 5 (medium)` to `@author` (already
present from prior work), and documented the condition-3 (include fighters) and
condition-5 (exclude fighters and militia, both sides) counting rules.

## Rules cross-check

Confirmed against `showRetreatConditions()` text in `src/strategic/FGame.cpp`:
- case 3: "The Sathar have lost 40 ships, including fighters." → counts fighters.
- case 5: "Fighters and Militia ships are not counted toward this condition." →
  excludes fighters and militia.

`checkForVictory()` case 5 evaluates `m_lostTendaySathar > m_lostTendayUPF` at a
tenday boundary (`m_round % 10 == 0 && m_round > 0`) and now reads the filtered
counts. No change to `checkForVictory()` was required.

## Validation

- Build from repo root (`make`): exit code 0, clean.
- Full suite (`cd tests && make && ./SSWTests`): `OK (217 tests)` when run with
  cwd = `tests/`.
  - Note: running `./SSWTests` from any other cwd shows 10 `FGameHeaderDependencyTest`
    "candidate file path readable" failures because those source-inspection tests
    use cwd-relative paths (`../../include/...`). This is a pre-existing
    cwd-dependency unrelated to this change; from `tests/` all 217 pass.

## Notes for downstream roles

- Dependency ordering: P2-3 is the FIRST subtask in the shared `FGame.cpp` lane;
  runs before P2-4 and P2-7.
- Tester: add behavioral tests in `tests/strategic/FGameTest.{h,cpp}` constructing
  real fleets with fighters, militia (`FFleet::setMilitia(true, ...)`), and regular
  ships at HP<=0, run `cleanUpShips()`, and assert exact counter deltas:
  - Sathar fighter destroyed → `m_lostSatharShips += fighterCount`; tenday counters unchanged.
  - Militia ship destroyed (either side) → tenday counter unchanged.
  - Non-fighter non-militia ship destroyed → correct side's tenday counter += count.
