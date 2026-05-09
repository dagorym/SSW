---
name: "doc-editing"
description: "Choose the right documentation targets and apply minimal documentation-only changes."
---

# Documenter Doc Editing

Load this skill only when deciding which docs to update or create, or when actively editing documentation.

## Decision Rules

- Update existing docs when the behavior belongs in documentation that already exists.
- Create a new file under `docs/` only when the implementation introduces a concept not already covered.
- Skip documentation for trivial fixes or minor refactors unless documented behavior changed.
- Preserve existing terminology and avoid creating parallel sources for the same fact.

## Editing Rules

- Make minimal, targeted documentation edits.
- Document the implemented behavior, interfaces, constraints, and usage that are now true.
- Remove or adjust outdated statements that conflict with the implemented behavior.
- Do not edit implementation or test files.

## Validation Rules

- Re-read changed docs for accuracy against the actual diff.
- Check for duplicated facts across files.
- Confirm edits remain within story scope and are documentation-only.
