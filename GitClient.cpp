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
GitClient::isGitDir() const
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
	xCHECK_RET(!isGitDir(), xT(""));

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
	sRv = String::trimSpace(sRv);

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
	xCHECK_RET(!isGitDir(), xT(""));

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

	sRv = String::trimSpace(sRv);

    return sRv;
}
//-------------------------------------------------------------------------------------------------
std::size_t
GitClient::localBranchesNum() const
{
	xCHECK_RET(!isGitDir(), 0);

	std::ctstring_t                     filePath {"/usr/bin/git"};
	std::cvec_tstring_t                 params   {"branch"};
	const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);

	std::vec_tstring_t values;
	String::split(String::trimSpace(stdOut), Const::nl(), &values);
	xCHECK_RET(values.empty(), 0);

	// without "master"
	return values.size() - 1;
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
	xCHECK_RET(!isGitDir(), xT(""));

	std::tstring_t sRv;

	std::ctstring_t                     filePath {"/usr/bin/git"};
	std::cvec_tstring_t                 params   {"status"};
	const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	cbool_t isNoCommit  = (stdOut.find("nothing to commit") != std::tstring_t::npos);
	cbool_t isModified  = (stdOut.find("modified:")         != std::tstring_t::npos);
	cbool_t isUntracked = (stdOut.find("Untracked files")   != std::tstring_t::npos);
	cbool_t isNew       = (stdOut.find("new file:")         != std::tstring_t::npos);
	cbool_t isRenamed   = (stdOut.find("renamed:")          != std::tstring_t::npos);
	cbool_t isDeleted   = (stdOut.find("deleted:")          != std::tstring_t::npos);

	if (isNoCommit) {
		sRv = "✔";
	}

	if (isRenamed) {
		sRv =+ "△";
	}

	if (isNew) {
		sRv =+ "+";
	}

	if (isUntracked) {
		sRv =+ "…";
	}

	if (isDeleted) {
		sRv =+ "✖";
	}

	if (isModified) {
		sRv =+ "●";
	}

	return sRv;
}
//-------------------------------------------------------------------------------------------------
/**
 find_git_ahead_behind()
 {
 	if ! $(is_git); then
 		git_ahead_behind=""
 		return
 	fi

 	local status=$(git rev-list --left-right --count origin/master...$branch)
 	local aheadRegex="([0-9]+)"
 	local behindRegex="\s(\w+)$"

 	[[ $status =~ $aheadRegex ]]  && ahead="${BASH_REMATCH[1]}"  || ahead="0"
 	[[ $status =~ $behindRegex ]] && behind="${BASH_REMATCH[1]}" || behind="0"

??? [[ $(git status 2> /dev/null | tail -n1) != "nothing to commit, working tree clean" ]] && git_ahead_behind=""
 	[[ $ahead != "0" ]]  && git_ahead_behind="↑${ahead}"
 	[[ $behind != "0" ]] && git_ahead_behind="↓${behind}"
 }
*/
std::tstring_t
GitClient::commitsAheadBehind() const
{
	xCHECK_RET(!isGitDir(), xT(""));

	std::tstring_t sRv;

	std::ctstring_t                     filePath {"/usr/bin/git"};
	std::cvec_tstring_t                 params   {"rev-list", "--left-right", "--count", "origin/master..." + branchName()};
	const std::set<std::pair_tstring_t> envs;
	std::tstring_t                      stdOut;
	std::tstring_t                      stdError;

	Process::execute(filePath, xTIMEOUT_INFINITE, params, envs, &stdOut, &stdError);
	// Cout() << xTRACE_VAR(stdOut);
	// Cout() << xTRACE_VAR(stdError);

	std::tstring_t ahead  {"0"};
	std::tstring_t behind {"0"};
	{
		std::vec_tstring_t values;
		String::split(stdOut, Const::ht(), &values);
		if (values.size() == 2) {
			ahead  = ::String::trimSpace(values.at(0));
			behind = ::String::trimSpace(values.at(1));
		}

		// Cout() << xTRACE_VAR_2(ahead, behind);
	}

	// format
	if (ahead != "0") {
		sRv = "↑" + ahead;
	}

	if (behind != "0") {
		sRv = "↓" + behind;
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
