/**
 * \file  Main.cpp
 * \brief GitPrompt
 */


#include "GitPromptApp.h"

//-------------------------------------------------------------------------------------------------
int_t main(int_t a_argNum, tchar_t *a_args[])
{
	xUNUSED(a_argNum);
	xUNUSED(a_args);

	auto exitStatus {Application::ExitCode::Failure};

	try {
		git_prompt::GitPromptApp app(::appName, xT(""));
		exitStatus = app.run();
	}
	catch (const Exception &a_e) {
		Cout() << xTRACE_VAR_2(exitStatus, a_e.what());
	}
	catch (const std::exception &a_e) {
		Cout() << xTRACE_VAR_2(exitStatus, a_e.what());
	}
	catch (...) {
		Cout() << xTRACE_VAR_2(exitStatus, xT("Unknown error"));
	}

	return exitStatus;
}
//-------------------------------------------------------------------------------------------------
