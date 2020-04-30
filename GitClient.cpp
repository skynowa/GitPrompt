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
