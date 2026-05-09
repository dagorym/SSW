---
name: "finalization"
description: "Scaffold new `.myteam` nodes with myteam, validate node structure, and enforce instruction governance."
---

# Agent Builder Finalization

Load this skill only after the node type, target path, and approved instruction content are ready to be scaffolded or validated in `.myteam`.

## Required Actions

- For new nodes, scaffold the target directory with:
  - `myteam new role <targetpath>` for roles, or
  - `myteam new skill <targetpath>` for skills
- Verify the target `.myteam/<targetpath>/` directory exists after scaffolding or discovery.
- Verify the correct instruction file exists for the node type:
  - `role.md` for roles
  - `skill.md` for skills
- Verify `load.py` exists and remains consistent with normal `myteam` discovery behavior unless the user explicitly requested custom loader behavior.
- Verify nested parent nodes exist when the target path relies on slash-delimited discovery.

## Limits

- Keep any lightweight IDE bootstrap instruction file bootstrap-only; do not treat it as a mirrored policy target.
