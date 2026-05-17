---
name: "intake"
description: "Determine create vs update, classify role vs skill, normalize the target path, and disclose the default myteam instruction path."
---

# Agent Builder Intake

Load this skill immediately after the request is recognized as `.myteam` role or skill creation or update work.

## Required Actions

- Determine whether the user wants to create a new role or skill or update an existing one.
- Determine whether the target should be a `role` or a `skill`.
- Ask for the target path.
- Run `node_target.py` to normalize the path and derive the default drafting target whenever enough input is available.
- State the default drafting file returned by `node_target.py` explicitly.
- State that a different file can be used only if the user explicitly requests it.
- If the tool cannot normalize the request cleanly, ask a targeted clarification question instead of guessing.

## Limits

- Do not ask the user to choose files up front.
