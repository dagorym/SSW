## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated repository testing guidance to document the hardened close oracle, idle-driven cleanup expectations, and the expanded tactical/BattleSim close-path coverage.
- **`tests/gui/WXGuiTestHarness.h`** — Refined Doxygen comments so the harness contract now states that shown pending-delete top levels remain open until they are hidden or removed and that orphan cleanup unwinds modal dialogs before waiting on idle cleanup.
- **`tests/gui/TacticalGuiLiveTest.h`** — Updated fixture and test comments to describe the stronger non-modal close assertions requiring hidden/absent battle screens and settled lifecycle counters.
- **`tests/gui/BattleSimGuiLiveTest.h`** — Updated fixture and modal close-test comments to describe caller unwind and no-live-instance expectations for both menu and title-bar close vectors.

### Summary
Documented the implemented TBSCPP-002 close-path behavior without changing runtime code or tests. Repository guidance and in-code documentation now reflect that GUI close waits cannot pass on `IsBeingDeleted()` alone, cleanup must hide shown pending-delete windows while wx idle cleanup settles, and both tactical and BattleSim close regressions require battle screens to stop showing with no live `FBattleScreen` instances left behind. Assumption: `AGENTS.md` and the affected test-fixture headers were the correct documentation targets because repository evidence showed this story changed contributor/testing guidance and existing in-code documentation, not a user-facing feature document.

### Commit Message
`docs: record TBSCPP-002 close-path coverage`

### Documentation Commit
- `1a90280e23a51f270f1cb835c0af05b98a557743`
