# LinuxShell-Project
- This is a self developed basic version of Bourne shell ("Bash") with multiple similar functionalities as well as many new ones.

## How to Use ?
- Download the entire project, open up your terminal in Linux.
- Make sure you have GCC compiler installed. If you don't refer [link](https://linuxize.com/post/how-to-install-gcc-compiler-on-ubuntu-18-04/)
- Compile the shell using command `gcc myshell-shell.c linkedlist.c utilities.c -o Shell.out`
- Run using Command `./Shell.out`
- Enter `help` for list of commands.

## Tech Stack used
- All the functionalities are programmed in C language with the help of libraries such as "sys/wait.h", "sys/types.h", "signal.h" ... etc.

## Logic Used
- Providing a few built-in functions, and seeing whether the command entered by user matches them. If yes, we call our custom function, or we call our sh_launch() function.
- Important concepts in C such as fork() ,exec() and concepts of processes and their ID's.

## Results Obtained
- A fully working shell, which executes the normal functions of the default terminal, but also a few more custom functions so that the power user can use it to his ability

## Existing Features
- Custom `cd` command for changing directories.
- Custom `exit` command for exiting the shell.
- Running background jobs(by `sh_bg()` method) , listing all background jobs running at the moment (by `sh_bglist()` method) and also functionality to kill a running background job (`sh_kill()` method).

## Interesting additions
- `sh_google()` method : This method will help you search google with whatever query you provide next to it. ***Usage*** : Simple `google` will open Google on your browser, and `google strings` will search strings on the same. You can also search for strings with multiple words separated by spaces.
- `sh_yahoo` method : similarly as `sh_google` method,`yahoo string` will search string on yahoo.

