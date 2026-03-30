/**
 * @file ITacticalUI.h
 * @brief Tactical-layer UI boundary for redraws, tactical dialogs, and winner notifications.
 */

#ifndef _ITACTICALUI_H_
#define _ITACTICALUI_H_

#include <string>
#include <vector>

#include "ships/FVehicle.h"

namespace Frontier {

struct FTacticalCombatReportSummary;

/**
 * Pure abstract interface used by tactical game logic to request UI actions.
 */
class ITacticalUI {
public:
virtual ~ITacticalUI() {}

/** Request a tactical redraw from the UI layer. */
virtual void requestRedraw() = 0;

/** Show a simple informational message dialog. */
virtual void showMessage(const std::string& title, const std::string& body) = 0;

/** Show the tactical damage summary dialog; returns 0 on success. */
virtual int showDamageSummary(const FTacticalCombatReportSummary& summary) = 0;

/** Run ICM selection dialog; returns 0 on success. */
virtual int runICMSelection(std::vector<ICMData*>& icmData, VehicleList* defenders) = 0;

/** Notify the tactical winner (true for attacker win, false otherwise). */
virtual void notifyWinner(bool attackerWins) = 0;
};

};

#endif //_ITACTICALUI_H_
