/**
 * @file WXGuiTestHarness.h
 * @brief Shared wx bootstrap, event pump, and modal-dismiss utility for GUI tests
 */

#ifndef _WXGUITESTHARNESS_H_
#define _WXGUITESTHARNESS_H_

#include <functional>
#include <vector>
#include <wx/defs.h>

class wxDialog;
class wxTopLevelWindow;

namespace FrontierTests {

/**
 * @brief Provides reusable wxWidgets bootstrap and modal-driving helpers for live GUI tests.
 *
 * The harness centralizes wx runtime lifecycle management, event-pump utilities, and
 * deterministic modal dialog helpers so GUI fixtures can assert runtime behavior without
 * duplicating setup and teardown scaffolding. Close-wait helpers treat shown pending-delete
 * top-level windows as still open until wx idle cleanup hides or removes them.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high)
 * @date Created: Apr 04, 2026
 * @date Last Modified: May 23, 2026
 */
class WXGuiTestHarness {
private:
	bool m_bootstrapped;
	bool m_startedWx;

public:
	/**
	 * @brief Constructs the harness with wx bootstrap state cleared.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	WXGuiTestHarness();
	/**
	 * @brief Destroys the harness and releases any wx runtime state it started.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	~WXGuiTestHarness();

	/**
	 * @brief Initializes wx runtime support for GUI tests when needed.
	 *
	 * @return True when a usable wx runtime is available; otherwise false.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	bool bootstrap();
	/**
	 * @brief Shuts down harness-owned wx runtime state and destroys residual top levels.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void shutdown();

	/**
	 * @brief Dispatches and processes pending wx events for a bounded number of iterations.
	 *
	 * @param iterations Number of pump iterations to perform.
	 *
	 * @author gpt-5.3-codex (medium), gpt-5.4 (high)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: May 23, 2026
	 */
	void pumpEvents(int iterations = 5);
	/**
	 * @brief Collects current top-level windows visible to the wx runtime.
	 *
	 * @param includeBeingDeleted When true, includes windows flagged for deletion.
	 * @return Vector of discovered top-level windows.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	std::vector<wxTopLevelWindow *> getTopLevelWindows(bool includeBeingDeleted = false) const;
	/**
	 * @brief Finds the first top-level window matching a predicate.
	 *
	 * @param predicate Callback that returns true for the desired window.
	 * @param includeBeingDeleted When true, scans windows pending deletion.
	 * @return Matching window pointer or NULL when no match is found.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	wxTopLevelWindow * findTopLevelWindow(const std::function<bool(wxTopLevelWindow *)> & predicate,
	                                      bool includeBeingDeleted = false) const;
	/**
	 * @brief Polls for a matching top-level window until timeout.
	 *
	 * @param predicate Callback that identifies the target window.
	 * @param timeoutMs Total time budget in milliseconds.
	 * @param pollMs Poll interval in milliseconds.
	 * @return Matching window pointer or NULL when timeout is reached.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	wxTopLevelWindow * waitForTopLevelWindow(const std::function<bool(wxTopLevelWindow *)> & predicate,
	                                         int timeoutMs = 250,
	                                         int pollMs = 5);
	/**
	 * @brief Waits until a matching top-level window is hidden or fully removed.
	 *
	 * @param predicate Callback that identifies the window to observe.
	 * @param timeoutMs Total wait budget in milliseconds.
	 * @param pollMs Poll interval in milliseconds.
	 * @param includeBeingDeleted When true, keeps observing pending-delete windows until they are
	 *        hidden or absent instead of treating `IsBeingDeleted()` alone as success.
	 * @return True when the target window is absent or no longer shown before timeout.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 22, 2026
	 * @date Last Modified: May 23, 2026
	 */
	bool waitForTopLevelWindowClosed(const std::function<bool(wxTopLevelWindow *)> & predicate,
	                                 int timeoutMs = 250,
	                                 int pollMs = 5,
	                                 bool includeBeingDeleted = true);
	/**
	 * @brief Returns the first currently modal dialog.
	 *
	 * @return Pointer to a modal dialog or NULL when none is open.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	wxDialog * findModalDialog() const;
	/**
	 * @brief Waits for a modal dialog to appear within a timeout window.
	 *
	 * @param timeoutMs Total wait budget in milliseconds.
	 * @param pollMs Poll interval in milliseconds.
	 * @return Pointer to the discovered modal dialog or NULL on timeout.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	wxDialog * waitForModalDialog(int timeoutMs = 250, int pollMs = 5);
	/**
	 * @brief Requests close for shown top-level windows and reports how many remain visible.
	 *
	 * Modal dialogs are unwound before close requests, and shown pending-delete top levels are
	 * hidden so teardown waits for wx idle cleanup instead of leaving visible orphan windows.
	 *
	 * @param pumpIterations Number of event-pump iterations after close requests.
	 * @return Count of shown top-level windows remaining after cleanup.
	 *
	 * @author gpt-5.3-codex (medium), gpt-5.4 (high)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: May 23, 2026
	 */
	int cleanupOrphanTopLevels(int pumpIterations = 10);
	/**
	 * @brief Shows a dialog and auto-dismisses it with a timer-driven return code.
	 *
	 * @param dialog Modal dialog to show.
	 * @param returnCode Code used when the auto-dismiss timer closes the dialog.
	 * @param timeoutMs Delay before auto-dismiss in milliseconds.
	 * @return Dialog modal result code.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	int showModalWithAutoDismiss(wxDialog & dialog, int returnCode, int timeoutMs = 25);
	/**
	 * @brief Shows a dialog, runs an action callback, and auto-dismisses if needed.
	 *
	 * @param dialog Modal dialog to show.
	 * @param action Callback scheduled during modal execution.
	 * @param fallbackCode Return code used by the fallback auto-dismiss path.
	 * @param timeoutMs Delay before fallback auto-dismiss in milliseconds.
	 * @return Dialog modal result code.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	int showModalWithAction(wxDialog & dialog,
	                        const std::function<void()> & action,
	                        int fallbackCode = wxID_CANCEL,
	                        int timeoutMs = 100);
	/**
	 * @brief Runs a modal-producing callback and auto-dismisses any active modal dialog.
	 *
	 * @param callback Function that launches and returns a modal result.
	 * @param returnCode Return code used by auto-dismiss when needed.
	 * @param timeoutMs Polling timer delay in milliseconds.
	 * @return Result returned by the callback.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	int runModalFunctionWithAutoDismiss(const std::function<int()> & callback,
	                                    int returnCode,
	                                    int timeoutMs = 25);
	/**
	 * @brief Runs a modal-producing callback with an injected action and fallback dismissal.
	 *
	 * @param callback Function that launches and returns a modal result.
	 * @param action Callback scheduled while the modal is running.
	 * @param fallbackCode Return code used by fallback dismissal.
	 * @param timeoutMs Polling timer delay in milliseconds.
	 * @return Result returned by the callback.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	int runModalFunctionWithAction(const std::function<int()> & callback,
	                               const std::function<void()> & action,
	                               int fallbackCode = wxID_CANCEL,
	                               int timeoutMs = 100);
	/**
	 * @brief Runs a void callback while auto-dismissing any modal dialog it launches.
	 *
	 * @param callback Function that may launch modal UI.
	 * @param returnCode Return code used by auto-dismiss behavior.
	 * @param timeoutMs Polling timer delay in milliseconds.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void runVoidFunctionWithAutoDismiss(const std::function<void()> & callback,
	                                    int returnCode,
	                                    int timeoutMs = 25);
	/**
	 * @brief Runs a void callback with an injected action and fallback modal dismissal.
	 *
	 * @param callback Function that may launch modal UI.
	 * @param action Callback scheduled while UI is active.
	 * @param fallbackCode Return code used by fallback dismissal.
	 * @param timeoutMs Polling timer delay in milliseconds.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 04, 2026
	 * @date Last Modified: Apr 04, 2026
	 */
	void runVoidFunctionWithAction(const std::function<void()> & callback,
	                               const std::function<void()> & action,
	                               int fallbackCode = wxID_CANCEL,
	                               int timeoutMs = 100);
};

}

#endif
