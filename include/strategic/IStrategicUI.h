/**
 * @file IStrategicUI.h
 * @brief Pure abstract strategic-layer UI boundary for dialogs, notifications, and redraw requests.
 *
 * Defines IStrategicUI, the non-wx seam through which strategic game logic
 * (FGame and its collaborators) initiates all UI interactions: informational
 * messages, victory and jump-failure notifications, player setup workflows,
 * system and fleet inspection dialogs, retreat-condition selection, combat
 * selection, and map redraw requests.  Concrete implementations live in the
 * gui module (WXStrategicUI); mock implementations are used in strategic
 * regression tests.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: May 15, 2026
 */

#ifndef ISTRATEGICUI_H
#define ISTRATEGICUI_H

#include <string>

#include "strategic/StrategicTypes.h"

namespace Frontier {

/**
 * @brief Pure abstract interface through which strategic game logic requests UI actions.
 *
 * Strategic model code (FGame, turn logic, combat resolution) calls this
 * interface instead of touching wx types directly, keeping the strategic
 * module free of GUI dependencies.  All methods are pure virtual; implementors
 * must provide concrete behavior for every entry point.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: May 15, 2026
 */
class IStrategicUI {
 public:
  virtual ~IStrategicUI() {}

  /**
   * @brief Display a simple informational message dialog to the player.
   *
   * The dialog should block until the player dismisses it.
   *
   * @param title Caption shown in the dialog title bar.
   * @param body  Main message text shown in the dialog body.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void showMessage(const std::string& title, const std::string& body) = 0;

  /**
   * @brief Notify the player that a fleet's jump attempt has failed.
   *
   * Implementations should present an appropriate alert so the player knows
   * the fleet did not successfully transit to its destination.
   *
   * @param fleetName Display name of the fleet whose jump failed.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void notifyFailedJump(const std::string& fleetName) = 0;

  /**
   * @brief Notify the player of the game's final victory outcome.
   *
   * Called at end-of-game to present the result.
   *
   * @param result Victory code: 1 = UPF wins, 2 = Sathar wins, 3 = Draw.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void notifyVictory(int result) = 0;

  /**
   * @brief Present the retreat-condition selection dialog to the player.
   *
   * Allows the player to choose under what circumstances their forces will
   * retreat from combat.
   *
   * @return 0 on success (condition selected), non-zero if the player cancels.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual int selectRetreatCondition() = 0;

  /**
   * @brief Run the UPF unattached-fleet setup workflow for the given player.
   *
   * Presents the UI sequence that lets the UPF player configure fleets that
   * are not currently attached to a system at game start or during setup.
   *
   * @param player Pointer to the UPF player whose fleets are being set up.
   * @param map    Pointer to the strategic map used for system and route context.
   * @return 0 on success, non-zero if the player cancels or setup is aborted.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual int runUPFUnattachedSetup(FPlayer* player, FMap* map) = 0;

  /**
   * @brief Run the Sathar fleet setup workflow for the given player.
   *
   * Presents the UI sequence that lets the Sathar player position and
   * configure their fleets, with different behavior for initial game setup
   * versus mid-game reinforcement.
   *
   * @param player         Pointer to the Sathar player whose fleets are being set up.
   * @param map            Pointer to the strategic map providing system and route context.
   * @param isInitialSetup True when called during the initial game-setup phase;
   *                       false when called for mid-game fleet placement.
   * @return 0 on success, non-zero if the player cancels or setup is aborted.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual int runSatharFleetSetup(FPlayer* player, FMap* map, bool isInitialSetup) = 0;

  /**
   * @brief Show the system-details dialog for the selected star system.
   *
   * Presents information about the system, its occupying fleets, and available
   * player actions, using the map and player for context resolution.
   *
   * @param sys    Pointer to the star system being inspected.
   * @param map    Pointer to the strategic map for route and neighbour context.
   * @param player Pointer to the active player viewing the system.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void showSystemDialog(FSystem* sys, FMap* map, FPlayer* player) = 0;

  /**
   * @brief Show the fleet-details dialog for the given fleet.
   *
   * Presents the fleet's composition, status, and navigation state.
   * Both @p location and @p destination may be null when the fleet is
   * not currently in or bound for a known system.
   *
   * @param fleet       Pointer to the fleet being inspected.
   * @param location    Pointer to the system where the fleet currently resides,
   *                    or nullptr if in transit.
   * @param destination Pointer to the system the fleet is travelling toward,
   *                    or nullptr if no destination is set.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void showFleetDialog(FFleet* fleet, FSystem* location, FSystem* destination) = 0;

  /**
   * @brief Display the current retreat conditions text to the player.
   *
   * Used to present a read-only view of the active retreat rules without
   * offering a selection.
   *
   * @param text Formatted retreat-conditions text to display.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void showRetreatConditions(const std::string& text) = 0;

  /**
   * @brief Present the combat-selection dialog for a conflict at a given system.
   *
   * Allows the player to select which attacker and defender fleets participate
   * in a combat engagement at the specified system.
   *
   * @param sys       Pointer to the star system where combat will take place.
   * @param defenders List of defending fleets present at the system.
   * @param attackers List of attacking fleets initiating the combat.
   * @param players   Pointer to the full player list, used to resolve fleet
   *                  ownership and display names.
   * @return A dialog-dependent status code; 0 typically indicates a valid
   *         selection was made, non-zero indicates cancellation or an error.
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual int selectCombat(FSystem* sys,
                           FleetList defenders,
                           FleetList attackers,
                           PlayerList* players) = 0;

  /**
   * @brief Request that the UI layer redraws the strategic map.
   *
   * Called by strategic model code after state changes that should be
   * reflected immediately in the map display (e.g., fleet movement, system
   * ownership changes).
   *
   * @author Tom Stephens, gpt-5.3-codex (medium)
   * @date Created: Mar 28, 2026
   * @date Last Modified: May 15, 2026
   */
  virtual void requestRedraw() = 0;
};

}  // namespace Frontier

#endif  // ISTRATEGICUI_H
