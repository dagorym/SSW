# Tester Report — SMF-05

## Status
PASS

## Acceptance criteria validated

1. **Activated seekers block starts at getActionButtonRowBottom(), not BORDER** — VERIFIED
   Source-inspection assertion confirms `int y = getActionButtonRowBottom();` in drawSeekerActivation,
   and `int y = BORDER;` is absent from that function body.

2. **Data source is getActiveSeekersByMovingPlayerThisPhase()** — VERIFIED
   Source-inspection assertion confirms `getActiveSeekersByMovingPlayerThisPhase()` call present
   and `getActiveSeekersByMovingPlayer()` absent in that function body.

3. **All 198 tactical tests pass** — VERIFIED: 198/198

4. **No new GUI test regressions** — VERIFIED: 9 pre-existing failures unchanged; no new failures.

## Test changes made
Added two new assertions in `testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker`:
- `assertContains(body, "int y = getActionButtonRowBottom();")` 
- `assertNotContains(body, "int y = BORDER;")`
- `assertNotContains(body, "m_parent->getActiveSeekersByMovingPlayer();")`

## Test commit hash
21dc405

## Validation commands
- make -C tests tactical-tests && ./tests/tactical/TacticalTests → OK (198 tests)
- DISPLAY=:99 ./tests/gui/GuiTests → 45 run, 9 failures (all pre-existing)
