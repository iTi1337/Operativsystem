#include "shell.h"
#include "fs.h" //the module that writes to memmory in shell through commands in the fs.cpp file
#include "disk.h" //the simulation of a disk (block device?), done in binary
//In the laboratory assignment you should modify only the files fs.h and fs.cpp (i.e., the file system part).
int
main(int argc, char **argv) //starts up the shell
{
    Shell shell; //shell is a simulated directory. 
    //Shell also provides commands to interact with the shell. 
    //This is through commands in the shell.ccp file
    shell.run();
    return 0;
}
