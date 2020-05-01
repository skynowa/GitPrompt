/**
 * \file  Crypt ++
 * \brief
 *
 * export PROMPT_COMMAND='PS1="$(~/Projects/GitPrompt_eclipse_gcc/GitPrompt)"'
 */


#include <xLib/xLib.h>

#include "Config.h"
#include "GitClient.h"

using namespace git_prompt;
//-------------------------------------------------------------------------------------------------
class GitPromptApp :
    public Application
{
public:
    GitPromptApp(std::ctstring_t &a_appGuid, std::ctstring_t &a_locale) :
        Application(a_appGuid, a_locale)
    {
    }

    virtual ExitCode
    onRun() override
    {
		std::vec_tstring_t appArgs;
		args(true, &appArgs);

		Console               console;
		std::ctstring_t       dateTimeNow = DateTime().current().format(xT("%d-%h %H:%M"), {});
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

		std::tstring_t gitBranchName = git.branchName();
		if ( !gitBranchName.empty() ) {
			gitBranchName = Format::str("[{}]", gitBranchName);
		}

		std::ctstring_t ps1 =
			Format::str("[{}]{}{}@{}: {}\\w{}{}{} {} ❱ ",
				dateTimeNow,
				"✔",	// TODO: ✔
				user.name(),
				sysInfo.hostName(),
				gitRepoName,
				gitBranchName,
				git.filesStatuses(),
				git.commitsAheadBehind(),
				(user.isAdmin() ? "#" : "$")
			);

		std::tcout << ps1 << std::endl;

        return ExitCode::Success;
    }

private:
    xNO_COPY_ASSIGN(GitPromptApp)
};
//-------------------------------------------------------------------------------------------------
int_t main(int_t a_argNum, tchar_t *a_args[])
{
    xTEST_NA(a_argNum);
    xTEST_NA(a_args);

    GitPromptApp application(::appName, xT(""));

    return application.run();
}
//-------------------------------------------------------------------------------------------------
