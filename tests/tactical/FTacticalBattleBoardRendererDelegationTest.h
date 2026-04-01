/**
 * @file FTacticalBattleBoardRendererDelegationTest.h
 * @brief Header file for FBattleBoard renderer delegation tests
 */

#ifndef FTacticalBattleBoardRendererDelegationTest_H_
#define FTacticalBattleBoardRendererDelegationTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalBattleBoardRendererDelegationTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleBoardRendererDelegationTest );
CPPUNIT_TEST( testDrawUsesBattleScreenModelAccessors );
CPPUNIT_TEST( testOverlayDrawingUsesModelStateAccessors );
CPPUNIT_TEST( testOnLeftUpOnlyHitTestsAndForwardsHexClick );
CPPUNIT_TEST( testHeaderRemovesDuplicatedMechanicsState );
CPPUNIT_TEST( testOnMotionUsesPlacementForwardersOnly );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

void testDrawUsesBattleScreenModelAccessors();
void testOverlayDrawingUsesModelStateAccessors();
void testOnLeftUpOnlyHitTestsAndForwardsHexClick();
void testHeaderRemovesDuplicatedMechanicsState();
void testOnMotionUsesPlacementForwardersOnly();
};

}

#endif
