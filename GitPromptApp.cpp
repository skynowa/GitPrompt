/**
 * \file  Main.cpp
 * \brief GitPrompt
 */


#include "GitPromptApp.h"

#include "GitClient.h"

xNAMESPACE_BEGIN(git_prompt)
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

		std::ctstring_t &title = Format::str(xT("{} - {}, {}, CPUs: {},                Build: {}"),
			::appName, sysInfo.distro(), sysInfo.desktopName(), sysInfo.numOfCpus(),
			BuildInfo().datetime());
		console.setTitle(title);
	}

	std::tstring_t ps1;

	constexpr auto fgGreen   = Console::Foreground::Green;
	constexpr auto fgYellow  = Console::Foreground::Yellow;
	constexpr auto fgBlue    = Console::Foreground::Blue;
	constexpr auto fgMagenta = Console::Foreground::Magenta;
	constexpr auto fgRed     = Console::Foreground::Red;
	constexpr auto fgWhite   = Console::Foreground::White;
	constexpr auto fgCyan    = Console::Foreground::Cyan;
	constexpr auto fgDefault = Console::Foreground::Default;

	constexpr auto bgDefault = Console::Background::Default;

	constexpr auto attrBold  = static_cast<int_t>(Console::Attribute::Bold);

	// Current date
	{
		std::ctstring_t &dateTimeNow = DateTime().current().format(xT("%d-%h %H:%M"), {});

		std::ctstring_t &str = xT("[") + dateTimeNow + xT("]");

		ps1 += console.setAttributesText(fgGreen, bgDefault, attrBold, str);
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
			std::ctstring_t &str = xT("✔");

			lastShellOk = console.setAttributesText(fgGreen, bgDefault, attrBold, str);
		}

		std::tstring_t lastShellError;
		{
			std::ctstring_t &str = xT("✖");

			lastShellError = console.setAttributesText(fgRed, bgDefault, attrBold, str);
		}

		ps1 += Format::str(xT("$(if [[ $? == 0 ]]; then echo \"{}\"; else echo \"{}\"; fi)"),
			lastShellOk, lastShellError);
	}

	// User name
	{
		std::ctstring_t &str = user.name();

		ps1 += console.setAttributesText(fgMagenta, bgDefault, attrBold, str);
		ps1 += xT("@");
	}

	// Host name
	{
		std::ctstring_t &str = sysInfo.hostName();

		ps1 += console.setAttributesText(fgCyan, bgDefault, attrBold, str);
		ps1 += xT(" ");
	}

	// Git repository
	if (isGitDir) {
		std::ctstring_t &gitRepoName    = git.repoName();
		std::ctstring_t &gitRepoUrlName = git.repoUrlName();

		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}

		// Git repo URL name
		{
			std::ctstring_t &str = Format::str(xT("{}:"), gitRepoUrlName);

			ps1 += console.setAttributesText(fgBlue, bgDefault, attrBold, str);
		}

		// Git repository name
		{
			std::ctstring_t &str = Format::str(xT("{}"), gitRepoName);
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}

		// ]
		{
			std::ctstring_t &str = xT("]");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}

		if ( !gitRepoName.empty() ) {
			ps1 += xT(" ");
		}
	}

	// Current dir
	{
		std::ctstring_t &str = currentDirPathBrief;

		ps1 += console.setAttributesText(fgGreen, bgDefault, attrBold, str);
	}

	// Git branch name
	if (isGitDir) {
		std::ctstring_t &gitBranchName = git.branchName();

		std::ctstring_t &str = xT("[") + gitBranchName + xT("]");

		ps1 += console.setAttributesText(fgRed, bgDefault, attrBold, str);
	}

	// Local branches number
	if (isGitDir) {
		std::csize_t localBranchesNum = git.localBranchesNum();
		if (localBranchesNum > 0) {
			std::ctstring_t &str = Format::str(xT("⎇{}"), localBranchesNum);

			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}
	}

	// Git file statuses
	if (isGitDir) {
		std::ctstring_t &str = git.filesStatuses();

		ps1 += console.setAttributesText(fgYellow, bgDefault, attrBold, str);
	}

	// Git ahead/behind commits
	if (isGitDir) {
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

		ps1 += console.setAttributesText(fgMagenta, bgDefault, attrBold, str);
	}

	// Stashes number
	if (isGitDir) {
		std::csize_t stashesNum = git.stashesNum();
		if (stashesNum > 0) {

			std::ctstring_t &str = Format::str(xT("⚑{}"), stashesNum);

			ps1 += console.setAttributesText(fgBlue, bgDefault, attrBold, str);
		}
	}

	// Is admin user
	{
		std::ctstring_t &str = user.isAdmin() ? xT("#") : xT("$");

		ps1 += xT(" ");
		ps1 += console.setAttributesText(fgDefault, bgDefault, attrBold, str);
		ps1 += xT(" ");
	}

	// "> "
	{
		std::ctstring_t &str = xT("❱ ");

		ps1 += console.setAttributesText(fgYellow, bgDefault, attrBold, str);
	}

	console.writeLine(ps1);

	return ExitCode::Success;
}
//-------------------------------------------------------------------------------------------------
xNAMESPACE_END(git_prompt)
