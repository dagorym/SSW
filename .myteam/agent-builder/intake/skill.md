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
- Normalize the path to slash-delimited lower-kebab-case segments as `<targetpath>`.
- State the default drafting file explicitly as:
  - `.myteam/<targetpath>/role.md` for roles
  - `.myteam/<targetpath>/skill.md` for skills
- State that a different file can be used only if the user explicitly requests it.

## Limits

- Do not ask the user to choose files up front.
