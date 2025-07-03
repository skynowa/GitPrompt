#!/usr/bin/env bash
#
# \file  PS1.sh
# \brief Export PS! env variable
#


function update_ps1()
{
	export PS1="$(~/Projects/GitPrompt_build/GitPrompt $?)"
}

PROMPT_COMMAND=update_ps1
