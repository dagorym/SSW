/**
 * @file FTacticalModelSelectionHexClickSurfaceTest.h
 * @brief Header file for FTacticalGame selection and hex-click API surface tests
 */

#ifndef FTacticalModelSelectionHexClickSurfaceTest_H_
#define FTacticalModelSelectionHexClickSurfaceTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalModelSelectionHexClickSurfaceTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalModelSelectionHexClickSurfaceTest );
CPPUNIT_TEST( testHeaderDeclaresMilestone8SelectionAndHexClickApi );
CPPUNIT_TEST( testImplementationDefinesSelectionAndHexClickHandlers );
CPPUNIT_TEST( testHexClickRoutesThroughModelOwnedSelectionAndTargetingFlow );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

void testHeaderDeclaresMilestone8SelectionAndHexClickApi();
void testImplementationDefinesSelectionAndHexClickHandlers();
void testHexClickRoutesThroughModelOwnedSelectionAndTargetingFlow();
};

}

#endif
