# Implementer Report

Status:
- success

Task summary:
- Add the versioned save-file header (kSaveMagic/kSaveFormatVersion) to FGame::save/load, convert FGame's own counts/IDs (m_currentPlayer, player count) to the P5-1 fixed-width little-endian helpers, aggregate load-error propagation across every header field, m_universe->load(), and each player->load() in FGame::load (reporting through IStrategicUI::showMessage with a console fallback and returning nonzero on any failure), and add the abort/preserve/report loader UX in FMainFrame::onOpen (check std::ifstream::is_open() and the FGame::load() return code, tearing the freshly-created failed game back down to the no-game state via the existing resetGame() path on failure without enabling turn/menu items, leaving the success path unchanged).

Changed files:
- include/strategic/FGame.h
- src/strategic/FGame.cpp
- src/FMainFrame.cpp
- include/FMainFrame.h
- tests/strategic/FGameTest.cpp

Validation commands run:
- make (repo root)
- make check (repo root; runs SSWTests, TacticalTests, and GuiTests under xvfb-run)

Validation outcome:
- make check is green across all three suites: SSWTests 282/282 passed, TacticalTests 253/253 passed, GuiTests 81/81 passed. Also manually verified (ad hoc scratch program, not committed) that FGame::load() returns nonzero and calls IStrategicUI::showMessage() exactly once for each of: wrong magic, unsupported version, a truncated stream, and an unknown ship factory type inside a player's unattached-ship list; and that a full init()->save()->load() round trip through a fresh FGame singleton restores round, current player ID, and player count.

Implementation/code commit hash:
- d9c268dad7517a4af71a86f38a334672bdda13ad

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-5/implementer_report.md
- artifacts/phase5-save-format-hardening/P5-5/tester_prompt.txt
- artifacts/phase5-save-format-hardening/P5-5/implementer_result.json

Implementation context:
- include/core/FSaveFormat.h defines kSaveMagic (0x31575353u), kSaveFormatVersion (1u), and kMaxSerializedStringBytes; include/core/FPObject.h provides the protected writeU32/readU32 fixed-width little-endian helpers FGame::save/load now use for the header and for m_currentPlayer/player-count.
- FGame::save() writes writeU32(kSaveMagic) then writeU32(kSaveFormatVersion) as the very first bytes, before any other game data.
- FGame::load() reads and validates magic first (return 1 on mismatch or read failure), then version (return 1 if unsupported or read failure); every subsequent header-field read, m_universe->load(is), and each `new FPlayer; p->load(is)` are all checked, with the private FGame::reportLoadError(const std::string&) helper (declared in include/strategic/FGame.h, defined in src/strategic/FGame.cpp) routing the failure message to m_ui->showMessage("SSW Load Error", detail) when m_ui != NULL, or std::cout otherwise, mirroring the existing init()-method console-fallback convention in this file.
- A player that fails p->load(is) (including the unknown-factory-type case already handled inside FPlayer::load()/FVehicle::createShip()) is deleted before FGame::load() returns nonzero; because FMainFrame::onOpen() discards the entire freshly-created FGame instance on any load() failure (via resetGame()), FGame::load() itself does not need to roll back any players already pushed into m_players -- the whole object is torn down by the caller.
- FMainFrame::onOpen() wraps the existing post-load menu-enable/turn-state block in an else branch guarded by `if (!is.is_open() || m_game->load(is) != 0) { resetGame(); } else { ...unchanged... }`; resetGame() (src/FMainFrame.cpp, pre-existing) deletes m_game, sets it to NULL, and disables the same menu items onNew()'s failure path already relies on, so this reuses an established reset path rather than inventing new teardown logic.
- DEVIATION NOTE (read before writing new tests): tests/strategic/FGameTest.cpp's readLossCounters() helper parses FGame::save()'s raw byte output directly (a real behavioral read of production save() bytes, not a source-text/string-contract check -- see its doc comment). Adding the magic+version header shifted every subsequent field's byte offset by 8 bytes, which broke 8 existing FGameTest cases that depend on this helper (loss-counter and checkForVictory tests). This is a direct, mandated, and unavoidable consequence of this subtask's own acceptance criteria (writing the header first) -- not a source-string test being weakened. Per the validation-triage classification, the implementer made the smallest viable fix: readLossCounters() now reads two extra uint32_t fields (magic, version) and discards them before parsing the same fixed-layout fields it always has; no assertions were changed or removed. This one file (tests/strategic/FGameTest.cpp) is therefore included in this commit alongside the four originally-approved files, and validate_commit_scope.py was re-run with it explicitly listed to confirm in-scope status. The Tester should be aware this helper now assumes the current wire format's 8-byte header prefix if it extends readLossCounters() further.

Expected validation failures carried forward:
- None
