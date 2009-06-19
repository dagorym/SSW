/**
 * @file SSWTests.cpp
 * @brief Main program for SSW unit tests
 * @author: Tom Stephens
 * @date Created: Jun 17, 2009
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "core/FPointTest.h"
#include "core/FObjectTest.h"
#include "weapons/FWeaponTest.h"

using namespace FrontierTests;

int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
//  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( FPointTest::suite() );
  runner.addTest( FObjectTest::suite() );
  runner.addTest( FWeaponTest::suite() );
//  runner.addTest( registry.makeTest() );
  bool wasSuccessful = runner.run( "", false );
  return wasSuccessful;
}
