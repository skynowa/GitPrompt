/**
 * \file  Main.cpp
 * \brief GitPrompt
 */


#include "Main.h"

//-------------------------------------------------------------------------------------------------
GitPromptApp::GitPromptApp(
	std::ctstring_t &a_appGuid,
	std::ctstring_t &a_locale
) :
	Application(a_appGuid, a_locale)
{
}
//-------------------------------------------------------------------------------------------------
GitPromptApp::ExitCode
GitPromptApp::onRun() /* override */
{
	std::vec_tstring_t appArgs;
	args(true, &appArgs);
	xUNUSED(appArgs);

	std::ctstring_t       dateTimeNow      = DateTime().current().format(xT("%d-%h %H:%M"), {});
	cbool_t               isLastShellError = _isShellLastError();
	git_prompt::GitClient git;
	User                  user;
	SystemInfo            sysInfo;

	// Format values
	std::tstring_t gitRepoName = git.repoName();

    std::tstring_t currentDirPath;
    std::tstring_t currentDirPathBrief;
    {
        currentDirPath = Path(Dir::current()).homeAsBrief();

		std::csize_t leftDirsNum  {2};
		std::csize_t rightDirsNum {2};
		currentDirPathBrief = Path(currentDirPath).brief(leftDirsNum, rightDirsNum);
    }

	std::ctstring_t gitBranchName    = git.branchName();
	std::csize_t    localBranchesNum = git.localBranchesNum();

	Console console;
	{
		console.setColorSupport(true);

		std::ctstring_t title = Format::str(xT("{}, {}, CPUs: {} {}"),
			sysInfo.distro(), sysInfo.desktopName(), sysInfo.numOfCpus(), currentDirPath);
		console.setTitle(title);
	}

	std::tstring_t ps1;

	// Current date
	{
        Console::Foreground foreground = Console::Foreground::Green;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = xT("[") + dateTimeNow + xT("]");

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Shell last error
	{
        Console::Foreground foreground = Console::Foreground::Red;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = isLastShellError ? xT("✖") : xT("✔");

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// User name
	{
        Console::Foreground foreground = Console::Foreground::Magenta;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = user.name();

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
        ps1 += xT("@");
	}

	// Host name
	{
        Console::Foreground foreground = Console::Foreground::Cyan;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = sysInfo.hostName();

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
        ps1 += xT(": ");
	}

	// Git repositiry name
	{
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Black;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = gitRepoName.empty() ? xT("") : xT("[") + gitRepoName + xT("]");

        ps1 += console.setAttributes(foreground, background, attributes);
		ps1 += str;
        ps1 += console.setAttributesDef();

        if ( !gitRepoName.empty() ) {
            ps1 += xT(" ");
        }
	}

	// Current dir
	{
        Console::Foreground foreground = Console::Foreground::Green;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = currentDirPathBrief;

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Git branch name
	if ( !gitBranchName.empty() ) {
        Console::Foreground foreground = Console::Foreground::Red;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = xT("[") + gitBranchName + xT("]");

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Local branches number
	{
        Console::Foreground foreground = Console::Foreground::Blue;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        =
            (localBranchesNum == 0) ? xT("") : std::to_string(git.localBranchesNum());

        ps1 += console.setAttributes(foreground, background, attributes);
		ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Git file statuses
	{
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = git.filesStatuses();

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Git ahead/behind commits
	{
        Console::Foreground foreground = Console::Foreground::Magenta;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = git.commitsAheadBehind();

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
        ps1 += xT(" ");
	}

	// Is admin user
	{
        Console::Foreground foreground = Console::Foreground::Default;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = user.isAdmin() ? xT("#") : xT("$");

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
        ps1 += xT(" ");
	}

	// "> "
	{
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = xT("❱ ");

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	console.writeLine(ps1);

	return ExitCode::Success;
}
//-------------------------------------------------------------------------------------------------
bool_t
GitPromptApp::_isShellLastError() const
{
#if 0
	std::ctstring_t                     filePath {"/bin/echo"};
	std::cvec_tstring_t                 params   {"$?"};
	const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	std::ctstring_t errorCode = ::String::trimSpace(stdOut);
	Cout() << "errorCode: >>>" << errorCode << "<<<";
	Cout() << xTRACE_VAR_4(errorCode.size(), errorCode[0], errorCode[1], std::tstring_t{"0"}.size());

	cbool_t bRv = false; // (errorCode == std::tstring_t{"0"});
	Cout() << xTRACE_VAR(bRv);

	return bRv;
#else
	return false;
#endif
}
//-------------------------------------------------------------------------------------------------
int_t main(int_t /* a_argNum */, tchar_t ** /* a_args */)
{
	GitPromptApp::ExitCode exitStatus {};

	try {
		GitPromptApp app(::appName, xT(""));
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
