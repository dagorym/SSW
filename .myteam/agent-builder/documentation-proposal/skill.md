---
name: "documentation-proposal"
description: "Propose repo-level documentation follow-up after `.myteam` node updates."
---

# Agent Builder Documentation Proposal

Load this skill only after `.myteam` node actions are complete and repo-level documentation or instruction files may need follow-up.

## Required Actions

- Inspect whether repo-level documentation or instruction files such as top-level docs, bootstrap-only `AGENTS.md`, or relevant `.myteam` role or skill files should change.
- Present minimal documentation diffs inline in chat when relevant.
- Ask for explicit approval before writing documentation updates.
- Only propose changes to `AGENTS.md` when its startup bootstrap instruction itself should change.
- Only propose changes to any lightweight IDE bootstrap instruction file when its bootstrap instruction itself should change.

## Limits

- Do not write repository documentation updates without explicit approval.
