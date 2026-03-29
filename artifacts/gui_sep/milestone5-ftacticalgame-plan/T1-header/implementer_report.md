Implementation Report - Milestone 5 Subtask 1 (todo: milestone5-header)

Plan step status
- Preflight scope check: complete
- Implement additive FTacticalGame header: complete
- Validate changes: complete
- Commit implementation/code changes: complete
- Produce handoff artifacts: complete

Files changed
- include/tactical/FTacticalGame.h

Validation commands run
1) Baseline/build check before edits:
- cd src/tactical && make objs
- Outcome: pass

2) Post-change required validation:
- cd src/tactical && make objs
- Outcome: pass

3) Header include compile check (non-wx include surface):
- printf '#include "tactical/FTacticalGame.h"\\nint main(){return 0;}\\n' | g++ -x c++ - -Iinclude -c -o /dev/null
- Outcome: pass

Acceptance criteria coverage
- FTacticalGame.h declares a compilable, non-wx tactical model class: satisfied.
- Header covers battle state, movement state, selection state, report state, ICM state, and hex-map mechanics state: satisfied.
- Header can be included without any wx header dependency: satisfied (include-only compile check passed).
- New tactical model ownership types are available in FTacticalGame.h: satisfied (FTacticalHexData and FTacticalTurnData declared in header).

Implementation notes
- Added pure C++ FTacticalGame class declaration with tactical mechanics ownership members mirrored from legacy tactical wx classes (FBattleScreen, FBattleBoard, FBattleDisplay).
- Added ITacticalUI forward declaration and placeholder pointer member m_ui.
- Added tactical model-owned state structs FTacticalHexData and FTacticalTurnData so later milestones can include FTacticalGame.h without depending on FBattleBoard.h.
- No wx includes or wx inheritance introduced in FTacticalGame.h.
