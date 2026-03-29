/**
 * @file FTacticalGameHeaderTest.h
 * @brief Header file for FTacticalGame header coverage tests
 */

#ifndef _FTACTICALGAMEHEADERTEST_H_
#define _FTACTICALGAMEHEADERTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalGameHeaderTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalGameHeaderTest );
CPPUNIT_TEST( testHeaderDeclaresTacticalModelState );
CPPUNIT_TEST( testHeaderProvidesHexAndTurnOwnershipTypes );
CPPUNIT_TEST( testHeaderCompilesWithoutWxIncludes );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);

public:
void setUp();
void tearDown();

void testHeaderDeclaresTacticalModelState();
void testHeaderProvidesHexAndTurnOwnershipTypes();
void testHeaderCompilesWithoutWxIncludes();
};

} // namespace FrontierTests

#endif
