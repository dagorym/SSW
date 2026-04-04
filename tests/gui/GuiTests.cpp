/**
 * @file GuiTests.cpp
 * @brief Main program for GUI unit tests
 */

#include <cppunit/ui/text/TestRunner.h>

#include "GuiHarnessTest.h"

using namespace FrontierTests;

int main(int argc, char **argv) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(GuiHarnessTest::suite());
	const bool wasSuccessful = runner.run("", false);
	return wasSuccessful ? 0 : 1;
}
