Implementer report - ms7-subtask1

Plan step status
- Preflight scope check: complete (allowed files respected; artifacts written to shared directory).
- Implement incrementally: complete (added FTacticalGame API surface and UI installation seam).
- Validate after change: complete.
- Completion gate: complete.

Code changes
- include/tactical/FTacticalGame.h
  - Added ITacticalUI installation/accessor seam.
  - Added tactical state accessors/mutators for control, done, close guard, planet/station state.
  - Added fleet/ship list pointer accessors.
- src/tactical/FTacticalGame.cpp
  - Added installUI(ITacticalUI*) implementation.

Validation commands run
1) cd tests/tactical && make && ./TacticalTests  (baseline)
2) cd tests/tactical && make && ./TacticalTests  (post-change)

Validation outcomes
- Both runs passed: OK (76 tests).

Notes
- No wx headers or wx types were introduced in FTacticalGame files.
- Work performed in required worktree and branch.
