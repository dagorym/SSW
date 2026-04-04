/**
 * @file WXGuiTestHarness.h
 * @brief Shared wx bootstrap, event pump, and modal-dismiss utility for GUI tests
 */

#ifndef _WXGUITESTHARNESS_H_
#define _WXGUITESTHARNESS_H_

#include <functional>

class wxDialog;

namespace FrontierTests {

class WXGuiTestHarness {
private:
	bool m_bootstrapped;
	bool m_startedWx;

public:
	WXGuiTestHarness();
	~WXGuiTestHarness();

	bool bootstrap();
	void shutdown();

	void pumpEvents(int iterations = 5);
	int showModalWithAutoDismiss(wxDialog & dialog, int returnCode, int timeoutMs = 25);
	int runModalFunctionWithAutoDismiss(const std::function<int()> & callback,
	                                    int returnCode,
	                                    int timeoutMs = 25);
	void runVoidFunctionWithAutoDismiss(const std::function<void()> & callback,
	                                    int returnCode,
	                                    int timeoutMs = 25);
};

}

#endif
