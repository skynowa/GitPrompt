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
    	bool bRv {};

		std::vec_tstring_t appArgs;
		args(true, &appArgs);

		Console console;
		// console.setTitle(::appName);
		// console.writeLine(::appName);

		git_prompt::GitClient git(appArgs, console);

		bRv = git.isGit();
		if (!bRv) {
		#if 0
			return Application::ExitCode::Failure;
		#endif
		}

		std::ctstring_t dateTimeNow = DateTime().current().format(xT("%d-%h-%Y %H:%M"), {});

		User       user;
		SystemInfo sysInfo;


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
        Cout() << "\n::::::::::::::::::::::::::::::\n";

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
