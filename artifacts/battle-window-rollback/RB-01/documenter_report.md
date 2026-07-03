# Documenter Report

Status:
- success

Task summary:
- Rolled back the battle-window close/Quit/modality/minimize attempts (TMF-02 b938d8f, TMF-03 233c1ad, TMFR-01 d4699c6) to merge-base (5c3f91a) behavior in FBattleScreen, WXTacticalUI, and SelectCombatGUI, while preserving TMF-05's (0bf0d9e) end-of-move Turn Left/Right button wiring. Verified the Implementer's production rollback against merge-base and removed six now-obsolete TMF-02/TMF-03/TMFR-01 tests referencing removed symbols/behavior so the full suites build and pass. Documenter pass: reconciled doc/test-contracts.md and AGENTS.md to remove stale descriptions of the reverted mechanisms and corrected a doc/UsersGuide.md claim about the minimize button.

Branch name:
- fixes-rb01-documenter-20260703

Documentation commit hash:
- 06f954d

Documentation files added or modified:
- AGENTS.md
- doc/test-contracts.md
- doc/UsersGuide.md

Commands run:
- git diff 5c3f91a -- include/gui/WXTacticalUI.h include/gui/WXTacticalUI.cpp src/gui/WXTacticalUI.cpp src/gui/SelectCombatGUI.cpp (empty)
- git diff 5c3f91a -- doc/UsersGuide.md (only TMF-05 preserved section after edit)
- git diff 5c3f91a -- AGENTS.md (only TMF-05/TMF-06/TMFR-03 preserved additions after edit)
- grep -rn for dismissActiveDialog/hasPendingDialog/m_dialogStack/m_activeDialog/wxWindowDisabler across src/, include/, doc/, AGENTS.md
- grep -rn for the six removed TacticalGuiLiveTest/WXTacticalUIAdapterTest names across tests/ (confirmed absent)
- git show 5c3f91a:doc/UsersGuide.md and git show 5c3f91a:AGENTS.md for baseline wording comparison
- git add AGENTS.md doc/UsersGuide.md doc/test-contracts.md; git commit

Final test outcomes:
- No production or test code was changed by the Documenter; only AGENTS.md, doc/test-contracts.md, and doc/UsersGuide.md were edited.
- Tester's final outcomes (unchanged, carried forward): SSWTests 198 tests OK; TacticalTests 253 tests OK; GuiTests 65 tests OK (DISPLAY=:0); top-level make after make all_clean exits 0.

Assumptions:
- No dedicated plans/*.md file exists for this rollback; scoped via artifacts/battle-window-rollback/RB-01/ per the coordinator handoff.
- Comparison base is merge-base 5c3f91a as specified by the coordinator and Tester handoff.

Artifacts written:
- artifacts/battle-window-rollback/RB-01/documenter_report.md
- artifacts/battle-window-rollback/RB-01/documenter_result.json
- artifacts/battle-window-rollback/RB-01/verifier_prompt.txt
