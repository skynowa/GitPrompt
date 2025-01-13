/**
 * \file  GitPromptApp.h
 * \brief Git prompt tools
 */


#include <xLib/xLib.h>
#include <StdTest/StdTest.h>
#include <StdStream/StdStream.h>
#include "Config.h"
//-------------------------------------------------------------------------------------------------
namespace git_prompt
{

class GitPromptApp final :
	public Application
{
public:
///@name ctors, dtor
///@{
	GitPromptApp(std::ctstring_t &appGuid, std::ctstring_t &locale);
   ~GitPromptApp() = default;

	xNO_DEFAULT_CONSTRUCT(GitPromptApp)
	xNO_COPY_ASSIGN(GitPromptApp)
///@}

	ExitCode onRun() final;

private:
	struct Config
	{
		// Host
		bool_t             isHostName      {false};
		std::vec_tstring_t myHostNames     {xT("skynowa-pc"), xT("skynowa-laptop")};

		// User
		bool_t             isUserName      {false};
		std::vec_tstring_t myUserNames     {xT("skynowa")};

		// Volume
		bool_t             isVolumeUsedPct {true};	///< force using VolumeUsedPct

		// Power supply
		bool_t             isPowerSupply   {true};	///< force using power supply info
	};

	const Config _config;
};

} // namespace
//-------------------------------------------------------------------------------------------------
