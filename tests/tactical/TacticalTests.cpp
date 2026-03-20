/**
 * @file TacticalTests.cpp
 * @brief Main program for tactical unit tests
 */

#include <cppunit/ui/text/TestRunner.h>

#include "FTacticalCombatReportTest.h"
#include "FTacticalAttackResultTest.h"

using namespace FrontierTests;

int main(int argc, char **argv) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(FTacticalCombatReportTest::suite());
	runner.addTest(FTacticalAttackResultTest::suite());
	bool wasSuccessful = runner.run("", false);
	return wasSuccessful ? 0 : 1;
}
