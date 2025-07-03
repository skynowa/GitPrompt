/**
 * \file  GitPromptApp.h
 * \brief Git prompt tools
 */


#include <xLib/xLib.h>
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

    xNO_DEFAULT_CTOR(GitPromptApp);
	xNO_COPY_ASSIGN(GitPromptApp);
///@}

	ExitCode onRun() final;

private:
	const Config _config;

	std::tstring_t _wrapAnsiForPS1(std::ctstring_t &ps1_str) const;
		///< safe PS1 now can be assigned to PS1 without messing up cursor/line wrap
};

} // namespace
//-------------------------------------------------------------------------------------------------
