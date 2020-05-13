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

	git_prompt::GitClient git;
	User                  user;
	SystemInfo            sysInfo;

	cbool_t isGitDir = git.isGitDir();

	// Current dir
    std::tstring_t currentDirPath;
    std::tstring_t currentDirPathBrief;
    {
        currentDirPath = Path(Dir::current()).homeAsBrief();

		std::csize_t leftDirsNum  {2};
		std::csize_t rightDirsNum {2};
		currentDirPathBrief = Path(currentDirPath).brief(leftDirsNum, rightDirsNum);
    }

	Console console;
	{
		console.setColorSupport(true);

		std::ctstring_t title = Format::str(xT("{} - {}, {}, CPUs: {} {}"),
			::appName, sysInfo.distro(), sysInfo.desktopName(), sysInfo.numOfCpus(), currentDirPath);
		console.setTitle(title);
	}

	std::tstring_t ps1;

	// Current date
	{
		std::ctstring_t dateTimeNow = DateTime().current().format(xT("%d-%h %H:%M"), {});

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
	   /**
		* ps1 += "$(if [[ $? == 0 ]]; then
		*     echo \"\\[\033[0;32m\\]✔\";
		* else
		*     echo \"\\[\033[0;31m\\]✖\";
		* fi)\\[\033[00m\\]";
		*/

		std::tstring_t lastShellOk;
		{
			Console::Foreground foreground = Console::Foreground::Green;
			Console::Background background = Console::Background::Default;
			cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t    &str        = xT("✔");

			lastShellOk    =
				console.setAttributes(foreground, background, attributes) +
				str +
				console.setAttributesDef();
		}

		std::tstring_t lastShellError;
		{
			Console::Foreground foreground = Console::Foreground::Red;
			Console::Background background = Console::Background::Default;
			cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t    &str        = xT("✖");

			lastShellError    =
				console.setAttributes(foreground, background, attributes) +
				str +
				console.setAttributesDef();
		}

		ps1 += Format::str(xT("$(if [[ $? == 0 ]]; then echo \"{}\"; else echo \"{}\"; fi)"),
			lastShellOk, lastShellError);
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
        ps1 += xT(" ");
	}

	// Git repositiry name
	if (isGitDir) {
		std::tstring_t gitRepoName = git.repoName();

        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Black;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = xT("[") + gitRepoName + xT("]");

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
	if (isGitDir) {
		std::ctstring_t gitBranchName = git.branchName();

        Console::Foreground foreground = Console::Foreground::Red;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = xT("[") + gitBranchName + xT("]");

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Local branches number
	if (isGitDir) {
		std::csize_t localBranchesNum = git.localBranchesNum();
		if (localBranchesNum > 0) {
			Console::Foreground foreground = Console::Foreground::White;
			Console::Background background = Console::Background::Default;
			cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t    &str        = Format::str(xT("⎇{}"), localBranchesNum);

			ps1 += console.setAttributes(foreground, background, attributes);
			ps1 += str;
			ps1 += console.setAttributesDef();
		}
	}

	// Git file statuses
	if (isGitDir) {
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = git.filesStatuses();

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Git ahead/behind commits
	if (isGitDir) {
        Console::Foreground foreground = Console::Foreground::Magenta;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        std::tstring_t str;
		{
			std::size_t aheadNum  {};
			std::size_t behindNum {};
			git.commitsAheadBehind(&aheadNum, &behindNum);

			if (aheadNum != 0) {
				str = Format::str(xT("↑{}"), aheadNum);
			}
			if (behindNum != 0) {
				str = Format::str(xT("↓{}"), behindNum);
			}
		}

        ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += str;
        ps1 += console.setAttributesDef();
	}

	// Stashes number
	if (isGitDir) {
		std::csize_t stashesNum = git.stashesNum();
		if (stashesNum > 0) {
			Console::Foreground foreground = Console::Foreground::Blue;
			Console::Background background = Console::Background::Default;
			cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t    &str        = Format::str(xT("⚑{}"), stashesNum);

			ps1 += console.setAttributes(foreground, background, attributes);
			ps1 += str;
			ps1 += console.setAttributesDef();
		}
	}

	// Is admin user
	{
        Console::Foreground foreground = Console::Foreground::Default;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);
        std::ctstring_t    &str        = user.isAdmin() ? xT("#") : xT("$");

        ps1 += xT(" ");
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
