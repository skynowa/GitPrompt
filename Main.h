/**
 * \file  Main.h
 * \brief GitPrompt
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
    GitPromptApp(std::ctstring_t &a_appGuid, std::ctstring_t &a_locale);
   ~GitPromptApp() = default;

    xNO_COPY_ASSIGN(GitPromptApp)

    ExitCode onRun() override;
};
//-------------------------------------------------------------------------------------------------
/**
 * TODO:
 * - repo icon: xT("🙃");
 *
 * DONE:
 * - Local branches number
 * - Stashes number
 *
 */
//-------------------------------------------------------------------------------------------------
