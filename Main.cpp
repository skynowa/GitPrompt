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

	std::ctstring_t       dateTimeNow      = DateTime().current().format(xT("%d-%h %H:%M"), {});
	cbool_t               isLastShellError = _isShellLastError();
	Console               console;
	git_prompt::GitClient git(appArgs, console);
	User                  user;
	SystemInfo            sysInfo;

#if 0
	Cout() << "\n::::::::::::::::::::::::::::::";
	Cout() << xTRACE_VAR(dateTimeNow);

	Cout() << xTRACE_VAR(git.isGit());
	Cout() << xTRACE_VAR(git.repoName());
	Cout() << xTRACE_VAR(git.branchName());
	Cout() << xTRACE_VAR(git.filesStatuses());
	Cout() << xTRACE_VAR(git.commitsAheadBehind());

	Cout() << xTRACE_VAR(user.isAdmin());
	Cout() << xTRACE_VAR(user.name());

	Cout() << xTRACE_VAR(sysInfo.hostName());
	Cout() << "::::::::::::::::::::::::::::::\n";
#endif

#if 0
	PS1+="$BP_TIME"
	PS1+="\$(if [[ \$? == 0 ]]; then echo \"\[\033[0;32m\]✔\"; else echo \"\[\033[0;31m\]✖\"; fi)\[\033[00m\]"
	PS1+="$(get_user)${BP_AT}${BP_HOST}:${BP_GIT_REPO}${BP_DIR}"
	PS1+="$BP_GIT_BRANCH"
	PS1+="$BP_GIT_STATES"
	PS1+="$BP_GIT_AHEAD_BEHIND"
	PS1+=" $(get_user_dirty)"
	PS1+=" $BP_ARROW ";
#endif

	std::tstring_t gitRepoName = git.repoName();
	if ( !gitRepoName.empty() ) {
		gitRepoName = Format::str("[{}] ", gitRepoName);
	}

	std::ctstring_t currentDirPath = Dir::current();

	std::tstring_t gitBranchName = git.branchName();
	if ( !gitBranchName.empty() ) {
		gitBranchName = Format::str("[{}]", gitBranchName);
	}

	std::tstring_t ps1;

	// dateTimeNow
	{
        Console::Foreground foreground = Console::Foreground::Green;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += "[" + dateTimeNow + "]";
        ps1 += console.setAttributesDef();
	}

	// isLastShellError
	{
        Console::Foreground foreground = Console::Foreground::Red;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += (isLastShellError ? "✖" : "✔");
        ps1 += console.setAttributesDef();
	}

	// user.name()
	{
        Console::Foreground foreground = Console::Foreground::Magenta;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += user.name();
        ps1 += console.setAttributesDef();
        ps1 += xT("@");
	}

	// sysInfo.hostName()
	{
        Console::Foreground foreground = Console::Foreground::Cyan;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += sysInfo.hostName();
        ps1 += console.setAttributesDef();
        ps1 += xT(": ");
	}

	// gitRepoName
	{
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += gitRepoName;
        ps1 += console.setAttributesDef();
	}

	// Current dir
	{
        Console::Foreground foreground = Console::Foreground::Green;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += currentDirPath;
        ps1 += console.setAttributesDef();
	}

	// gitBranchName
	{
        Console::Foreground foreground = Console::Foreground::Red;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += gitBranchName;
        ps1 += console.setAttributesDef();
	}

	// git.filesStatuses()
	{
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        // ps1 += git.filesStatuses();
        ps1 += console.setAttributesDef();
	}

	// git.commitsAheadBehind()
	{
        Console::Foreground foreground = Console::Foreground::Magenta;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        // ps1 += git.commitsAheadBehind();
        ps1 += console.setAttributesDef();
        // ps1 += xT(" ");
	}

	// user.isAdmin()
	{
        Console::Foreground foreground = Console::Foreground::Default;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        ps1 += (user.isAdmin() ? "#" : "$");
        ps1 += console.setAttributesDef();
        ps1 += xT(" ");
	}

	// "> "
	{
        Console::Foreground foreground = Console::Foreground::Yellow;
        Console::Background background = Console::Background::Default;
        cint_t              attributes = static_cast<int_t>(Console::Attribute::Bold);

        // ps1 += console.setAttributes(foreground, background, attributes);
        // ps1 += xT("❱ ");
        ps1 += xT("> ");
        ps1 += console.setAttributesDef();
	}

#if 0
	std::ctstring_t ps1 =
		Format::str("[{}]{}{}@{}: {}\\w{}{}{} {} ❱ ",
			dateTimeNow,
			isLastShellError,
			user.name(),
			sysInfo.hostName(),
			gitRepoName,
			gitBranchName,
			git.filesStatuses(),
			git.commitsAheadBehind(),
			(user.isAdmin() ? "#" : "$")
		);
#endif

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
