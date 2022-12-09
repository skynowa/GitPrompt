/**
 * \file  GitPromptApp.h
 * \brief Git prompt tools
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
///@name ctors, dtor
///@{
	GitPromptApp(std::ctstring_t &appGuid, std::ctstring_t &locale);
   ~GitPromptApp() = default;

	xNO_COPY_ASSIGN(GitPromptApp)
///@}

	ExitCode onRun() final;

private:
	struct Config
	{
		bool_t             isHostName      {false};
		std::vec_tstring_t myHostNames     {xT("skynowa-pc"), xT("skynowa-laptop")};
		bool_t             isVolumeUsedPct {true};	///< force using VolumeUsedPct
	};

	Config _config;
};

} // namespace
//-------------------------------------------------------------------------------------------------
