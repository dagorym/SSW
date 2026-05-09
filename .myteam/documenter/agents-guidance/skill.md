---
name: "agents-guidance"
description: "Conditionally review bootstrap guidance and update repository-wide myteam instructions when shared guidance actually changed."
---

# Documenter AGENTS Guidance

Load this skill only when the implemented change may affect startup bootstrap guidance or repository-wide agent or contributor instructions.

## Required Actions

- Read `AGENTS.md` before editing it only when the startup bootstrap instructions may need to change.
- Update `AGENTS.md` only when the startup bootstrap guidance itself changed.
- Update the relevant `.myteam` role or skill files when the implemented change affects repository-wide runtime guidance.
- Re-read any edited guidance files and confirm the updated guidance is accurate.

## Limits

- Treat guidance-file review as conditional scope, not a universal startup blocker.
- Do not update `AGENTS.md` for ordinary documentation-only churn or for runtime-policy changes that belong in `.myteam`.
