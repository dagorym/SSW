Implementation status: success

Plan step status:
1. Preflight scope check - completed
   - Goal: add reusable FGameConfig asset-path resolver for repo-relative assets.
   - Allowed files respected: include/core/FGameConfig.h, src/core/FGameConfig.cpp.
   - Acceptance criteria captured and validated against implementation.
   - Validation command assumption: planner-provided `cd tests/core && make && ./CoreTests` appears stale for this repo because `CoreTests` binary is not produced by tests/core/Makefile.
2. Implement incrementally - completed
   - Added new public API `FGameConfig::resolveAssetPath(const std::string&) const`.
   - Preserved existing `getBasePath()` lifecycle behavior.
   - Added executable-directory tracking and resolver candidate search logic without absolute machine paths.
3. Validate after changes - completed
   - Ran: `cd tests/core && make` (build succeeds; module makefile builds `libcoreTests.a` only).
   - Ran: `cd tests && make && ./SSWTests` (pass).
4. Completion gate - completed
   - Scope limited to approved files plus required artifacts.
   - Code committed, then artifacts written for second commit.

Files changed:
- include/core/FGameConfig.h
- src/core/FGameConfig.cpp

Validation outcomes:
- `cd tests/core && make` -> passed
- `cd tests && make && ./SSWTests` -> passed
- Note: `./CoreTests` from planner guidance is not present in this repository layout; used smallest equivalent existing runnable suite command.

Implementation/code commit hash:
- bd9710bbbd20dbd8da196741472f409452243a30
