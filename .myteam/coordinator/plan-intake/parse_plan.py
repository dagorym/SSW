#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


PROMPT_PREFIX = "Your role is 'implementer'. Your task is as follows:"
STABLE_ID_PATTERN = re.compile(r"\b[A-Za-z]+-\d+\b")
SECURITY_MARKER = re.compile(r"(?im)^\s*security review:\s*required\b")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def extract_section(text: str, title: str) -> str:
    pattern = re.compile(
        rf"^## {re.escape(title)}\s*$\n(.*?)(?=^## |\Z)",
        re.MULTILINE | re.DOTALL,
    )
    match = pattern.search(text)
    return match.group(1).strip() if match else ""


def extract_output_artifact_path(text: str) -> str | None:
    section = extract_section(text, "Output Artifact Path")
    for line in section.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        md_link = re.search(r"\((/[^)]+|[^)]+)\)", stripped)
        if md_link:
            return md_link.group(1)
        return stripped
    return None


def extract_subtask_ids(text: str) -> list[str]:
    return list(dict.fromkeys(STABLE_ID_PATTERN.findall(text)))


def extract_dependency_lines(text: str) -> list[str]:
    section = extract_section(text, "Dependency Ordering")
    return [line.strip() for line in section.splitlines() if line.strip()]


def extract_parallelizable_ids(text: str, subtask_ids: list[str]) -> list[str]:
    section = extract_section(text, "Dependency Ordering")
    parallelizable: list[str] = []
    for subtask_id in subtask_ids:
        pattern = re.compile(rf"(?im)^.*\b{re.escape(subtask_id)}\b.*parallel")
        if pattern.search(section):
            parallelizable.append(subtask_id)
    return parallelizable


def split_prompt_blocks(text: str) -> list[str]:
    indices = [match.start() for match in re.finditer(re.escape(PROMPT_PREFIX), text)]
    blocks: list[str] = []
    for index, start in enumerate(indices):
        end = indices[index + 1] if index + 1 < len(indices) else len(text)
        block = text[start:end].strip()
        # Trim anything from the next ATX heading onward. Each prompt block is
        # terminated by the following subtask's "### <ID> Prompt" header (level 3)
        # or by a "## <Section>" header (level 2). The previous pattern only
        # matched level-2 headers, so the trailing "### <next-ID> Prompt" line was
        # left in the block and leaked the next subtask's id into this block's id
        # match (mis-associating prompts and dropping the final subtask). Match
        # level-2-or-deeper headers so the trailing prompt header is removed.
        block = re.split(r"\n#{2,6} [^\n]+", block, maxsplit=1)[0].strip()
        blocks.append(block)
    return blocks


ARTIFACT_OWNER_PATTERN = re.compile(r"artifacts/[^\s`]+?/([A-Za-z]+-\d+)(?:/|`|\s|$)")


def owner_id_for_block(block: str, subtask_ids: list[str]) -> str | None:
    """Return the subtask id that owns a prompt block.

    A prompt's owning subtask is the one whose shared artifact directory the
    prompt writes to (``artifacts/<plan>/<ID>/``). Using that path as the
    association signal — instead of "any subtask id mentioned in the block" —
    prevents a prompt body that legitimately cross-references another subtask
    (e.g. a forward dependency note "see SMF-09") from stealing that other
    subtask's prompt slot.
    """
    for match in ARTIFACT_OWNER_PATTERN.finditer(block):
        candidate = match.group(1)
        if candidate in subtask_ids:
            return candidate
    return None


def associate_prompts(subtask_ids: list[str], prompt_blocks: list[str]) -> dict[str, str]:
    by_id: dict[str, str] = {}
    remaining_ids = list(subtask_ids)

    for block in prompt_blocks:
        # Prefer the block's artifact-path owner so cross-references in the body
        # cannot mis-associate the block to a different subtask.
        owner = owner_id_for_block(block, subtask_ids)
        if owner is not None and owner not in by_id:
            by_id[owner] = block
            if owner in remaining_ids:
                remaining_ids.remove(owner)
            continue
        # Fallback for plans whose prompts do not carry an artifact-path owner:
        # match any not-yet-assigned id present in the block, else assign the
        # next remaining id positionally.
        block_ids = [subtask_id for subtask_id in remaining_ids if subtask_id in block]
        if block_ids:
            by_id[block_ids[0]] = block
            remaining_ids.remove(block_ids[0])
            continue
        if remaining_ids:
            by_id[remaining_ids.pop(0)] = block

    return by_id


def order_subtask_ids(all_ids: list[str], prompts_by_subtask: dict[str, str]) -> list[str]:
    """Order subtask ids by their implementer-prompt position in the document.

    ``prompts_by_subtask`` is built in prompt-block order, so its keys already
    follow the plan's subtask sequence. Ordering by those keys avoids the
    misordering that arises when an id is first mentioned out of sequence in
    overview prose (e.g. a "PGS-01 ... PGS-04" summary line that would otherwise
    place PGS-04 second). Any ids without a prompt block keep their first-seen
    order, appended after the prompt-backed ids.
    """
    ordered = list(prompts_by_subtask.keys())
    for subtask_id in all_ids:
        if subtask_id not in ordered:
            ordered.append(subtask_id)
    return ordered


def build_result(plan_path: Path) -> dict[str, object]:
    text = read_text(plan_path)
    all_ids = extract_subtask_ids(text)
    dependency_lines = extract_dependency_lines(text)
    prompt_blocks = split_prompt_blocks(text)
    prompts_by_subtask = associate_prompts(all_ids, prompt_blocks)
    subtask_ids = order_subtask_ids(all_ids, prompts_by_subtask)
    security_required = [
        subtask_id
        for subtask_id, block in prompts_by_subtask.items()
        if SECURITY_MARKER.search(block)
    ]

    return {
        "plan_path": plan_path.as_posix(),
        "output_artifact_path": extract_output_artifact_path(text),
        "subtask_ids": subtask_ids,
        "dependency_lines": dependency_lines,
        "parallelizable_subtask_ids": extract_parallelizable_ids(text, subtask_ids),
        "security_required_subtask_ids": security_required,
        "implementer_prompt_count": len(prompt_blocks),
        "implementer_prompts_by_subtask": prompts_by_subtask,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Parse a planner plan artifact into coordinator-friendly JSON.")
    parser.add_argument("plan_path", help="Path to the plan markdown artifact.")
    args = parser.parse_args()

    plan_path = Path(args.plan_path)
    result = build_result(plan_path)
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
