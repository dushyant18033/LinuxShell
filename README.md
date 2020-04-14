# LinuxShell
Simple implementation of a basic linux shell in C, capable of handling any executable binary (need to give the absolute path), pipes, I/O redirections, as given in the ReadMe file.

## The following are handled.

* __command__: execute the command and wait for the command to finish
* __command > filename__: redirect stdout to file filename. If the file does not exist create one, otherwise, overwrite the existing file
* __command >> filename__: If the filename already exists append the stdout output, otherwise, create a new file
* __1>filename__: redirect stdout to filename
* __2>filename__: redirect stderr to filename
* __2>&1__: redirect stderr to stdout
* __command < filename__: use file descriptor 0 (stdin) for filename. If command tries to read from stdin, effectively it will read from filename.
* __|__: pipe command
* __exit__: exit from the shell program
