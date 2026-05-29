# CLAUDE.md: Project-Specific Instructions

**IMPORTANT: These instructions OVERRIDE any default behavior and MUST be followed exactly as written.**

## Non-Negotiable: myteam Command Compliance

When you run ANY `myteam` command:
1. You MUST read the ENTIRE output, even if it's 100+ KB
2. You MUST NOT use `grep`, `head`, `tail`, `sed`, `awk`, or any filter that drops lines
3. You MUST NOT use `limit` or `offset` parameters when reading myteam output files  
4. If output is split across pages, read the whole file sequentially without sampling
5. If a `myteam` command result is too large to display, use the Read tool to consume the entire file without filters

This applies to all agents, all sessions, all computers. No exceptions.

**Rationale:** myteam commands return critical role definitions, skill specifications, and orchestration rules that must be fully understood before acting. Incomplete reads lead to violated constraints and broken workflows.

## Role and Skill Framework

- Read `AGENTS.md` completely before starting any work
- Load role-specific skills via `myteam get role <role>` and read the full output
- Follow skill loading rules and required workflow order exactly as specified in your role definition
- Do not substitute coordinator, implementer, tester, documenter, verifier, or reviewer work across roles

## Critical Constraints (From AGENTS.md)

- The document at `doc/rules/tactical_operations_manual.md` is non-volatile and MUST NOT be edited
- Module boundaries and non-GUI constraints must be strictly maintained
- All code must use the F-prefix naming convention
- Doxygen-style headers are mandatory in all header files
- Do not commit changes without explicit user approval (unless working within an approved workflow like coordinator orchestration)

## Architecture Reminders

- All game code lives in `namespace Frontier`
- Core modules: `core`, `strategic`, `ships`, `weapons`, `defenses`, `tactical`, `gui`, `battleSim`
- Non-GUI model modules must remain free of wx includes
- `IStrategicUI` and `ITacticalUI` are model-facing interface seams
- `FGame` is a singleton managed via `FGame::create()`

## Testing

- Tests use CppUnit and live under `tests/`
- Run `make tests` from repo root or `cd tests && make`
- GUI tests require an X display (use `xvfb-run` in headless environments)
- Do not skip pre-commit hooks unless explicitly instructed

## Build System

- Linux: `make` from repo root
- Windows: Open `SSW.sln` in Visual Studio 2022
- Requires wxWidgets 3.3.1
- Module linking order: `-lgui -ltactical -lweapons -ldefenses -lships -lstrategic -lcore`

## Workflow Notes

- When working with plans, the Coordinator role orchestrates Implementer → Tester → Documenter → Verifier chains
- Artifacts are stored in `artifacts/` with plan-level and subtask-level organization
- Final Reviewer pass happens only after all subtasks complete and merge back
- Use dedicated per-plan coordination branches (never `main` or `master` as base)
