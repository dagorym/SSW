---
name: "final-reviewer"
description: "Compose and launch the final Reviewer stage after all subtasks are merged."
---

# Coordinator Final Reviewer

Load this skill only when all subtasks have completed successfully and been merged into the coordination base branch.

## Required Actions

- Verify that no planned subtask remains pending, in flight, failed, or awaiting a user decision before launching the Reviewer.
- Create a dedicated review worktree from the coordination base branch.
- Resolve reviewer artifacts to the plan-level artifact directory.
- Reread `.myteam/reviewer/role.md` immediately before writing the Reviewer prompt.
- Compose the Reviewer launch prompt from the approved plan, completed subtask outputs, and the current reviewer definition.
- Start the substantive Reviewer prompt with the exact line `Your role is 'reviewer'. Your task is as follows:`.
- Include the required coordinator wrapper lines ahead of the substantive Reviewer prompt body.
- Launch the Reviewer as an isolated sub-agent and report its outcome without starting automatic follow-up work.

## Limits

- Do not launch this skill after an individual subtask completion or merge event.
- Do not treat a partially completed plan as eligible for final review.
