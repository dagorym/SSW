Implementer Report — Subtask 1: Stabilize Tactical Spin-Control Layout

Plan step status:
1. Preflight scope check — completed.
   - Goal: stabilize tactical speed selection control path in FBattleDisplay and preserve tactical setup flow.
   - Allowed files respected: src/tactical/FBattleDisplay.cpp, tests/gui/TacticalGuiLiveTest.cpp, tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp.
   - Conditional src/gui/ICMSelectionGUI.cpp not touched (GtkSpinButton warning not reproduced in covered flow).
2. Implementation — completed.
   - Replaced free-positioned tactical controls in FBattleDisplay with sizer-managed layout.
   - Kept speed selection behavior intact (hidden by default, shown during setup, same onSetSpeed transition path).
   - Added layout refresh after speed control show/hide.
3. Validation — completed.
   - Baseline build/tests ran before edits.
   - Post-change build/tests ran and passed.
4. Completion gate — completed.
   - Implementation changes committed.
   - Required handoff artifacts written in shared artifact directory.

Files changed:
- src/tactical/FBattleDisplay.cpp
- tests/gui/TacticalGuiLiveTest.cpp
- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp

Validation commands run:
- Baseline:
  - `make -s`
  - `cd tests/tactical && make -s && ./TacticalTests`
  - `cd tests/gui && make -s && xvfb-run -a ./GuiTests`
- Post-change:
  - `make -s`
  - `cd tests/tactical && make -s && ./TacticalTests`
  - `cd tests/gui && set -o pipefail && make -s && xvfb-run -a ./GuiTests`

Validation outcomes:
- Root build: pass.
- Tactical tests: pass (88 tests).
- GUI tests: pass (24 tests).
- Covered GUI flow output: no `GtkSpinButton` assertion observed.

Implementation/code commit hash:
- `b876f58`
