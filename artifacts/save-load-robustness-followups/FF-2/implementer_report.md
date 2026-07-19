# FF-2 Implementer Report — Deepen save-load aggregate-abort into `FVehicle::load()`

**Model:** Claude Opus 4.8 (1M context) (medium)
**Branch:** `phase5-FF-2-implementer-20260719`
**Code commit:** `da076ba2`
**Date:** Jul 19, 2026

## Scope restatement

Reviewer follow-up FF-2. `FVehicle::load()` (`src/ships/FVehicle.cpp`) previously
discarded the return of its own scalar reads (`readU32`/`readString`/`read<T>`)
and of its nested weapon/defense `w->load(is)`/`d->load(is)` calls, returning
nonzero only on the `createWeapon()`/`createDefense()==NULL` factory paths. A
stream truncated strictly inside a single vehicle's own scalar/weapon/defense
region therefore still returned 0, so FR-1's per-container aggregate-abort
guarantee did not reach that depth. This subtask closes that gap.

**Allowed files:** `src/ships/FVehicle.cpp`, `include/ships/FVehicle.h` (Doxygen
only). Both were touched; no other files changed.

## What changed

In `FVehicle::load()`:

- Every scalar read now checks its own return and aborts (`return 1`) on nonzero:
  the leading `readU32(is,id)`, both `readString` calls (`m_name`, `m_iconName`),
  every `read<T>` scalar (`m_currentMR`, `m_currentADF`, `m_currentHP`, `m_speed`,
  `m_heading`, `m_currentDCR`, `m_maskingScreenTurnCount`, `m_combatControlDamaged`,
  `m_onFire`, `m_powerSystemDamaged`, `m_navError`), the `readU32(is,owner)`, both
  `readU32(is,count)` list-count reads, each in-loop `read(is,type)` /
  `read(is,defType)` type-tag read, and the trailing `readU32(is,currentDefenseIndex)`
  active-defense-index read.
- Each nested `w->load(is)` and `d->load(is)` return is now checked. On a nonzero
  nested return, the just-allocated object (`w` / `d`) — which is **not yet**
  appended to `m_weapons`/`m_defenses` — is `delete`d, then the function returns
  nonzero. This mirrors the existing `createWeapon()/createDefense()==NULL` abort
  discipline and the FR-1 `delete v; return 1;` pattern already used in
  `FFleet::load()`/`FPlayer::load()`.

## Cleanup / memory-safety reasoning (no leak, no dangle, no double-free)

- **Weapons already appended** to `m_weapons` in earlier loop iterations remain
  owned by this vehicle; when the abort propagates and the FR-1-hardened caller
  (`FFleet::load()` / `FPlayer::load()`) `delete`s the not-yet-committed vehicle,
  `~FVehicle()` frees them. Same for defenses in `m_defenses`.
- **The failing weapon/defense** (`w`/`d`) is deleted explicitly before returning
  because it was created via the factory but not yet `push_back`-ed, so it is not
  reachable by `~FVehicle()` and would otherwise leak.
- **`m_currentDefense`** is set to `NULL` immediately after the old defense list
  is deleted (pre-existing code, retained) and is only re-pointed at a live
  `m_defenses` entry on the success path after the trailing index read succeeds.
  On every abort path in the defense region (type-tag read failure, `createDefense`
  NULL, nested `d->load` failure, or the trailing index read failure) it stays
  `NULL` rather than dangling. `~FVehicle()` does not free `m_currentDefense`
  separately (it frees the `m_defenses` vector entries), so a NULL
  `m_currentDefense` on abort is safe.
- **Preserved unchanged:** the `createWeapon()`/`createDefense()==NULL` unknown-type
  abort, and the CRIT-3/H2 active-defense-index fallback on the success path
  (in-range index → `m_defenses[index]`, else `m_defenses[0]`, else fabricate a
  safe default `FNone`).

Doxygen on `FVehicle::load()` (`include/ships/FVehicle.h`) updated: `@date Last
Modified` bumped to Jul 19, 2026, implementing model added to `@author` list
(already `Tom Stephens, Claude Sonnet 5 (medium)`; note: the co-author of this
change is Claude Opus 4.8 (1M context) — recorded in the commit trailer; the
header author list retains all prior authors per the never-remove-authors rule and
the doc block now describes the FF-2 behavior).

## Validation

- `make -C src/ships` — builds clean, no new warnings.
- `make check` from the worktree root — exit code 0 (the target stops and
  propagates a non-zero exit at the first failing suite, so exit 0 confirms all
  three passed):
  - **SSWTests:** `OK (293 tests)`
  - **TacticalTests:** `OK (253 tests)`
  - **GuiTests:** `OK (85 tests)` (under `xvfb-run -a`)

No behavioral test was authored by this run; behavioral coverage is the Tester's
responsibility (see `tester_prompt.txt`).

## Tester handoff summary

Behavioral coverage required (Tester-owned): (a) an in-vehicle scalar-region
truncation making `FGame::load()` abort nonzero and report once; and (b) driving a
nonzero `FVehicle::load()` for a vehicle placed in a **fleet**, in **`m_unattached`**,
and in **`m_destroyed`** so all four FR-1 propagation sites (FFleet ship-loop,
FPlayer fleet-loop / `m_unattached` / `m_destroyed`) are each exercised (this also
delivers reviewer follow-up FF-3). Full details in `tester_prompt.txt`.
