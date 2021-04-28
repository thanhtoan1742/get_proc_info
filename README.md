This is a custom linux module produces a character device that stream some information about the process.

to build the device, run `build.sh` as root. Remember to remove the module before rebuilding the device again.
use system call read to read from with `buffer` is the size of `procinfos` struct and `len` is the pid of the process to get information about (see test.c for example).
