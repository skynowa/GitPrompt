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
bool
GitClient::isGitDir() const
{
    std::cvec_tstring_t params {"rev-parse", "--git-dir"};
	std::tstring_t      stdOut;
	std::tstring_t      stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);
	xCHECK_RET(!stdError.empty(), false);
	xCHECK_RET((stdOut.find(xT(".git")) == std::tstring_t::npos), false);

    return true;
}
//-------------------------------------------------------------------------------------------------
std::tstring_t
GitClient::repoName() const
{
	xCHECK_RET(!isGitDir(), xT(""));

	std::tstring_t sRv;

    std::cvec_tstring_t params {"rev-parse", "--show-toplevel"};
	std::tstring_t      stdOut;
	std::tstring_t      stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);

	sRv = Path(stdOut).fileBaseName();
	sRv = String::trimSpace(sRv);

    return sRv;
}
//-------------------------------------------------------------------------------------------------
std::tstring_t
GitClient::branchName() const
{
	xCHECK_RET(!isGitDir(), xT(""));

	std::tstring_t sRv;

    std::cvec_tstring_t params {"rev-parse", "--abbrev-ref", "HEAD"};
	std::tstring_t      stdOut;
	std::tstring_t      stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);

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

	std::cvec_tstring_t  params {"branch"};
	std::tstring_t       stdOut;
	std::tstring_t       stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);

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

	std::cvec_tstring_t params {"status"};
	std::tstring_t      stdOut;
	std::tstring_t      stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);

	cbool_t isNoCommit  = StringCI::contains(stdOut, xT("nothing to commit"));
	cbool_t isModified  = StringCI::contains(stdOut, xT("modified:"));
	cbool_t isUntracked = StringCI::contains(stdOut, xT("Untracked files"));
	cbool_t isNew       = StringCI::contains(stdOut, xT("new file:"));
	cbool_t isRenamed   = StringCI::contains(stdOut, xT("renamed:"));
	cbool_t isDeleted   = StringCI::contains(stdOut, xT("deleted:"));

	std::tstring_t sRv;

	if (isNoCommit) {
		sRv += xT("✔");
	}

	if (isRenamed) {
		sRv += xT("△");
	}

	if (isNew) {
		sRv += xT("✚");
	}

	if (isUntracked) {
		sRv += xT("…");
	}

	if (isDeleted) {
		sRv += xT("✖");
	}

	if (isModified) {
		sRv += xT("●");
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

	std::cvec_tstring_t params {"rev-list", "--left-right", "--count", "origin/master..." + branchName()};
	std::tstring_t      stdOut;
	std::tstring_t      stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);

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
std::size_t
GitClient::stashesNum() const
{
	xCHECK_RET(!isGitDir(), 0);

	std::cvec_tstring_t params {"stash", "list"};
	std::tstring_t      stdOut;
	std::tstring_t      stdError;

	Process::execute(::gitPath, params, {}, xTIMEOUT_INFINITE, &stdOut, &stdError);

	std::vec_tstring_t values;
	String::split(String::trimSpace(stdOut), Const::nl(), &values);

	return values.size();
}
//-------------------------------------------------------------------------------------------------

xNAMESPACE_END(git_prompt)
