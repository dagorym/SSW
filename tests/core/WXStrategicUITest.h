/**
 * @file WXStrategicUITest.h
 * @brief Declares the WXStrategicUI guard-path regression fixture.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Apr 17, 2026
 */

#ifndef WXStrategicUITest_H_
#define WXStrategicUITest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief Validates WXStrategicUI guard behavior when no usable wx runtime context exists.
 *
 * This fixture verifies cancel/early-return behavior for modal strategic UI entry points and
 * confirms redraw requests remain null-safe in non-runtime contexts.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Apr 17, 2026
 */
class WXStrategicUITest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( WXStrategicUITest );
CPPUNIT_TEST( testGuardedDialogMethodsReturnNonZeroWhenNoUIContext );
CPPUNIT_TEST( testGuardedDialogMethodsReturnCancelForValidInputsWithoutWxRuntime );
CPPUNIT_TEST( testGuardedDialogMethodsIgnoreParentWhenNoWxRuntime );
CPPUNIT_TEST( testRequestRedrawIsNullSafe );
CPPUNIT_TEST_SUITE_END();

public:
/**
 * @brief Prepares per-test fixture state.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Mar 28, 2026
 */
void setUp();
/**
 * @brief Releases per-test fixture state.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Mar 28, 2026
 */
void tearDown();

/**
 * @brief Verifies dialog entry points return non-zero when called without usable UI context.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Mar 28, 2026
 */
void testGuardedDialogMethodsReturnNonZeroWhenNoUIContext();
/**
 * @brief Verifies no-runtime guard paths return wxID_CANCEL for valid inputs.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Apr 17, 2026
 * @date Last Modified: Apr 17, 2026
 */
void testGuardedDialogMethodsReturnCancelForValidInputsWithoutWxRuntime();
/**
 * @brief Verifies no-runtime guard behavior is preserved even when a parent pointer is provided.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Apr 17, 2026
 * @date Last Modified: Apr 17, 2026
 */
void testGuardedDialogMethodsIgnoreParentWhenNoWxRuntime();
/**
 * @brief Ensures requestRedraw() remains safe when no parent wx window exists.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Mar 28, 2026
 */
void testRequestRedrawIsNullSafe();
};

}

#endif
