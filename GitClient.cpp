/**
 * \file  GitClient.cpp
 * \brief
 */


#include "GitClient.h"


xNAMESPACE_BEGIN(git_prompt)

/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
GitClient::GitClient(
    std::cvec_tstring_t &a_args,
    const Console       &a_console
) :
    _args   {a_args},
    _console{a_console}
{
}
//-------------------------------------------------------------------------------------------------
/**
 is_git()
 {
 	if ! git rev-parse --git-dir > /dev/null 2>&1; then
 		false
 		return
 	fi

 	true;
 }
*/
bool
GitClient::isGit() const
{
    std::ctstring_t                     filePath {"/usr/bin/git"};
    std::cvec_tstring_t                 params   {"rev-parse", "--git-dir"};
    const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	xCHECK_RET(!stdError.empty(), false);
	xCHECK_RET((stdOut.find(xT(".git")) == std::tstring_t::npos), false);

    return true;
}
//-------------------------------------------------------------------------------------------------
/**
 get_git_repo()
 {
     if ! $(is_git); then
         return
     fi

     path=`git rev-parse --show-toplevel`
     dir_name=`basename $path`

     echo $dir_name
 }
*/
std::tstring_t
GitClient::repoName() const
{
	xCHECK_RET(!isGit(), xT(""));

	std::tstring_t sRv;

    std::ctstring_t                     filePath {"/usr/bin/git"};
    std::cvec_tstring_t                 params   {"rev-parse", "--show-toplevel"};
    const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	sRv = Path(stdOut).fileBaseName();
	sRv = ::String::trimSpace(sRv);

    return sRv;
}
//-------------------------------------------------------------------------------------------------
/**
 git_branch=""

 find_git_branch()
 {
     if ! $(is_git); then
         git_branch=""
         return
     fi

     branch=$(git rev-parse --abbrev-ref HEAD 2> /dev/null)

     if   [[ "$branch" == "" ]]; then
         git_branch=""
     elif [[ "$branch" == "HEAD" ]]; then
         git_branch="detached*"
     else
         git_branch="$branch"
     fi
 }
*/
std::tstring_t
GitClient::branchName() const
{
	xCHECK_RET(!isGit(), xT(""));

	std::tstring_t sRv;

    std::ctstring_t                     filePath {"/usr/bin/git"};
    std::cvec_tstring_t                 params   {"rev-parse", "--abbrev-ref", "HEAD"};
    const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	if   (stdOut == "") {
		sRv = "";
	}
	else if (stdOut == "HEAD") {
		sRv = "detached*";
	}
	else {
		sRv = stdOut;
	}

	sRv = ::String::trimSpace(sRv);

    return sRv;
}
//-------------------------------------------------------------------------------------------------
/**
 git_states=""

 get_git_states()
 {
	local status=`git status 2>&1 | tee`

	local nocommit=`echo  "${status}" 2> /dev/null | grep "nothing to commit" &> /dev/null; echo "$?"`
	local dirty=`echo     "${status}" 2> /dev/null | grep "modified:"         &> /dev/null; echo "$?"`
	local untracked=`echo "${status}" 2> /dev/null | grep "Untracked files"   &> /dev/null; echo "$?"`
	local newfile=`echo   "${status}" 2> /dev/null | grep "new file:"         &> /dev/null; echo "$?"`
	local renamed=`echo   "${status}" 2> /dev/null | grep "renamed:"          &> /dev/null; echo "$?"`
	local deleted=`echo   "${status}" 2> /dev/null | grep "deleted:"          &> /dev/null; echo "$?"`

	 git_states=""

	if [ "${nocommit}" == "0" ]; then
		git_states="✔"
	fi

	if [ "${renamed}" == "0" ]; then
		git_states="△${git_states}"
	fi

	if [ "${newfile}" == "0" ]; then
		git_states="+${git_states}"
	fi

	if [ "${untracked}" == "0" ]; then
		git_states="…${git_states}"
	fi

	if [ "${deleted}" == "0" ]; then
		git_states="✖${git_states}"
	fi

	if [ "${dirty}" == "0" ]; then
		git_states="●${git_states}"
	fi
 }
*/
std::tstring_t
GitClient::filesStatuses() const
{
	xCHECK_RET(!isGit(), xT(""));

	std::tstring_t sRv;

	std::ctstring_t                     filePath {"/usr/bin/git"};
	std::cvec_tstring_t                 params   {"status"};
	const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	cbool_t nocommit  = (stdOut.find("nothing to commit") != std::tstring_t::npos);
	cbool_t dirty     = (stdOut.find("modified:")         != std::tstring_t::npos);
	cbool_t untracked = (stdOut.find("Untracked files")   != std::tstring_t::npos);
	cbool_t newfile   = (stdOut.find("new file:")         != std::tstring_t::npos);
	cbool_t renamed   = (stdOut.find("renamed:")          != std::tstring_t::npos);
	cbool_t deleted   = (stdOut.find("deleted:")          != std::tstring_t::npos);

	if (nocommit) {
		sRv="✔";
	}

	if (renamed) {
		sRv =+ "△";
	}

	if (newfile) {
		sRv =+ "+";
	}

	if (untracked) {
		sRv =+ "…";
	}

	if (deleted) {
		sRv =+ "✖";
	}

	if (dirty) {
		sRv =+ "●";
	}

	return sRv;
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

xNAMESPACE_END(git_prompt)
