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

    ExitCode onRun() override;

private:
    xNO_COPY_ASSIGN(GitPromptApp)
};
//-------------------------------------------------------------------------------------------------
