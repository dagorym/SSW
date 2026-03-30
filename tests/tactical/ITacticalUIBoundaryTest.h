/**
 * @file ITacticalUIBoundaryTest.h
 * @brief Header file for ITacticalUI boundary acceptance tests
 */

#ifndef ITACTICALUIBOUNDARYTEST_H_
#define ITACTICALUIBOUNDARYTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class ITacticalUIBoundaryTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( ITacticalUIBoundaryTest );
CPPUNIT_TEST( testHeaderDeclaresFrontierITacticalUI );
CPPUNIT_TEST( testInterfaceDeclaresExactlyRequiredCallbacks );
CPPUNIT_TEST( testHeaderHasNoWxIncludesOrTypes );
CPPUNIT_TEST( testBoundaryCompilesForTacticalModelUse );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
int countToken(const std::string & source, const std::string & token);

public:
void setUp();
void tearDown();

void testHeaderDeclaresFrontierITacticalUI();
void testInterfaceDeclaresExactlyRequiredCallbacks();
void testHeaderHasNoWxIncludesOrTypes();
void testBoundaryCompilesForTacticalModelUse();
};

} // namespace FrontierTests

#endif
