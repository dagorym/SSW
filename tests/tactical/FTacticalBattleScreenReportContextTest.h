/**
 * @file FTacticalBattleScreenReportContextTest.h
 * @brief Header file for the FTacticalBattleScreenReportContextTest class
 */

#ifndef FTacticalBattleScreenReportContextTest_H_
#define FTacticalBattleScreenReportContextTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

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

void testAppendTacticalAttackReportDelegatesToTacticalModel();
void testAppendTacticalReportEventDelegatesToTacticalModel();
};

}

#endif
