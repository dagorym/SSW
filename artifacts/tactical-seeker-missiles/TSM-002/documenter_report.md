## Documenter Report

### Files Updated
- **`include/tactical/FTacticalGame.h`** — Added Doxygen comments for the generalized placement-source discovery, selection, ordnance placement, deployable-hex checks, and legacy mine-compatibility entry points introduced by TSM-002.
- **`include/tactical/FBattleScreen.h`** — Documented the forwarding seam for generalized ordnance placement/source-selection APIs and the retained mine-compatibility wrappers.
- **`doc/DesignNotes.md`** — Updated the tactical delegation notes so the architecture narrative now reflects generalized ordnance placement, exact source selection, and the expanded delegation-test surface.

### Summary
Documented the shipped TSM-002 behavior without changing runtime logic: setup placement now generalizes from mines to source-tracked mines and inactive seekers, legacy mine entry points remain compatibility wrappers, and the tactical design notes now describe the broader ordnance-placement seam and related delegation coverage. Assumption: the safest comparison base is commit `b838b18`, the direct parent of implementation commit `d9c2e41fb18f2ab4f90579583c06f438123217d7`, because it is the last shared commit before TSM-002 work begins on this branch stack. Per repository policy and plan guidance, `doc/rules/tactical_operations_manual.md` was reviewed for scope only and was not edited.

### Validation
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (158 tests)`

### Commit Message
`docs: document tactical ordnance placement`
