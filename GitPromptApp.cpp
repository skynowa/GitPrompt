/**
 * \file  Main.cpp
 * \brief GitPrompt
 */


#include "GitPromptApp.h"

#include <xLib/Package/GitClient.h>

//-------------------------------------------------------------------------------------------------
namespace
{

// Options
constexpr ulonglong_t volumeUsedWarnPct {90};
constexpr std::size_t leftDirsNum       {2};
constexpr std::size_t rightDirsNum      {2};

}
//-------------------------------------------------------------------------------------------------

namespace git_prompt
{
//-------------------------------------------------------------------------------------------------
GitPromptApp::GitPromptApp(
	std::ctstring_t &a_appGuid,
	std::ctstring_t &a_locale
) :
	Application(a_appGuid, a_locale)
{
	std::vec_tstring_t appArgs;
	args(true, &appArgs);
	xUNUSED(appArgs);

	// Cout() << xTRACE_VAR(appArgs);

	CmdOptions options;

	std::vector<CmdOptionsUsage> usage;
	options.parse(appArgs, usage);
}
//-------------------------------------------------------------------------------------------------
GitPromptApp::ExitCode
GitPromptApp::onRun() /* override */
{
	GitClient  git;
	User       user;
	SystemInfo sysInfo;

	std::ctstring_t &hostName = sysInfo.hostName();
	cbool_t          isAdmin  = user.isAdmin();
	cbool_t          isGitDir = git.isGitDir();
	cbool_t          isMc     = Environment(xT("MC_SID")).isExists();
		///< Check if MC is runnin (check env: MC_SID=30463)

	// Current dir
	std::tstring_t currentDirPathBrief;
	{
		std::ctstring_t dirCurrent  = Dir::current().str();
		std::ctstring_t homeAsBrief = Path(dirCurrent).homeAsBrief().str();

		currentDirPathBrief = Path(homeAsBrief).brief(::leftDirsNum, ::rightDirsNum).str();
	}

	ulonglong_t volumeUsedPct {};
	{
        ulonglong_t total     {};
        ulonglong_t available {};

        Volume::currentSpace(&total, &available, nullptr);

        volumeUsedPct = (total - available) * 100 / total;
	}

	Console console;
	{
		console.setColorSupport(true);
		console.setEscapeValues(true);

		std::ctstring_t &title = Format::str(xT("{} - {}, {}, CPUs: {},                Build: {}"),
			::appName, sysInfo.distro(), sysInfo.desktopName(), sysInfo.cpusNum(),
			BuildInfo().datetime());
		console.setTitle(title);
	}

	std::tstring_t ps1;

	// Foreground
	constexpr auto fgGreen   = Console::Foreground::Green;
	constexpr auto fgYellow  = Console::Foreground::Yellow;
	constexpr auto fgBlue    = Console::Foreground::Blue;
	constexpr auto fgMagenta = Console::Foreground::Magenta;
	constexpr auto fgRed     = Console::Foreground::Red;
	constexpr auto fgWhite   = Console::Foreground::White;
	constexpr auto fgCyan    = Console::Foreground::Cyan;
	constexpr auto fgDefault = Console::Foreground::Default;

	// Background
	constexpr auto bgDefault = Console::Background::Default;

	// Attribute
	constexpr auto attrBold  = static_cast<int_t>(Console::Attribute::Bold);

	// Current date
	{
		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}

		// Current date
		{
			std::ctstring_t &str = DateTime().current().format(xT("%d-%h %H:%M"), {});
			ps1 += console.setAttributesText(fgGreen, bgDefault, attrBold, str);
		}

		// ]
		{
			std::ctstring_t &str = xT("]");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}
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
		std::ctstring_t &str = user.loginName();
		const auto       fg  = isAdmin ? fgRed : fgMagenta;

		ps1 += console.setAttributesText(fg, bgDefault, attrBold, str);
	}

	// Host name
	if (_config.isHostName ||
		hostName != "skynowa-pc")
	{
		std::ctstring_t &str = hostName;
		const auto       fg  = fgCyan;

		ps1 += xT("@");
		ps1 += console.setAttributesText(fg, bgDefault, attrBold, str);
	}

	ps1 += xT(" ");

	// Git repository
	if (isGitDir) {
		std::ctstring_t &gitRepoName = git.repoName();

		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}

		// Git repo URL name
		{
			std::ctstring_t &str = Format::str(xT("{}:"), git.repoUrlName());
			ps1 += console.setAttributesText(fgBlue, bgDefault, attrBold, str);
		}

		// Git repository name
		{
			std::ctstring_t &str = Format::str(xT("{}"), gitRepoName);
			ps1 += console.setAttributesText(fgYellow, bgDefault, attrBold, str);
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

	// Volume used %
	if (_config.isVolumeUsedPct ||
		volumeUsedPct > ::volumeUsedWarnPct) {
		ps1 += xT(" ");

		std::ctstring_t &str = Format::str(xT("{}%"), volumeUsedPct);
		ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);

		ps1 += xT(" ");
	}

	// Git branch name
	if (isGitDir) {
		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}

		// Git branch name
		{
			std::ctstring_t &str = git.branchName();
			ps1 += console.setAttributesText(fgRed, bgDefault, attrBold, str);
		}

		// ]
		{
			std::ctstring_t &str = xT("]");
			ps1 += console.setAttributesText(fgWhite, bgDefault, attrBold, str);
		}
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
		std::ctstring_t &str = isAdmin ? xT("#") : xT("$");
		const auto       fg  = isAdmin ? fgRed : fgDefault;

		ps1 += xT(" ");
		ps1 += console.setAttributesText(fg, bgDefault, attrBold, str);
		ps1 += xT(" ");
	}

	// mc
	if (isMc) {
		std::ctstring_t str = xT("mc");
		const auto      fg  = fgCyan;

		ps1 += console.setAttributesText(fg, bgDefault, attrBold, str);
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

} // namespace

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
