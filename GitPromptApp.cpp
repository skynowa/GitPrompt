/**
 * \file  GitPromptApp.cpp
 * \brief Git prompt tools
 */


#include "GitPromptApp.h"

#include <StdTest/StdTest.h>
#include <StdStream/StdStream.h>
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

class Ps1Color final :
	public Color
{
public:
	Ps1Color(cFG fg, cBG bg, cAttr attrs) :
		Color(true, false, fg, bg, attrs)
	{
	}
};
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
GitPromptApp::onRun() /* final */
{
	GitClient  git;
	User       user;

	std::ctstring_t &hostName  = info::Host().name();
	cbool_t          isAdmin   = user.isAdmin();
	std::cstring_t  &loginName = user.loginName();
	cbool_t          isGitDir  = git.isGitDir();
	cbool_t          isMc      = Env(xT("MC_"), xT("SID")).isExists();
		///< Check if MC is running (check env: MC_SID=30463)

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

	std::tstring_t ps1;

	// Background
	constexpr auto bgDefault = Color::BG::Default;

	// Attribute
	constexpr auto attrBold  = Color::Attr::Bold;

	const Ps1Color clGreenBold(Color::FG::Green, bgDefault, attrBold);
	const Ps1Color clYellowBold(Color::FG::Yellow, bgDefault, attrBold);
	const Ps1Color clBlueBold(Color::FG::Blue, bgDefault, attrBold);
	const Ps1Color clMagentaBold(Color::FG::Magenta, bgDefault, attrBold);
	const Ps1Color clRedBold(Color::FG::Red, bgDefault, attrBold);
	const Ps1Color clWhiteBold(Color::FG::White, bgDefault, attrBold);
	const Ps1Color clCyanBold(Color::FG::Cyan, bgDefault, attrBold);
	const Ps1Color clDefaultBold(Color::FG::Default, bgDefault, attrBold);

	// Current date
	{
		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += clWhiteBold.setText(str);
		}

		// Current date
		{
			std::ctstring_t &str = DateTime().current().format(xT("%d-%h %H:%M"), {});
			ps1 += clMagentaBold.setText(str);
		}

		// ]
		{
			std::ctstring_t &str = xT("]");
			ps1 += clWhiteBold.setText(str);
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
			lastShellOk = clGreenBold.setText(str);
		}

		std::tstring_t lastShellError;
		{
			std::ctstring_t &str = xT("✖");
			lastShellError = clRedBold.setText(str);
		}

		ps1 += Format::str(xT("$(if [[ $? == 0 ]]; then echo \"{}\"; else echo \"{}\"; fi)"),
			lastShellOk, lastShellError);
	}

	// User name
	if (_config.isUserName ||
		!Algos::isContains(_config.myUserNames, loginName))
	{
		std::ctstring_t &str = loginName;

		if (isAdmin) {
			ps1 += clRedBold.setText(str);
		} else {
			ps1 += clMagentaBold.setText(str);
		}
	}

	// Host name
	if (_config.isHostName ||
		!Algos::isContains(_config.myHostNames, hostName))
	{
		std::ctstring_t &str = hostName;

		ps1 += xT("@");
		ps1 += clCyanBold.setText(str);
	}

	ps1 += xT(" ");

	// Git repository
	if (isGitDir) {
		std::ctstring_t &gitRepoName = git.repoName();

		std::vec_tstring_t remoteRepoNames;
		git.remoteRepoNames(&remoteRepoNames);

		// [
		{
			std::ctstring_t &str = xT("[");
			ps1 += clWhiteBold.setText(str);
		}

		// Git repo URL name, remote repository names
		{
			std::ctstring_t &str = Format::str(xT("{}*{}"), git.repoUrlName(), remoteRepoNames.size());
			ps1 += clBlueBold.setText(str);

			std::ctstring_t &sep = xT("/");
			ps1 += clDefaultBold.setText(sep);
		}

		// Gitlab repo group name
		if (std::ctstring_t &groupName = git.gitlabRepoGroupName();
			!groupName.empty())
		{
			std::ctstring_t &str = Format::str(xT("{}"), groupName);
			ps1 += clCyanBold.setText(str);

			std::ctstring_t &sep = xT("/");
			ps1 += clDefaultBold.setText(sep);
		}

		// Git repository name
		{
			std::ctstring_t &str = Format::str(xT("{}"), gitRepoName);
			ps1 += clYellowBold.setText(str);
		}

		// ]
		{
			std::ctstring_t &str = xT("]");
			ps1 += clWhiteBold.setText(str);
		}

		if ( !gitRepoName.empty() ) {
			ps1 += xT(" ");
		}
	}

	// Current dir
	if (isGitDir) {
		if (_config.isDirPathInGitRepo) {
			std::ctstring_t &str = currentDirPathBrief;
			ps1 += clGreenBold.setText(str);
		}
	} else {
		std::ctstring_t &str = currentDirPathBrief;
		ps1 += clGreenBold.setText(str);
	}

	// Volume used %
	if (_config.isVolumeUsedPct ||
		volumeUsedPct > ::volumeUsedWarnPct)
	{
		if (ps1.back() != Const::space()[0]) {
			ps1 += xT(" ");
		}

		std::ctstring_t &str = Format::str(xT("{}%"), volumeUsedPct);
		ps1 += clWhiteBold.setText(str);
	}

	if (isGitDir) {
		ps1 += xT(" ");

		// Git branch name
		{
			// [
			{
				std::ctstring_t &str = xT("[");
				ps1 += clWhiteBold.setText(str);
			}

			// Git branch name
			{
				std::ctstring_t &str = git.branchName();
				ps1 += clRedBold.setText(str);
			}

			// ]
			{
				std::ctstring_t &str = xT("]");
				ps1 += clWhiteBold.setText(str);
			}
		}

		// Local branches number
		{
			std::csize_t localBranchesNum = git.localBranchesNum();
			if (localBranchesNum > 0) {
				std::ctstring_t &str = Format::str(xT("⎇{}"), localBranchesNum);
				ps1 += clWhiteBold.setText(str);
			}
		}

		// Git file statuses
		{
			std::ctstring_t &str = git.filesStatuses();
			ps1 += clYellowBold.setText(str);
		}

		// Git files number
		{
			std::cvec_tstring_t filterFileExts {};
			std::vec_tstring_t  filePathes;
			git.modifiedFiles(filterFileExts, &filePathes);

			std::ctstring_t &filesNum = std::to_string( filePathes.size() );
			if (filesNum != xT("0")) {
				ps1 += clYellowBold.setText(filesNum);
			}
		}

		// Git ahead/behind commits
		{
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

			ps1 += clMagentaBold.setText(str);
		}

		// Stashes number
		{
			std::csize_t stashesNum = git.stashesNum();
			if (stashesNum > 0) {
				std::ctstring_t &str = Format::str(xT("⚑{}"), stashesNum);
				ps1 += clBlueBold.setText(str);
			}
		}
	} // if (isGitDir)

	// Is admin user
	{
		std::ctstring_t &str = isAdmin ? xT("#") : xT("$");

		ps1 += xT(" ");

		if (isAdmin) {
			ps1 += clRedBold.setText(str);
		} else {
			ps1 += clDefaultBold.setText(str);
		}

		ps1 += xT(" ");
	}

	// mc
	if (isMc) {
		std::ctstring_t str = xT("mc");

		ps1 += clCyanBold.setText(str);
		ps1 += xT(" ");
	}

	// "> "
	{
		std::ctstring_t &str = xT("❱ ");
		ps1 += clYellowBold.setText(str);
	}

	// Console
	{
		info::PowerSupply powerSupplyInfo;
		info::Os          osInfo;
		info::Cpu         cpuInfo;
		info::Net         netInfo;

		std::tstring_t powerSupply;
		if (_config.isPowerSupply &&
			powerSupplyInfo.isExists())
		{
			std::tstring_t powerSupplyIco;

			switch ( powerSupplyInfo.status() ) {
			case info::PowerSupply::Status::Unknown:
				powerSupplyIco = xT("?");
				break;
			case info::PowerSupply::Status::Discharging:
				powerSupplyIco = xT("↓");
				break;
			case info::PowerSupply::Status::Charging:
				powerSupplyIco = xT("↑");
				break;
			case info::PowerSupply::Status::Full:
				powerSupplyIco = xT("🔋"); // ⊛,⊕,*,∗,☀,🔌
				break;
			default:
				powerSupplyIco = xT("[Todo]");
				break;
			}

			powerSupply = Format::str(xT(", Power: {}%{}"),
				powerSupplyInfo.level(),
				powerSupplyIco);
		}

		std::ctstring_t isVpn = netInfo.isVpnActive() ? xT("on") : xT("off");

		std::ctstring_t &title = Format::str(xT("{}@{} - {}, {}, CPUs: {}, VPN: {}{}                Build: {}"),
			hostName, loginName, osInfo.distro(), osInfo.desktopName(), cpuInfo.num(), isVpn,
			powerSupply, BuildInfo().datetime());

		Console console;
		console.setTitle(title);
		console.writeLine(ps1);
	}

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
		git_prompt::GitPromptApp app(::appName, {});
		exitStatus = app.run();
	}
	catch (const Exception &a_e) {
		LogCoutError() << STD_TRACE_VAR2(exitStatus, a_e.what());
	}
	catch (const std::exception &a_e) {
		LogCoutError() << STD_TRACE_VAR2(exitStatus, a_e.what());
	}
	catch (...) {
		LogCoutError() << STD_TRACE_VAR2(exitStatus, xT("Unknown error"));
	}

	return exitStatus;
}
//-------------------------------------------------------------------------------------------------
