---
name: "impact-analysis"
description: "Survey the full in-scope design documents and identify impacted sections and downstream effects."
---

# Designer Impact Analysis

Load this skill after request intake establishes the in-scope document set.

## Tooling

- Use the colocated tool `map_design_impact.py` to enumerate headings, rank likely impacted sections, flag repeated terminology, and surface cross-references before deeper editorial reasoning.

## Required Actions

- Review the entire in-scope design document set before editing.
- List every section that must change.
- List cross-section impacts, assumptions, risks, and downstream effects.
- Check for cross-document contradictions: verify that the proposed changes do not create logical inconsistencies between documents on shared design decisions, terminology, or behavioral assumptions.
- Identify security-relevant design implications in impacted sections, including trust boundaries, authentication and authorization assumptions, secret handling, sensitive data flow, misuse cases, and unsafe defaults.
- Preserve existing structure and terminology while identifying where targeted edits are sufficient.

## Limits

- Do not spend prompt tokens manually reconstructing heading inventories or repeated cross-document terminology when the colocated tool already provides that context.
