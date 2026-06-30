# Implementer Report (pass-2 remediation)

Status:
- success

Task summary:
- SMFR-05 pass-2: Add behavioral GUI render test for AC1 seeker path visibility,
  addressing the Verifier WARNING that AC1's render side was covered only by a
  source-contract check. No production code changes were required (all five
  acceptance criteria were already confirmed correct). The deliverable is
  testSeekerPathRendersInPHMoveWithMovementPath added to TacticalGuiLiveTest.

Changed files:
- tests/gui/TacticalGuiLiveTest.cpp
- tests/gui/TacticalGuiLiveTest.h

Validation commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests
- make -C tests/gui
- ./tests/gui/GuiTests (with live DISPLAY=:0)

Validation outcome:
- 214 tactical tests pass. GUI suite passes new test (48 total, 9 pre-existing
  failures unchanged). No regressions introduced.

Implementation/code commit hash:
- 6dddd6a

Implementation context:
- The key insight discovered during development: setPhase(PH_MOVE) routes through
  beginSeekerActivationPhase() -> resolveActiveSeekersForMovingPlayer() which clears
  and re-initializes all active seeker movementPaths. So the test MUST seed the seeker
  WITH a path AFTER calling setPhase(PH_MOVE), not before, or the path is wiped out.
- The test uses SeederGame/TestableBattleScreen (from SMFR-04) to inject a seeker
  directly after the phase is set, bypassing the full tactical flow.
- Pixel detection uses an offscreen wxMemoryDC differential approach: draw before
  seeder (baseline), draw after seeder+path (after-seed), compare pixel diffs in the
  path band (x=344..356, y=290..385) where drawSeekerPaths draws the line.
  The count label from drawSeekerMoveCountOverlay is at (360,360) which is OUTSIDE
  the path band, so it does not contaminate the gate check.
- The PH_ATTACK_FIRE gate check confirms zero diffs in the path band (drawSeekerPaths
  not called), while PH_MOVE and PH_SEEKER_ACTIVATION both produce path band diffs > 0.
- A platform pre-check verifies dc.DrawLine at those coordinates produces cyan pixels
  on a fresh wxMemoryDC before the board-level assertions run.

Expected validation failures carried forward:
- None
