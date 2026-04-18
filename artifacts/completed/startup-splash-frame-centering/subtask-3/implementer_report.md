Implementer Report — startup-splash-frame-centering / subtask-3

Plan step status
- Preflight scope check: completed. Stayed within allowed files plus required artifact outputs.
- Implementation: completed. Added live GUI startup seam tests for both SSW and BattleSim suites.
- Validation: completed with documented xvfb limitation fallback.
- Commit flow: completed with separate code and artifact commits.

Task goal completed
- Added live GUI regression coverage for extracted startup seams.
- Verified startup path creates splash + startup frame for SSW and BattleSim.
- Verified splash and startup frame are centered on display in both suites.
- Verified splash stays above frame while both are visible using the narrowest reliable observable check (`wxSTAY_ON_TOP` on live splash window with simultaneous visibility).

Files changed (implementation commit)
- tests/gui/StrategicGuiLiveTest.h
- tests/gui/StrategicGuiLiveTest.cpp
- tests/gui/BattleSimGuiLiveTest.h
- tests/gui/BattleSimGuiLiveTest.cpp

Validation commands and outcomes
1) Baseline
- `cd tests/gui && make && xvfb-run -a ./GuiTests` → failed (`xvfb-run: command not found`)
- `cd tests/gui && ./GuiTests` (fallback) → passed (`OK (29 tests)`)

2) Post-change validation
- `cd tests/gui && make && xvfb-run -a ./GuiTests` → failed (`xvfb-run: command not found`)
- `cd tests/gui && ./GuiTests` (fallback) → passed (`OK (31 tests)`)

Notes
- No Makefile updates were needed; existing GUI test target built and ran with the added coverage.
- Existing debug lines from repeated image-handler registration were observed but non-fatal.
