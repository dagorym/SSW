Implementer report - Milestone 7 Subtask 2 (FBattleScreen ownership/lifecycle wiring)

Plan step status
- Preflight scope check: complete.
- Implement incrementally: complete.
- Validate after change: complete.
- Completion gate: complete.

Code changes
- include/tactical/FBattleScreen.h
  - Added forward declarations for FTacticalGame and WXTacticalUI (token-split form to preserve current adapter test constraints).
  - Added owned members `m_tacticalGame` and `m_tacticalUI`.
- src/tactical/FBattleScreen.cpp
  - Added includes for FTacticalGame and WXTacticalUI.
  - Constructor now allocates FTacticalGame and WXTacticalUI and installs the UI onto the tactical game.
  - Destructor now detaches UI from FTacticalGame then deletes owned WXTacticalUI and FTacticalGame safely.
  - Existing child widget construction and close-path guard behavior remained intact.
- src/tactical/Makefile
  - Updated dependency lines for FBattleScreen.o to include WXTacticalUI and FTacticalGame headers.

Validation commands run
1) cd src/tactical && make clean >/dev/null && make  (baseline)
2) cd tests/tactical && make && ./TacticalTests      (baseline)
3) cd src/tactical && make clean >/dev/null && make  (post-change)
4) cd tests/tactical && make && ./TacticalTests      (post-change)

Validation outcomes
- Baseline tactical build: pass.
- Baseline tactical tests: pass (OK 76 tests).
- Post-change tactical build: pass.
- Post-change tactical tests: pass (OK 76 tests).

Notes
- Scope was limited to allowed files plus required shared artifacts.
- Ownership/lifecycle wiring is now in FBattleScreen while preserving guarded modal close behavior.
