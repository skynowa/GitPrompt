#!/usr/bin/env bash
#
# \file  PS1.sh
# \brief Export PS! env variable
#


function update_ps1_cpp()
{
	export PS1="$(~/Projects/GitPrompt_build/GitPrompt $?)"
}

function update_ps1_py()
{
	export PS1="$(python3 ~/Projects/GitPrompt/GitPromptApp.py $?)"
}

PROMPT_COMMAND=update_ps1_py
