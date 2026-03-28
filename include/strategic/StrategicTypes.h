/**
 * @file StrategicTypes.h
 * @brief Lightweight forward declarations and shared strategic pointer-list aliases.
 *
 * This header exists to provide strategic-layer type aliases without pulling in
 * full model headers or GUI dependencies.
 */

#ifndef STRATEGICTYPES_H
#define STRATEGICTYPES_H

#include <vector>

namespace Frontier {

class FPlayer;
class FFleet;
class FMap;
class FSystem;

/** Shared list type for fleet pointers across strategic interfaces. */
typedef std::vector<FFleet *> FleetList;

/** Shared list type for player pointers across strategic interfaces. */
typedef std::vector<FPlayer *> PlayerList;

}  // namespace Frontier

#endif  // STRATEGICTYPES_H
