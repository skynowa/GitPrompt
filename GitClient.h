/**
 * \file  GitClient.h
 * \brief
 */


#pragma once

#include <xLib/xLib.h>
#include "Config.h"
//-------------------------------------------------------------------------------------------------
xNAMESPACE_BEGIN(git_prompt)

class GitClient
{
public:
    GitClient() = default;
        ///< constructor
   ~GitClient() = default;
        ///< destructor

    xNO_COPY_ASSIGN(GitClient);

    bool           isGit() const;
        ///< check incoming data
    std::tstring_t repoName() const;
        ///< get repository name
    std::tstring_t branchName() const;
        ///< get brach name
    std::tstring_t filesStatuses() const;
        ///< get source files statuses
    std::tstring_t commitsAheadBehind() const;
        ///< get number commits ahead behind
};

xNAMESPACE_END(git_prompt)
//-------------------------------------------------------------------------------------------------
