Security Review Report

Scope reviewed:
- P5-5 (final subtask) of phase5-save-format-hardening: the versioned save-file header, aggregate load-failure propagation, and load-error UX -- i.e. the top-level orchestration of the untrusted save-file load path that the whole hardening effort funnels into.
- Reviewed as committed, read-only: src/strategic/FGame.cpp + include/strategic/FGame.h (magic/version gate, reportLoadError helper, aggregated load-error propagation, fixed-width header counts/IDs); src/FMainFrame.cpp + include/FMainFrame.h (onOpen abort/reset/no-menu-enable UX); supporting primitives src/core/FPObject.cpp/.h and include/core/FSaveFormat.h; behavioral tests tests/strategic/FGameSaveFormatTest.*, and GUI onOpen failed-load coverage in tests/gui/StrategicGuiLiveTest.cpp.
- Verification: built and ran the top-level SSWTests runner from this worktree -- OK (289 tests), including the full FGameSaveFormatTest malformed-input rejection suite (wrong magic, unsupported version, truncated-after-header, unknown factory type, console fallback, byte-for-byte round-trip).

Why specialist review was triggered:
- Loading a save file is an untrusted-input path and P5-5 is the aggregate loader boundary. Wrong magic, unsupported version, truncated stream, and unknown factory type must ALL be rejected with a nonzero return and no partially-constructed live FGame singleton. Plan Section 7 marks every stream-parsing subtask 'Security review: required'.
- The coordinator explicitly requested specialist disposition of two pre-existing findings surfaced by the Tester that sit outside P5-5's allowed file list.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md -- Section 4 (P5-5 acceptance criteria), Section 7 (Security Notes: untrusted-input path).

Findings

BLOCKING
- None

WARNING
- src/strategic/FPlayer.cpp:154 - FPlayer::load() ignores the return of v->load() (m_unattached loop line 154, m_destroyed loop line 177) and f->load() (fleet loop line 160); FFleet::load() likewise ignores v->load() for ships (src/strategic/FFleet.cpp:197). A stream truncated deep inside fleet/ship data makes FFleet/FVehicle::load() return nonzero, but FPlayer::load() discards that and returns 0, so FGame::load()'s per-player check (src/strategic/FGame.cpp:710) observes success -- the aggregate abort P5-5 relies on is bypassed for deep-truncation inputs.
  Severity: LOW (memory-safety/DoS lens) / MEDIUM (hardening-completeness lens). Exploitability is low: all reads are bounded (readString capped at kMaxSerializedStringBytes=64KB, readU32=4 bytes, read<T>=sizeof) and the stream failbit is sticky, so a failed stream yields no out-of-bounds read, no unbounded allocation, and no NULL deref (createShip is null-checked before use). Worst case is silent acceptance of a corrupt/truncated save producing a logically-inconsistent game (partial fleet, fields left at constructor defaults) -- a data-integrity/robustness defect, not memory corruption or RCE. DISPOSITION: does NOT block P5-5. P5-5's own FGame::load() aggregation correctly checks every sub-load return; the gap is upstream in FPlayer::load()/FFleet::load(), outside P5-5's allowed files (P5-3 scope). RECORD AS FOLLOW-UP DEFECT for the user: check the FFleet::load()/FVehicle::load() returns in FPlayer::load()'s three loops and the ship-load return in FFleet::load(). The Tester deliberately truncated right after the 8-byte header (FGameSaveFormatTest.cpp:251-262) to avoid this documented gap, so the abort guarantee is only partially proven by tests.
- src/FMainFrame.cpp:169 - With a shown FMainFrame, onOpen() calls m_drawingPanel->setGame(m_game) (line 157) BEFORE load; a failed load calls reportLoadError() -> IStrategicUI::showMessage(), whose nested modal event loop can deliver a paint to FGamePanel. FGamePanel::onPaint -> WXGameDisplay::draw -> WXMapDisplay::getScale -> FMap::getMaxSize() dereferences the still-NULL FMap singleton (load failed before m_universe was assigned), segfaulting the process.
  Severity: MEDIUM. A reliable crash / availability DoS on the exact load-error UX P5-5 introduces, triggerable by opening a corrupt / wrong-magic / wrong-version / truncated .ssw from the real GUI -- instead of the intended graceful error dialog and return to the no-game state. It is a NULL-pointer read (segfault), not memory corruption or RCE, and requires the frame shown plus a repaint during the modal. DISPOSITION: does NOT block P5-5 on file scope -- the canonical fix is a NULL-FMap guard in the gui draw path (FGamePanel::onPaint / WXGameDisplay::draw / WXMapDisplay::getScale), outside P5-5's allowed files. A partial IN-SCOPE mitigation would be to defer m_drawingPanel->setGame(...) in onOpen() until after a successful load, but the proper fix is the gui-layer NULL guard. The Tester documented this and avoided it by not showing the frame in the onOpen failed-load test (StrategicGuiLiveTest.cpp:2323-2404). RECORD AS FOLLOW-UP DEFECT for the user; recommend prompt gui-layer remediation since it degrades the delivered load-error UX.

NOTE
- src/strategic/FGame.cpp:639 - FGame::load() validates the magic FIRST (readU32 failure OR magic != kSaveMagic -> nonzero, before any game data is interpreted or allocated) and the version SECOND (unsupported -> nonzero); every header-field read, the nested m_universe->load(), the player count, and each player->load() return is checked; a failed player load deletes the just-allocated FPlayer before returning nonzero. reportLoadError() routes through m_ui->showMessage() when m_ui != NULL and falls back to std::cout otherwise, using fixed string literals (no format-string / injection hazard). onOpen() aborts via resetGame() (delete m_game -> ~FGame frees m_universe and all players; game-dependent menu items disabled, none enabled) whenever !is.is_open() or load() != 0.
  These are the P5-5-scoped behaviors the subtask owns; all are implemented correctly and backed by passing behavioral tests. No half-built FGame is left reachable in the GUI flow because the whole failed FGame is discarded by resetGame().

Test sufficiency assessment:
- Strong behavioral coverage. FGameSaveFormatTest asserts byte-level magic/version header emission, a byte-for-byte full-game save->load round-trip, and four distinct malformed-input rejection paths (wrong magic, unsupported version, truncated-after-header, unknown factory type) each requiring a nonzero load() and exactly one IStrategicUI::showMessage(), plus a no-UI console-fallback path. StrategicGuiLiveTest covers onOpen() failed-load: game-dependent menu items stay disabled and the frame returns to the no-game state.
- Confirmed by running the suite from this worktree: SSWTests -> OK (289 tests). Tests construct real streams/objects and assert runtime outcomes, satisfying the repo behavioral-verification mandate.
- Transparent gap tied to Finding #1: deep-truncation-inside-fleet is intentionally NOT tested (it would fail, since load() returns 0, and the fix is out of P5-5 scope), so the aggregate-abort guarantee is only partially proven. Acceptable/documented for P5-5; should be closed when Finding #1 is remediated in P5-3-scope files.

Documentation / operational guidance assessment:
- Adequate. Doxygen headers on FGame::save/load and reportLoadError document the magic/version header, the ordered validation, the aggregate error contract, and the IStrategicUI/console-fallback routing; include/core/FSaveFormat.h documents kSaveMagic / kSaveFormatVersion / kMaxSerializedStringBytes. AGENTS.md (lines 243 and 404) and doc/UsersGuide.md describe the versioned, fixed-width little-endian format, the up-front magic/version rejection, and the one-time compatibility break.
- Recommendation: record the two follow-up defects (Findings #1 and #2) in doc/deferred-tasks.md (or the roadmap) so the residual untrusted-input-path gaps are tracked rather than lost, since P5-5 is the phase's final subtask.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-5/security_report.md
- artifacts/phase5-save-format-hardening/P5-5/security_result.json

Outcome:
- CONDITIONAL PASS
