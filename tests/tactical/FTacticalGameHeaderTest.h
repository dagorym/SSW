/**
 * @file FTacticalGameHeaderTest.h
 * @brief Header file for FTacticalGame header coverage tests
 */

#ifndef _FTACTICALGAMEHEADERTEST_H_
#define _FTACTICALGAMEHEADERTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalGameHeaderTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
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

/**
 * @brief Verifies header Declares Tactical Model State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testHeaderDeclaresTacticalModelState();
/**
 * @brief Verifies header Provides Hex And Turn Ownership Types.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testHeaderProvidesHexAndTurnOwnershipTypes();
/**
 * @brief Verifies header Compiles Without Wx Includes.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 29, 2026
 * @date Last Modified: Mar 29, 2026
 */
void testHeaderCompilesWithoutWxIncludes();
};

} // namespace FrontierTests

#endif
