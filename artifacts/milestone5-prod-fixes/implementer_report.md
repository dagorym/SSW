Subtask: m5-prod-fixes
Worktree: /home/tstephen/worktrees/gui_sep_ms5-implementer-20260329
Branch: gui_sep_ms5-implementer-20260329

Plan step status:
- Preflight scope check: completed (approved plan reviewed, allowed production-file scope preserved)
- Artifact audit/remediation: completed
- Validation rerun in assigned worktree: completed
- Artifact handoff regeneration: completed

Summary:
- This remediation run regenerated implementer handoff artifacts only.
- Production code was not modified in this run.
- The implementation/code commit remains:
  475cd7e057b600cc7e5f396e05b5004c9b380d59

Production files associated with implementation commit:
- include/tactical/FTacticalGame.h
- src/tactical/FTacticalGame.cpp

Acceptance criteria covered by implementation context (from milestone5-tweak-plan):
- clearDestroyedShips() removes dead ship IDs from side list, model hex occupancy (m_hexData), and turn bookkeeping (m_turnInfo), while preserving winner bookkeeping.
- fireICM() clears old m_ICMData, resolves target hex occupants from m_hexData, enqueues only actionable opportunities, and never stores vehicles == NULL.
- Compatibility with existing ICMData contract and additive non-wx FTacticalGame design is preserved.

Validation commands run (this remediation run):
1) cd /home/tstephen/worktrees/gui_sep_ms5-implementer-20260329/tests/tactical && make -s && ./TacticalTests
   - Result: PASS (OK (63 tests))

Artifact files written:
- artifacts/milestone5-prod-fixes/implementer_report.md
- artifacts/milestone5-prod-fixes/implementer_result.json
- artifacts/milestone5-prod-fixes/tester_prompt.txt
