/**
 * \file  Main.h
 * \brief GitPrompt
 */


#include <xLib/xLib.h>
#include "Config.h"
//-------------------------------------------------------------------------------------------------
namespace git_prompt
{

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
		bool_t         isHostName      {false};
		std::tstring_t myHostName      {xT("skynowa-pc")};
		bool_t         isVolumeUsedPct {true};	///< force using VolumeUsedPct
	};

	Config _config;
};

} // namespace
//-------------------------------------------------------------------------------------------------
