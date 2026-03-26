/**
 * @file FTacticalStationOrbitalMovementTest.h
 * @brief Header file for FBattleBoard station orbital movement tests
 * @author Tom Stephens
 * @date Created: Mar 26, 2026
 */

#ifndef _FTACTICALSTATIONORBITALMOVEMENTTEST_H_
#define _FTACTICALSTATIONORBITALMOVEMENTTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalStationOrbitalMovementTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalStationOrbitalMovementTest );
CPPUNIT_TEST( testStationHeadingUpdatedAfterOrbit );
CPPUNIT_TEST( testStationSpeedPreservedAfterOrbit );
CPPUNIT_TEST( testNonStationHeadingUnchangedByFix );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);

public:
void setUp();
void tearDown();

void testStationHeadingUpdatedAfterOrbit();
void testStationSpeedPreservedAfterOrbit();
void testNonStationHeadingUnchangedByFix();
};

} // namespace FrontierTests

#endif
