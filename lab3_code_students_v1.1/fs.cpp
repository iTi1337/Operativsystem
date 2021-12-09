#include <iostream>
#include "fs.h"
#include <stdio.h>
#include <string.h>

FS::FS()
{
    std::cout << "FS::FS()... Creating file system\n";
}

FS::~FS()
{

}

// formats the disk, i.e., creates an empty file system
int
FS::format()
{
    uint8_t *blk = (uint8_t*)malloc(BLOCK_SIZE); //writes a whole empty block
    disk.write(0, blk); //time to write some cooode
    //fat = (uint8_t*)malloc(BLOCK_SIZE); //writes a whole empty block (should mean the)
    memset(fat, 0, sizeof(fat)/sizeof(int16_t));
    fat[0] = disk.get_no_blocks();// 4096/2
    fat[1] = 2041;
    disk.write(1, (uint8_t*)fat); //time to write some cooode
    //how do i get a create disk function hmmm
    std::cout << "FS::format()\n";
    return 0;
}

// create <filepath> creates a new file on the disk, the data content is
// written on the following rows (ended with an empty row)
int
FS::create(std::string filepath) // /name_of_file
{   
    //char input[10];
    //std::cin >> input;
    uint8_t *blk = (uint8_t*)malloc(BLOCK_SIZE); //writes a whole empty block
    disk.read(1, blk);
    blk = (uint8_t*)blk;
    for (int i = 0; i < 10; i+=2){
        std::cout << "Read value " << (uint16_t)(((blk[i] << 8) | blk[i+1] )<< 8) << "\n";
    }
    //struct dir_entry new_file;
    //new_file.file_name = filepath;
    //strcpy(new_file.file_name, filepath.c_str()); //yeah?
    //blk = (uint8_t*)malloc(BLOCK_SIZE); //writes a whole empty block
    //disk.write(3, blk);

    std::cout << "FS::create(" << filepath << ")\n";
    return 0;
}

// cat <filepath> reads the content of a file and prints it on the screen
int
FS::cat(std::string filepath)
{
    std::cout << "FS::cat(" << filepath << ")\n";
    return 0;
}

// ls lists the content in the currect directory (files and sub-directories)
int
FS::ls()
{
    std::cout << "FS::ls()\n";
    return 0;
}

// cp <sourcepath> <destpath> makes an exact copy of the file
// <sourcepath> to a new file <destpath>
int
FS::cp(std::string sourcepath, std::string destpath)
{
    std::cout << "FS::cp(" << sourcepath << "," << destpath << ")\n";
    return 0;
}

// mv <sourcepath> <destpath> renames the file <sourcepath> to the name <destpath>,
// or moves the file <sourcepath> to the directory <destpath> (if dest is a directory)
int
FS::mv(std::string sourcepath, std::string destpath)
{
    std::cout << "FS::mv(" << sourcepath << "," << destpath << ")\n";
    return 0;
}

// rm <filepath> removes / deletes the file <filepath>
int
FS::rm(std::string filepath)
{
    std::cout << "FS::rm(" << filepath << ")\n";
    return 0;
}

// append <filepath1> <filepath2> appends the contents of file <filepath1> to
// the end of file <filepath2>. The file <filepath1> is unchanged.
int
FS::append(std::string filepath1, std::string filepath2)
{
    std::cout << "FS::append(" << filepath1 << "," << filepath2 << ")\n";
    return 0;
}

// mkdir <dirpath> creates a new sub-directory with the name <dirpath>
// in the current directory
int
FS::mkdir(std::string dirpath)
{
    std::cout << "FS::mkdir(" << dirpath << ")\n";
    return 0;
}

// cd <dirpath> changes the current (working) directory to the directory named <dirpath>
int
FS::cd(std::string dirpath)
{
    std::cout << "FS::cd(" << dirpath << ")\n";
    return 0;
}

// pwd prints the full path, i.e., from the root directory, to the current
// directory, including the currect directory name
int
FS::pwd()
{
    std::cout << "FS::pwd()\n";
    return 0;
}

// chmod <accessrights> <filepath> changes the access rights for the
// file <filepath> to <accessrights>.
int
FS::chmod(std::string accessrights, std::string filepath)
{
    std::cout << "FS::chmod(" << accessrights << "," << filepath << ")\n";
    return 0;
}
