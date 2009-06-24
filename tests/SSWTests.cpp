/**
 * @file SSWTests.cpp
 * @brief Main program for SSW unit tests
 * @author: Tom Stephens
 * @date Created: Jun 17, 2009
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "core/FPointTest.h"
#include "core/FObjectTest.h"
#include "weapons/FWeaponTest.h"
#include "weapons/FAssaultRocketTest.h"
#include "weapons/FDisruptorCannonTest.h"
#include "weapons/FElectronBatteryTest.h"
#include "weapons/FLaserBatteryTest.h"
#include "weapons/FLaserCannonTest.h"
#include "weapons/FMineLauncherTest.h"
#include "weapons/FProtonBatteryTest.h"
#include "weapons/FRocketBatteryTest.h"
#include "weapons/FSeekerMissileLauncherTest.h"
#include "weapons/FTorpedoTest.h"
#include "ships/FVehicleTest.h"
#include "ships/FAssaultScoutTest.h"
#include "ships/FArmedStationTest.h"
#include "ships/FAssaultCarrierTest.h"
#include "ships/FBattleshipTest.h"
#include "ships/FDestroyerTest.h"
#include "ships/FFighterTest.h"
#include "ships/FFortifiedStationTest.h"
#include "ships/FFortressTest.h"
#include "ships/FFrigateTest.h"
#include "ships/FHvCruiserTest.h"
#include "ships/FLtCruiserTest.h"
#include "ships/FMinelayerTest.h"
#include "strategic/FGameTest.h"
//#include "FMainFrameTest.h"

using namespace FrontierTests;

int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
//  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( FPointTest::suite() );
  runner.addTest( FObjectTest::suite() );
  runner.addTest( FWeaponTest::suite() );
  runner.addTest( FAssaultRocketTest::suite() );
  runner.addTest( FDisruptorCannonTest::suite() );
  runner.addTest( FElectronBatteryTest::suite() );
  runner.addTest( FLaserBatteryTest::suite() );
  runner.addTest( FLaserCannonTest::suite() );
  runner.addTest( FMineLauncherTest::suite() );
  runner.addTest( FProtonBatteryTest::suite() );
  runner.addTest( FRocketBatteryTest::suite() );
  runner.addTest( FSeekerMissileLauncherTest::suite() );
  runner.addTest( FTorpedoTest::suite() );
  runner.addTest( FVehicleTest::suite() );
  runner.addTest( FAssaultScoutTest::suite() );
  runner.addTest( FArmedStationTest::suite() );
  runner.addTest( FAssaultCarrierTest::suite() );
  runner.addTest( FBattleshipTest::suite() );
  runner.addTest( FDestroyerTest::suite() );
  runner.addTest( FFighterTest::suite() );
  runner.addTest( FFortifiedStationTest::suite() );
  runner.addTest( FFortressTest::suite() );
  runner.addTest( FFrigateTest::suite() );
  runner.addTest( FHvCruiserTest::suite() );
  runner.addTest( FLtCruiserTest::suite() );
  runner.addTest( FMinelayerTest::suite() );
  runner.addTest( FGameTest::suite() );
//  runner.addTest( FMainFrameTest::suite() );
//  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run( "", false );
  return wasSuccessful;
}
