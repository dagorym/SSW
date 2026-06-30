Reviewer Report

Feature plan reviewed:
- plans/seeker-missile-fixes-round6-plan.md (Seeker Missile Fixes Round 6 — two subtasks SMRVI-01, SMRVI-02; overall acceptance criteria 1-7)

Inputs reviewed:
- Governing plan: plans/seeker-missile-fixes-round6-plan.md
- Commit range reviewed: 72a2e45..8852c3a (HEAD 8852c3a) on coordination base branch sf2
- SMRVI-01 artifacts: artifacts/seeker-missile-fixes-round6/SMRVI-01/{implementer,tester,documenter,verifier}_{report.md,result.json} (verifier PASS)
- SMRVI-02 artifacts: artifacts/seeker-missile-fixes-round6/SMRVI-02/{implementer,tester,documenter,verifier}_{report.md,result.json} (verifier PASS)
- Production code: include/tactical/FBattleDisplay.h, src/tactical/FBattleDisplay.cpp
- Tests: tests/gui/TacticalGuiLiveTest.{cpp,h}, tests/tactical/FTacticalBattleDisplayFireFlowTest.{cpp,h}
- Docs: doc/test-contracts.md, AGENTS.md; confirmed doc/rules/tactical_operations_manual.md NOT edited
- Feature-level validation run by reviewer: tactical suite 226/226 PASS; GUI suite 56/56 PASS (DISPLAY=:0, run from tests/gui)

Overall feature completeness:
- COMPLETE. All seven overall acceptance criteria map to merged code plus genuine behavioral tests, and both subtasks compose cleanly without regressing each other.
- AC-1 (single shared constant): SEEKER_DEPLOY_INSTRUCTION is declared once in FBattleDisplay.h and defined once in FBattleDisplay.cpp:61; both draw() (pendingLMargin text-extent measurement) and drawAttackFire() (drawn text via .ToStdString()) reference it. A repo-wide grep confirms the literal exists in production code ONLY as that single constant definition plus one explanatory comment — no remaining duplicate code literal.
- AC-2 (SMRVI-01 behavior-preserving): the wording, draw position, and pendingLMargin formula are unchanged (only the source of the measured string changed). Behavioral coverage is the pre-existing live test testOffensiveSeekerPendingListRegionVisibilityAndRecall, which still passes; the FTacticalBattleDisplayFireFlowTest source-contract assertions (literal-once, present-in-both-sites, absent-as-inline) supplement it.
- AC-3 ((X,Y) row text): drawSeekerActivation() now formats each row as 'Deactivate seeker #N (X,Y)' from seeker.hex.getX()/getY(), dropping heading/allowance. Behaviorally discriminated by testSeekerActivationRowTextShowsPositionAndMarginIsDynamic AC-a (positive: region width == new-text width+16; negative: != old heading/allowance width+16).
- AC-4 (dynamic margin): the fixed lMargin=310 is replaced by leftOffset + max(instrW, doneButtonRightExtent) + 2*BORDER, where instrW measures the exact three instruction strings drawn (per D4) and doneButtonRightExtent is the Seeker Activation Done button's right edge. Behaviorally discriminated by AC-b (region.GetX() > 310, which the old fixed value would fail).
- AC-5 (click still deactivates): row click regions are re-derived from the new text/margin; behaviorally discriminated by AC-c (a click at the region center decrements getActiveSeekersByMovingPlayerThisPhase().size() from 1 to 0).
- AC-6 (protected doc): git diff over the full range confirms doc/rules/tactical_operations_manual.md was not edited.
- AC-7 (suites pass, no new regression): reviewer-run tactical suite 226/226 PASS; GUI suite 56/56 PASS — including the documented pre-existing BattleSimGuiLiveTest centering item, which passed in this run.
- Cross-subtask composition is clean: SMRVI-01 edits draw()/drawAttackFire()/the header constant; SMRVI-02 edits drawSeekerActivation()/the header. They touch disjoint regions of FBattleDisplay; the SEEKER_DEPLOY_INSTRUCTION constant is used by the attack-phase sites only, and the activation panel correctly measures its own (different) instruction strings.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/tactical/FBattleDisplay.cpp:1714-1734 - The three seeker-activation instruction strings are inline literals at two sites within drawSeekerActivation() (measured for instrW, then drawn). This is the same measure-vs-draw drift pattern that SMRVI-01 eliminated for the attack-phase string, but the plan explicitly scoped extracting these strings out of SMRVI-02 (D4 / Assumptions), and an anti-drift comment flags the invariant.
  Low risk because both sites are in the same function and the comment calls out the invariant. Not a defect in this feature; carried forward as an optional consistency follow-up (mirrors the SMRVI-01 fix). The verifier raised the same observation.
- tests/gui/TacticalGuiLiveTest.cpp:0 - AC-4 of the plan requires the activation column to clear BOTH the widest instruction line AND the Done-button's right edge; the implementation correctly computes max(instrW, doneButtonRightExtent), but the behavioral test asserts only region.GetX() > 310 (dominated in practice by instrW). The button-clearing leg of AC-4 is verified by code inspection and the verifier rather than by a discriminating assertion against the button geometry.
  The implementation is correct, so this is a test-rigor observation, not a behavior gap. A stronger test could assert region.GetX() >= leftOffset + doneButtonRightExtent + 2*BORDER. Low priority.
- include/tactical/FBattleDisplay.h:4 - The @author lists in FBattleDisplay.h and FBattleDisplay.cpp now contain 'claude-sonnet-4-6 (medium)' more than once across rounds. AGENTS.md prohibits removing authors but does not prohibit duplicate entries.
  Cosmetic only; no convention violation. Mentioned for completeness.

Missed functionality or edge cases:
- No missed functionality. All three user-requested items are delivered: (1) attack-phase instruction string single-sourced; (2) activation rows show (X,Y); (3) activation column dynamically shifted to clear the left column and Done button.
- Edge cases handled: the dynamic margin uses the Done-button's laid-out right edge when available (pos.x >= leftOffset) and falls back to leftOffset + best width when the button geometry is not yet valid at draw time, so the column does not collapse during early layout passes.

Follow-up feature requests for planning:
- OPTIONAL / non-blocking consistency follow-up: extract the three PH_SEEKER_ACTIVATION instruction strings ('Seeker activation phase.', 'Click a seeker stack on the board to activate one seeker.', 'Click a row below to deactivate an activated seeker.') into shared FBattleDisplay constants and reference them from both the GetTextExtent measurement and the DrawText calls in drawSeekerActivation(), eliminating the residual measure-vs-draw drift risk in the same way SMRVI-01 did for the attack-phase string. This was explicitly out of SMRVI-02's scope per plan decision D4/Assumptions; raise only if the team wants to close the remaining drift surface.
- OPTIONAL test hardening: strengthen testSeekerActivationRowTextShowsPositionAndMarginIsDynamic AC-b to assert the activation column left edge also clears the Done-button right edge (region.GetX() >= leftOffset + doneButtonRightExtent + 2*BORDER), not only region.GetX() > 310, so the button-clearing leg of plan AC-4 has a discriminating behavioral assertion rather than relying on inspection.

Artifacts written:
- artifacts/seeker-missile-fixes-round6/reviewer/reviewer_report.md
- artifacts/seeker-missile-fixes-round6/reviewer/reviewer_result.json

Final outcome:
- PASS
