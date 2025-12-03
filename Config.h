/**
 * \file  Config.h
 * \brief config
 */


#pragma once

using namespace xl;
using namespace xl::debug;
using namespace xl::log;
using namespace xl::fs;
using namespace xl::sync;
using namespace xl::system;
//-------------------------------------------------------------------------------------------------
inline const std::ctstring_t appName = xT("GitPrompt");
//-------------------------------------------------------------------------------------------------
namespace git_prompt
{

struct Config
{
	// Date, timer
	bool_t             isDate          {false};
	bool_t             isTime          {true};

	// Host
	bool_t             isHostName      {false};
	std::vec_tstring_t myHostNames     {xT("skynowa-pc"), xT("skynowa-laptop")};

	// User
	bool_t             isUserName      {false};
	std::vec_tstring_t myUserNames     {xT("skynowa")};

	// Dirs
	bool_t             isDirPathInGitRepo {false};
		///< Don't show current dir path in Git repository

	// Volume
	bool_t             isVolumeUsedPct {false};	///< force using VolumeUsedPct

	// Power supply
	bool_t             isPowerSupply   {true};	///< force using power supply info
};

} // namespace
//-------------------------------------------------------------------------------------------------
