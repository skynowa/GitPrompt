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

	// Current date
	{
		std::ctstring_t &dateTimeNow = DateTime().current().format(xT("%d-%h %H:%M"), {});

		const auto       fg    = Console::Foreground::Green;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = xT("[") + dateTimeNow + xT("]");

		ps1 += console.setAttributesText(fg, bg, attrs, str);
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
			const auto       fg    = Console::Foreground::Green;
			const auto       bg    = Console::Background::Default;
			const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t &str   = xT("✔");

			lastShellOk = console.setAttributesText(fg, bg, attrs, str);
		}

		std::tstring_t lastShellError;
		{
			const auto       fg    = Console::Foreground::Red;
			const auto       bg    = Console::Background::Default;
			const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t &str   = xT("✖");

			lastShellError = console.setAttributesText(fg, bg, attrs, str);
		}

		ps1 += Format::str(xT("$(if [[ $? == 0 ]]; then echo \"{}\"; else echo \"{}\"; fi)"),
			lastShellOk, lastShellError);
	}

	// User name
	{
		const auto       fg    = Console::Foreground::Magenta;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = user.name();

		ps1 += console.setAttributesText(fg, bg, attrs, str);
		ps1 += xT("@");
	}

	// Host name
	{
		const auto       fg    = Console::Foreground::Cyan;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = sysInfo.hostName();

		ps1 += console.setAttributesText(fg, bg, attrs, str);
		ps1 += xT(" ");
	}

	// Git repository
	if (isGitDir) {
		std::ctstring_t &gitRepoName    = git.repoName();
		std::ctstring_t &gitRepoUrlName = git.repoUrlName();

		const auto fg    = Console::Foreground::White;
		const auto bg    = Console::Background::Default;
		const auto attrs = static_cast<int_t>(Console::Attribute::Bold);

		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += console.setAttributesText(fg, bg, attrs, str);
		}

		// Git repo URL name
		{
			const auto       fg2    = Console::Foreground::Blue;
			const auto       bg2    = Console::Background::Default;
			const auto       attrs2 = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t &str    = Format::str(xT("{}:"), gitRepoUrlName);

			ps1 += console.setAttributesText(fg2, bg2, attrs2, str);
		}

		// Git repository name
		{
			std::ctstring_t &str = Format::str(xT("{}"), gitRepoName);
			ps1 += console.setAttributesText(fg, bg, attrs, str);
		}

		// ]
		{
			std::ctstring_t &str = xT("]");
			ps1 += console.setAttributesText(fg, bg, attrs, str);
		}

		if ( !gitRepoName.empty() ) {
			ps1 += xT(" ");
		}
	}

	// Current dir
	{
		const auto       fg    = Console::Foreground::Green;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = currentDirPathBrief;

		ps1 += console.setAttributesText(fg, bg, attrs, str);
	}

	// Git branch name
	if (isGitDir) {
		std::ctstring_t &gitBranchName = git.branchName();

		const auto       fg    = Console::Foreground::Red;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = xT("[") + gitBranchName + xT("]");

		ps1 += console.setAttributesText(fg, bg, attrs, str);
	}

	// Local branches number
	if (isGitDir) {
		std::csize_t localBranchesNum = git.localBranchesNum();
		if (localBranchesNum > 0) {
			const auto       fg    = Console::Foreground::White;
			const auto       bg    = Console::Background::Default;
			const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t &str   = Format::str(xT("⎇{}"), localBranchesNum);

			ps1 += console.setAttributesText(fg, bg, attrs, str);
		}
	}

	// Git file statuses
	if (isGitDir) {
		const auto       fg    = Console::Foreground::Yellow;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = git.filesStatuses();

		ps1 += console.setAttributesText(fg, bg, attrs, str);
	}

	// Git ahead/behind commits
	if (isGitDir) {
		const auto fg    = Console::Foreground::Magenta;
		const auto bg    = Console::Background::Default;
		const auto attrs = static_cast<int_t>(Console::Attribute::Bold);

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

		ps1 += console.setAttributesText(fg, bg, attrs, str);
	}

	// Stashes number
	if (isGitDir) {
		std::csize_t stashesNum = git.stashesNum();
		if (stashesNum > 0) {
			const auto       fg    = Console::Foreground::Blue;
			const auto       bg    = Console::Background::Default;
			const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
			std::ctstring_t &str   = Format::str(xT("⚑{}"), stashesNum);

			ps1 += console.setAttributesText(fg, bg, attrs, str);
		}
	}

	// Is admin user
	{
		const auto       fg    = Console::Foreground::Default;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = user.isAdmin() ? xT("#") : xT("$");

		ps1 += xT(" ");
		ps1 += console.setAttributesText(fg, bg, attrs, str);
		ps1 += xT(" ");
	}

	// "> "
	{
		const auto       fg    = Console::Foreground::Yellow;
		const auto       bg    = Console::Background::Default;
		const auto       attrs = static_cast<int_t>(Console::Attribute::Bold);
		std::ctstring_t &str   = xT("❱ ");

		ps1 += console.setAttributesText(fg, bg, attrs, str);
	}

	console.writeLine(ps1);

	return ExitCode::Success;
}
//-------------------------------------------------------------------------------------------------
xNAMESPACE_END(git_prompt)
