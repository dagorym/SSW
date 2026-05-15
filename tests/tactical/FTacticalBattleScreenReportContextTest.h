/**
 * @file FTacticalBattleScreenReportContextTest.h
 * @brief Header file for the FTacticalBattleScreenReportContextTest class
 */

#ifndef FTacticalBattleScreenReportContextTest_H_
#define FTacticalBattleScreenReportContextTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalBattleScreenReportContextTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Mar 30, 2026
 */
class FTacticalBattleScreenReportContextTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleScreenReportContextTest );
CPPUNIT_TEST( testAppendTacticalAttackReportDelegatesToTacticalModel );
CPPUNIT_TEST( testAppendTacticalReportEventDelegatesToTacticalModel );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);

public:
void setUp();
void tearDown();

/**
 * @brief Verifies append Tactical Attack Report Delegates To Tactical Model.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 30, 2026
 * @date Last Modified: Mar 30, 2026
 */
void testAppendTacticalAttackReportDelegatesToTacticalModel();
/**
 * @brief Verifies append Tactical Report Event Delegates To Tactical Model.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 30, 2026
 * @date Last Modified: Mar 30, 2026
 */
void testAppendTacticalReportEventDelegatesToTacticalModel();
};

}

#endif
