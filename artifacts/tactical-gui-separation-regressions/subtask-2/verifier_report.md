# Verifier Report — Subtask 2 Move-Route Selection Regression

## Agent Activation
- Requested agent: `verifier`
- Repository-local verifier definition: not found in `/home/tstephen/repos/SSW`
- Shared verifier definition: found at `/home/tstephen/repos/agents/agents/verifier.yaml`
- Precedence decision: followed the shared verifier definition because no repository-local override existed and the user explicitly directed that contract.
- Governing workflow obligations followed:
  - review the combined implementer/tester/documenter scope from the assigned verifier worktree;
  - keep project files read-only and write only required verifier artifacts in the shared artifact directory;
  - assess correctness, security, conventions, test sufficiency, and documentation accuracy;
  - produce structured findings with file/line references and a final verdict; and
  - stage and commit only the verifier artifact files.

## Review Scope Summary
- Worktree: `/home/tstephen/.copilot/session-state/a133f2f5-4873-4897-9efa-632892009c61/worktrees/gui_sep-verifier-20260404`
- Verified branch: `gui_sep-verifier-20260404`
- Base/root branch context: `gui_sep`
- Compared scope: `gui_sep..HEAD`
- Reviewed implementation/tests/docs in scope:
  - `src/tactical/FTacticalGame.cpp`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.h`
  - `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`
  - `doc/DesignNotes.md`
  - reviewer handoff artifacts under `artifacts/tactical-gui-separation-regressions/subtask-2`
- Confirmed unchanged renderer file: `src/tactical/FBattleBoard.cpp` is absent from `git diff --name-only gui_sep..HEAD`.

## Acceptance Criteria Reference
Evaluated against the handoff acceptance criteria for exact-click route extension, exact-click later-hex endpoint selection, exact-click trimming to earlier path hexes, movement counter/heading consistency, and renderer-contract alignment.

## Convention Files Considered
- `/home/tstephen/repos/SSW/AGENTS.md`
- `/home/tstephen/repos/agents/AGENTS_LOOKUP.md`
- `/home/tstephen/repos/agents/agents/verifier.yaml`

## Findings

### BLOCKING
- No blocking findings identified.

### WARNING
1. **The new regression fixture is not wired into the tactical module's normal test build/run path.**  
   - Evidence: `tests/tactical/Makefile:29,45-49` omits `FTacticalMoveRouteSelectionTest.o` from both `OBJS` and the archive inputs, and `tests/tactical/TacticalTests.cpp:8-22,28-42` neither includes nor registers `FTacticalMoveRouteSelectionTest`.  
   - Why it matters: the documented `cd tests/tactical && make && ./TacticalTests` validation path cannot compile or execute the new fixture, so future routine test runs will not catch regressions in this exact-click movement behavior.

2. **The design-note validation section is not fully reproducible and overstates how the route-selection fixture is exercised.**  
   - Evidence: `doc/DesignNotes.md:742-765` says the new fixture “locks that behavior” and lists validation commands, but the first listed command is the tactical suite that does not include the fixture, while the second command block is only a comment placeholder rather than a runnable command.  
   - Why it matters: the documentation does not accurately tell maintainers how the shipped regression coverage is actually executed, which weakens the documentation-accuracy requirement for this subtask.

### NOTE
1. **The new test header guard diverges from the repository's stated test-header convention.**  
   - Evidence: `tests/tactical/FTacticalMoveRouteSelectionTest.h:8-9` uses `_FTACTICALMOVEROUTESELECTIONTEST_H_`, while `AGENTS.md:137-142` specifies test include guards without a leading underscore.  
   - Why it matters: this is a small convention mismatch, but it also uses a reserved leading-underscore identifier pattern.

## Correctness Review
- `src/tactical/FTacticalGame.cpp:917-998` fixes the off-by-one behavior by incrementing the matched path-count in `findHexInList(...)` before returning, which makes `handleMoveHexSelection(...)` add exactly the number of hexes implied by the clicked highlight.
- The existing trim-path branch at `src/tactical/FTacticalGame.cpp:936-945` already removes trailing points back to the clicked path hex and recomputes remaining moves from the trimmed endpoint.
- Source review found no need for a renderer-side adjustment; the fix stays model-local and `FBattleBoard.cpp` remained unchanged as required.

## Security Review
- No security findings identified in the scoped implementation or documentation changes.

## Test Sufficiency Assessment
- The standalone `FTacticalMoveRouteSelectionTest` fixture appears logically sufficient for the stated acceptance criteria because it exercises first-step extension, later-hex endpoint selection, earlier-path trimming, and heading/`nMoved` invariants.
- However, coverage is only partially sufficient in delivered form because the fixture is not integrated into the standard tactical runner/build path (`tests/tactical/Makefile` and `tests/tactical/TacticalTests.cpp`), so ordinary suite execution does not enforce it.

## Documentation Accuracy Assessment
- The behavioral description in `doc/DesignNotes.md:737-751` matches the shipped model change and correctly states that no `FBattleBoard` adjustment was required.
- The validation subsection in `doc/DesignNotes.md:753-765` is inaccurate/incomplete because it implies routine suite validation of a fixture that is not part of that suite and does not provide the actual standalone command.

## Final Verdict
**PASS**

No blocking findings were identified, and the model fix itself matches the acceptance criteria. The warnings above should still be addressed so the regression coverage and documentation are part of the normal, reproducible tactical validation path.
