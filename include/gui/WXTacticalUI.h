/**
 * @file WXTacticalUI.h
 * @brief Concrete bridge from tactical game logic to wxWidgets dialogs.
 * @author Tom Stephens, gpt-5.3-codex (medium), claude-sonnet-4-6 (medium), Claude Sonnet 5 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Jul 03, 2026
 */

#ifndef _WXTACTICALUI_H_
#define _WXTACTICALUI_H_

#include "tactical/ITacticalUI.h"

#include <vector>

class wxDialog;
class wxWindow;

namespace Frontier {

/**
 * @brief Concrete bridge from tactical game logic to wx dialogs.
 *
 * Implements ITacticalUI callbacks by opening wx modal dialogs and routing
 * redraw/winner notifications to the tactical parent window.
 *
 * TMFR-01: the currently active modal dialogs are tracked as an
 * innermost-first STACK (rather than a single pointer) so that FBattleScreen
 * can call dismissActiveDialog() before it hides itself and have EVERY live
 * child dialog dismissed, not just the most recently opened one. This
 * prevents the duplicate wxDialog::EndModal assert that otherwise occurs when
 * the parent frame's Show(false) triggers wxGTK's automatic EndModal on a
 * still-modal child dialog that was left untracked because a second dialog
 * had overwritten the single tracked pointer.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium), claude-sonnet-4-6 (medium), Claude Sonnet 5 (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Jul 03, 2026
 */
class WXTacticalUI : public ITacticalUI {
 public:
  /**
   * @brief Construct the tactical UI adapter.
   * @param parent Parent window used for modal ownership and redraws.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  explicit WXTacticalUI(wxWindow* parent);
  /**
   * @brief Destroy the tactical UI adapter.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  virtual ~WXTacticalUI();

  /**
   * @brief Request a redraw from the owning parent window.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  void requestRedraw();
  /**
   * @brief Show a tactical message dialog or fallback output.
   *
   * Pushes the dialog onto m_dialogStack before ShowModal() and pops it after,
   * so dismissActiveDialog() can find and dismiss it even if this call is
   * nested inside another dialog's own ShowModal().
   *
   * @param title Message-box title text.
   * @param body Message body text.
   * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Jul 03, 2026
   */
  void showMessage(const std::string& title, const std::string& body);
  /**
   * @brief Show the tactical damage summary modal dialog.
   *
   * Pushes the dialog onto m_dialogStack before ShowModal() and pops it after,
   * so dismissActiveDialog() can find and dismiss it even if this call is
   * nested inside another dialog's own ShowModal().
   *
   * @param summary Tactical report summary to display.
   * @return Modal result code from the summary dialog.
   * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Jul 03, 2026
   */
  int showDamageSummary(const FTacticalCombatReportSummary& summary);
  /**
   * @brief Run ICM assignment selection for defender ships.
   *
   * Pushes the dialog onto m_dialogStack before ShowModal() and pops it after,
   * so dismissActiveDialog() can find and dismiss it even if this call is
   * nested inside another dialog's own ShowModal().
   *
   * @param icmData Weapon-to-ICM assignment rows to edit.
   * @param defenders Defending ships used to validate ICM selection.
   * @return Dialog result code, or 1 when selection cannot be run.
   * @author Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Jul 03, 2026
   */
  int runICMSelection(std::vector<ICMData*>& icmData, VehicleList* defenders);
  /**
   * @brief Announce the battle winner.
   * @param attackerWins True when the attacking side won.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Mar 29, 2026
   */
  void notifyWinner(bool attackerWins);
  /**
   * @brief Configure optional auto-dismiss timeout for modal UI tests.
   * @param timeoutMs Timeout in milliseconds; zero disables auto-dismiss.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 29, 2026
   * @date Last Modified: Apr 04, 2026
   */
  static void setModalAutoDismissMs(int timeoutMs);

  /**
   * @brief Returns true when at least one child modal dialog is currently open.
   *
   * Used by FBattleScreen to check whether any child dialog must be dismissed
   * before the frame is hidden during the close lifecycle.
   *
   * @return True when the dialog stack contains at least one entry that is
   *         still modal (IsModal() true).
   * @author claude-sonnet-4-6 (medium), Claude Sonnet 5 (medium)
   * @date Created: Jun 30, 2026
   * @date Last Modified: Jul 03, 2026
   */
  bool hasPendingDialog() const;

  /**
   * @brief Dismiss every active child modal dialog, innermost-first.
   *
   * TMFR-01: showMessage(), showDamageSummary(), and runICMSelection() can
   * each be re-entered while an earlier call in the same chain is still
   * inside its own ShowModal() (for example, a callback triggered while a
   * damage-summary dialog is modal that itself opens an ICM-selection
   * dialog). Each open dialog is pushed onto m_dialogStack and popped once
   * its own ShowModal() call returns, so at any moment the stack holds every
   * currently-live child dialog with the most recently opened (innermost)
   * one last.
   *
   * Walks the stack from the back (innermost/most recently opened) to the
   * front (outermost) and calls EndModal(wxID_CANCEL) on every entry still
   * modal, so that all of their IsModal() flags are cleared before
   * FBattleScreen calls Show(false). This prevents the duplicate
   * wxDialog::EndModal assert that wxGTK raises when it tries to auto-dismiss
   * a still-modal child as a side-effect of hiding the parent frame -- a
   * single-pointer implementation could only dismiss the most recently opened
   * dialog and would leave any others to trigger that assert.
   *
   * Safe to call when no dialog is active (no-op in that case).
   *
   * @author claude-sonnet-4-6 (medium), Claude Sonnet 5 (medium)
   * @date Created: Jun 30, 2026
   * @date Last Modified: Jul 03, 2026
   */
  void dismissActiveDialog();

 private:
  static int s_modalAutoDismissMs;
  wxWindow* m_parent;
  /// Innermost-first stack of child modal dialogs currently open via
  /// showMessage, showDamageSummary, or runICMSelection. Each dialog is
  /// pushed immediately before its own ShowModal() call and popped
  /// immediately after that call returns, so the back of the vector is
  /// always the most recently opened (innermost) live dialog. Empty when no
  /// dialog is active.
  std::vector<wxDialog*> m_dialogStack;
};

}  // namespace Frontier

#endif  // _WXTACTICALUI_H_
