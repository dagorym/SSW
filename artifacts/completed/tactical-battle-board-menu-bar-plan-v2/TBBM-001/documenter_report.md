## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — documented the `FBattleScreen` wxFrame migration, the class-owned modal compatibility shim, the modal-first close ordering, and the lifecycle-counter validation context for tactical launch flows.
- **`include/tactical/FBattleScreen.h`** — refreshed in-code documentation for the frame-backed tactical top-level, modal shim APIs, and `closeBattleScreen(...)` modal-first behavior.

### Summary
Updated the shipped technical documentation to match TBBM-001 as implemented and tested: `FBattleScreen` is now a menu-capable `wxFrame` that preserves legacy blocking launch sites through class-owned `ShowModal()` / `EndModal(...)` compatibility methods, keeps `closeBattleScreen(...)` modal-first before the non-modal destroy path, and retains lifecycle-counter coverage for deterministic battle-screen teardown. Assumption: used `master` merge-base `7dbdb3fd1b58e106dbf7f7e285123bebae105696` as the comparison base because this repository has no `main` branch and that merge base matches the story plan branch tip.

### Validation
- `cd tests && make tactical-tests`
- `cd tests/gui && make`

### Documentation Commit
- `89e38130bf2743a16ab39afd125ca50b628369d7`

### Commit Message
`Document FBattleScreen frame modal shim`
