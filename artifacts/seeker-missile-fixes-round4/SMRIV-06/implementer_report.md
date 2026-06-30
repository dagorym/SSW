# Implementer Report — SMRIV-06

**Task:** Cleanup/hardening follow-ups in `FTacticalGame::placeOrdnanceAtHex()`  
**Branch:** `sf2-SMRIV-06-implementer-20260629`  
**Commit:** `5421d75`  
**Status:** SUCCESS

## Changes Made

### File: `src/tactical/FTacticalGame.cpp`

Two minimal, localized edits to `placeOrdnanceAtHex()`:

**Edit 1 — Remove dead `else if (getState() == BS_PlaceSeekers)` branch**

Within the `removePlacedOrdnanceForSelection` block, the `else if (getState() == BS_PlaceSeekers) { rebuildDeployablePlacementSourcesFiltered(FWeapon::SM); }` branch was removed. This branch was unreachable: the PGS-03 early-return at the top of the function (`if (getState() == BS_PlaceSeekers && selectedSource.weaponType == FWeapon::SM) return placeSeekerFromSelection(...)`) takes every BS_PlaceSeekers SM placement before the removal block is reached. The BS_PlaceMines rebuild path and the `else` default are preserved unchanged.

**Edit 2 — Add defensive `m_minedHexList.erase(hex)` after PGS-04 for-loop**

After the PGS-04 for-loop that searches `m_placedOrdnance` for a matching `FWeapon::M` record, a defensive `m_minedHexList.erase(hex)` was added inside the outer `if` block. This ensures that when a hex is present in `m_minedHexList` but has no corresponding placed-ordnance record (a rare inconsistency), the stale hex entry is cleared rather than left behind.

## Diff Summary

- `src/tactical/FTacticalGame.cpp`: +3 insertions, −2 deletions

## Validation

Command: `make -C tests tactical-tests` then `./tests/tactical/TacticalTests`

Result: **PASS — 224 tests, 0 failures**

## Acceptance Criteria Status

1. Dead `else if (getState() == BS_PlaceSeekers)` branch removed — DONE
2. Defensive `m_minedHexList.erase(hex)` added for stale-entry case — DONE
3. Existing pre-game ordnance behavioral tests pass — PASS (224/224)

## Expected Validation Failures

None.
