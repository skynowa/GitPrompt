/**
 * \file  Main.h
 * \brief GitPrompt
 */


#include <xLib/xLib.h>
#include "Config.h"
//-------------------------------------------------------------------------------------------------
xNAMESPACE_BEGIN(git_prompt)

class GitPromptApp :
	public Application
{
public:
			 GitPromptApp(std::ctstring_t &appGuid, std::ctstring_t &locale);
			~GitPromptApp() = default;

	xNO_COPY_ASSIGN(GitPromptApp)

	ExitCode onRun() override;

private:
	struct Config
	{
		bool_t         isHostName {false};
		std::tstring_t myHostName;
	} _config;
};

xNAMESPACE_END(git_prompt)
//-------------------------------------------------------------------------------------------------
/**
 * TODO:
 * - repo icon: xT("🙃");
 *
 * DONE:
 * - repo URL name
 * - Local branches number
 * - Stashes number
 *
 * UNTODO:
 * - screens num (Must be connected to a terminal.)
 *
 */
//-------------------------------------------------------------------------------------------------
