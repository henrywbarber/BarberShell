---
title: Barber Shell
layout: default
---

# Barber Shell

## Overview
The Barber Shell is a personal project where I’ve built a custom Unix-based shell from scratch, with select custom coded built in functions. This shell, called `barber`, serves as a command-line interpreter, emulating the fundamental functionality of popular shells like bash or zsh, but with a simpler design.

In interactive mode, the shell reads user input and executes commands, and in batch mode, it processes commands from a file. The shell supports basic functionality such as forking processes, executing commands, handling I/O redirections, and managing both environment and shell variables.

## Features
### 1. Interactive & Batch Modes
Interactive Mode: The shell prompts for user input and executes the command after parsing it.
Batch Mode: Executes commands from a file, without showing a prompt, for automation.
### 2. Built-in Commands
* `exit`: Terminates the shell session.
* `cd`: Handles change directory commands.
* `ls`: Handles listing current directory contents.
* `export`: Handles setting or editing enviorment variables.
* `local`: Handles shell-specific variables, similar to local variables in programming.
* `vars`: Provides output of local variables and values.
* `history`: Provides recently used commands, allows for recalling commands, and setting history size.
### 3. Command Execution
The shell can handle external commands by spawning child processes. It locates executables using the system path and supports passing arguments to commands.
### 4. Redirection
The shell supports various forms of input/output redirection to manage how command results are handled:
* Input: `[optional file discriptor]<file` to read input from a file.
* Output: `[optional file discriptor]>file` to write output to a file.
* Append Output: `[optional file discriptor]>>file` to append the output to a file.
* Standard Output and Error: `&>file` for redirecting both stdout and stderr simultaneously.
* Appending Standard Output and Error: `&>>file` for redirecting both stdout and stderr simultaneously.
### 5. Variable Management
Supports environment variables as well as shell variables, with the ability to set, reference, and use them in commands.



## Development Insights
This project has been an excellent learning experience in system-level programming, particularly with process management using `fork()`, `execv()`, `dup()`, `dup2()`, and `wait()`. I also explored efficient memory management techniques to handle shell operations such as parsing commands and managing redirections. Through iterative testing, I enhanced the robustness of the shell by introducing error handling and debugging facilities.

### Future Enhancements
Some future features I plan to enhance or add:
* Memory Management: Full optimization of dynamic variables for input, as well as full memory freeing.
* Pipelines: Allow chaining commands using `|`.
* Job Control: Add support for handling background processes.
* Enhanced Scripting Capabilities: Improve the handling of shell scripts with more robust variable expansions and conditions.