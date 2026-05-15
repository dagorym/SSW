/**
 * @file WXStrategicUI.h
 * @brief Concrete bridge from strategic game logic to wxWidgets dialogs.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Apr 17, 2026
 */

#ifndef _WXSTRATEGICUI_H_
#define _WXSTRATEGICUI_H_

#include <string>

#include "strategic/IStrategicUI.h"

class wxWindow;

namespace Frontier {

/**
 * @brief Concrete bridge from strategic game logic to wx dialogs.
 *
 * Implements the IStrategicUI seam with wx dialog and message surfaces while
 * preserving null-parent and no-runtime fallback behavior.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Apr 17, 2026
 */
class WXStrategicUI : public IStrategicUI {
 public:
  /**
   * @brief Construct the strategic UI adapter.
   * @param parent Parent window used to own dialogs and redraw requests.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Mar 28, 2026
   */
  explicit WXStrategicUI(wxWindow* parent);
  /**
   * @brief Destroy the strategic UI adapter.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Mar 28, 2026
   */
  virtual ~WXStrategicUI();

  /**
   * @brief Show an informational strategic message.
   * @param title Message-box title text.
   * @param body Message body text.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 04, 2026
   */
  void showMessage(const std::string& title, const std::string& body);
  /**
   * @brief Notify the player that a risk jump failed.
   * @param fleetName Name of the fleet that was lost.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Mar 28, 2026
   */
  void notifyFailedJump(const std::string& fleetName);
  /**
   * @brief Show campaign victory state messaging.
   * @param result Strategic victory code (UPF/Sathar/draw).
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Mar 28, 2026
   */
  void notifyVictory(int result);

  /**
   * @brief Prompt for Sathar retreat condition selection.
   * @return Modal dialog result or wxID_CANCEL when runtime is unavailable.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 17, 2026
   */
  int selectRetreatCondition();
  /**
   * @brief Run the UPF unattached-ship setup dialog.
   * @param player Player whose unattached ships are being assigned.
   * @param map Strategic map used by the setup dialog.
   * @return Dialog result code, 1 for invalid input, or wxID_CANCEL when no
   *         usable wx runtime exists.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 17, 2026
   */
  int runUPFUnattachedSetup(FPlayer* player, FMap* map);
  /**
   * @brief Run the Sathar fleet setup dialog.
   * @param player Sathar player being configured.
   * @param map Strategic map used by the setup dialog.
   * @param isInitialSetup True during initial setup flow.
   * @return Dialog result code, 1 for invalid input, or wxID_CANCEL when no
   *         usable wx runtime exists.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 17, 2026
   */
  int runSatharFleetSetup(FPlayer* player, FMap* map, bool isInitialSetup);

  /**
   * @brief Show read-only system information.
   * @param sys System being inspected.
   * @param map Strategic map context.
   * @param player Active player context.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 17, 2026
   */
  void showSystemDialog(FSystem* sys, FMap* map, FPlayer* player);
  /**
   * @brief Show fleet detail information.
   * @param fleet Fleet being inspected.
   * @param location Current system location.
   * @param destination Optional destination system.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 17, 2026
   */
  void showFleetDialog(FFleet* fleet, FSystem* location, FSystem* destination);
  /**
   * @brief Show current Sathar retreat condition text.
   * @param text Retreat condition text to display.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Mar 28, 2026
   */
  void showRetreatConditions(const std::string& text);

  /**
   * @brief Launch the combat-selection dialog for a contested system.
   * @param sys System where combat is being resolved.
   * @param defenders Candidate defending fleets and stations.
   * @param attackers Candidate attacking fleets.
   * @param players Player roster used by selection dialogs.
   * @return Dialog result code, 1 for invalid input, or wxID_CANCEL when no
   *         usable wx runtime exists.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Apr 17, 2026
   */
  int selectCombat(FSystem* sys,
                   FleetList defenders,
                   FleetList attackers,
                   PlayerList* players);

  /**
   * @brief Request strategic surface redraw on the owning parent.
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: Mar 28, 2026
   */
  void requestRedraw();

 private:
  wxWindow* m_parent;
};

}  // namespace Frontier

#endif  // _WXSTRATEGICUI_H_
