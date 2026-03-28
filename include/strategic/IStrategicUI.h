/**
 * @file IStrategicUI.h
 * @brief Strategic-layer UI boundary for dialogs, notifications, and redraw requests.
 */

#ifndef ISTRATEGICUI_H
#define ISTRATEGICUI_H

#include <string>

#include "strategic/StrategicTypes.h"

namespace Frontier {

/**
 * Pure abstract interface used by strategic game logic to request UI actions.
 */
class IStrategicUI {
 public:
  virtual ~IStrategicUI() {}

  /** Show a simple informational message dialog. */
  virtual void showMessage(const std::string& title, const std::string& body) = 0;

  /** Notify that a fleet failed its jump attempt. */
  virtual void notifyFailedJump(const std::string& fleetName) = 0;

  /** Notify victory result: 1=UPF, 2=Sathar, 3=Draw. */
  virtual void notifyVictory(int result) = 0;

  /** Select retreat condition; returns 0 on success, non-zero to cancel. */
  virtual int selectRetreatCondition() = 0;

  /** Run UPF unattached fleet setup; returns 0 on success, non-zero to cancel. */
  virtual int runUPFUnattachedSetup(FPlayer* player, FMap* map) = 0;

  /** Run Sathar fleet setup; returns 0 on success, non-zero to cancel. */
  virtual int runSatharFleetSetup(FPlayer* player, FMap* map, bool isInitialSetup) = 0;

  /** Show system details dialog for the selected system context. */
  virtual void showSystemDialog(FSystem* sys, FMap* map, FPlayer* player) = 0;

  /** Show fleet details dialog for current location and destination context. */
  virtual void showFleetDialog(FFleet* fleet, FSystem* location, FSystem* destination) = 0;

  /** Show retreat conditions text to the player. */
  virtual void showRetreatConditions(const std::string& text) = 0;

  /** Select one combat at the given system; returns dialog-dependent status code. */
  virtual int selectCombat(FSystem* sys,
                           FleetList defenders,
                           FleetList attackers,
                           PlayerList* players) = 0;

  /** Request a strategic map redraw from the UI layer. */
  virtual void requestRedraw() = 0;
};

}  // namespace Frontier

#endif  // ISTRATEGICUI_H
