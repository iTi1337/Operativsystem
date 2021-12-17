#include <iostream>
#include "fs.h"
#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>

FS::FS()
{
    std::cout << "FS::FS()... Creating file system\n";
    disk.read(1, (uint8_t*)fat);
}

FS::~FS()
{

}

//HELPER FUNCTIONS HELP ME FALL ASLEEP AT NIGHT
//Takes in fat starting position and returns all fat indexes it finds untill it finds a -1 (EOF)
std::vector<int>
FS::get_fats(int first_blk){ 
    std::vector<int> return_values; //vectors are cool
    int pointer = first_blk;
    while (fat[pointer] != -1) {
        return_values.push_back(pointer);
        pointer = fat[pointer]; //Sets the pointer to the next value
    } 
    return_values.push_back(pointer); //To include -1 in return values
    return return_values;
}

//LEET GAMER CODE
//Takes in the first block of a file and returns the entire file, even if it's stored on multiple blocks
std::string 
FS::diskread(int first_blk){
    std::vector<int> to_read = get_fats(first_blk); //Gets all fat blocks to read from
    std::string total_read; //Where read values are stored
    for(int i = 0; i < to_read.size(); i++){//Reads one block and appends to output
        char data[4097] = {0};  //One extra bit to store null terminator
        disk.read(to_read[i], (uint8_t*)data);
        total_read.append(data);
    }
    return total_read;
}

//HIGHLY EXPERIMENTAL, TREAD WITH CAUTION
//Takes in what to write, and optionally where to start writing it, and returns where the first fat index is located
int
FS::diskwrite(std::string input, int start_blk = 0){
    int size = input.length();
    int blocks = (int)(size/4096); //Counts how many blocks to write to
    int fat_indexes[blocks+1]; //How many indexes that are necessary
    int i = 0;
    if (start_blk != 0){ //If we specify block to start at
        i = 1; //Since start blk is set, we skip i = 0 in the loop
        fat_indexes[0] = start_blk;
        std::vector<int> to_reset = get_fats(start_blk);
        for(int j = 0; j < to_reset.size();j++){ //Nulls blocks which the block we started at points at
            fat[to_reset[j]] = 0;
        }
    }
    for (i; i <= blocks; i++){ //Looks for empty fat indexes for each block
        for (int j = 0; j < disk.get_no_blocks(); j++){
            if(fat[j] == 0){
                fat_indexes[i] = j; //Saves found empty FAT index
                fat[j] = -1;
                break;
            }
        }
    }

    //THIS MIGHT BE WHERE WE WOULD WRITE CODE TO CHECK IF DISK IS FULL, AKA DID NOT FIND ENOUGH EMPTY FATS, THAT'S A JOB FOR LATER THOUGH
    
    for (int i = 0; i <= blocks; i++){ //Loops for every block necessary to write to
        std::string partition = "";
        if (i == blocks){ 
            //IF LAST BLOCK
            fat[fat_indexes[i]] = -1; //EOF
            partition.append(input.substr(4096*i, size % 4096)); //Writes the last bits of the ipnut to partition
            partition.resize(4096); //To get rid of junk already stored in block
            disk.write(fat_indexes[i], (uint8_t*)partition.c_str());
        }
        else{
            fat[fat_indexes[i]] = fat_indexes[i+1]; //Points towards next fat index
            partition.append(input.substr(4096*i, 4096)); //Since it's not the last block, we know that more than 4096 bytes can be written
            disk.write(fat_indexes[i], (uint8_t*)partition.data());
        }
    }
    disk.write(1, (uint8_t*)fat);
    return fat_indexes[0];
}

// formats the disk, i.e., creates an empty file system
int
FS::format()
{
    uint8_t *blk = (uint8_t*)calloc(BLOCK_SIZE, sizeof(uint8_t*)); //Creates an empty block
    disk.write(0, blk); //Writes it to the root directory
    memset(fat, 0, sizeof(fat)); //Resets the local fat variable
    fat[0] = -1; //Sets initial values
    fat[1] = -1;
    disk.write(1, (uint8_t*)fat); //Writes local fat to the disk
    std::cout << "FS::format()\n";
    return 0;
}

// create <filepath> creates a new file on the disk, the data content is
// written on the following rows (ended with an empty row)
int
FS::create(std::string filepath) // /name_of_file
{   
    std::string input = ""; //Variable for the entire input
    int first = 1; //To check if it's the first line of input
    std::string line_input; //To read new lines
    while (getline(std::cin, line_input, '\n')){
        if (line_input.empty())
            break;
        else{
            if(first != 1){ //if it's ugly and it's working, is it stil- yes, yes it is.
                input.append("\n");
            }
            first = 0;
            input.append(line_input);
        }
    }
    //Creates the new directory entry
    dir_entry *new_file;
    new_file = (dir_entry*)calloc(1, sizeof(dir_entry));
    strcpy(new_file->file_name, filepath.c_str());
    new_file->size = input.length();
    new_file->first_blk = diskwrite(input); //Writes the input to the disk and store where it begins
    
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    //Finds a spot for the file in the directory
    // DOES NOT CHECK FOR DUPLICATES, SHOULD PROBABLY BE FIXED EVENTUALLY
    for (int j = 0; j < 64; j++){
        if (strcmp(blk[j].file_name, "") == 0){
            blk[j] = *new_file;
            disk.write(0, (uint8_t*)blk);
            break;
        }
    }
    std::cout << "FS::create(" << filepath << ")\n";
    return 0;
}

// cat <filepath> reads the content of a file and prints it on the screen
int
FS::cat(std::string filepath)
{
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath.c_str()) == 0){ //Finds the file with the inputted filepath
            std::string content;
            dir_entry file = blk[i];
            content.append(diskread(file.first_blk));
            std::cout << content << "\n";
            break;
        }
    }

    return 0;
}

// ls lists the content in the currect directory (files and sub-directories)
int
FS::ls()
{
    std::cout << "FS::ls()\n";
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    std::cout << "name" << "   " << "size" << "\n";
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, "") != 0){ //List every filename in directory which is not null
            std::cout << blk[i].file_name << "   " << blk[i].size << "\n";
        }
    }
    return 0;
}

// cp <sourcepath> <destpath> makes an exact copy of the file
// <sourcepath> to a new file <destpath>
int
FS::cp(std::string sourcepath, std::string destpath)
{
    std::cout << "FS::cp(" << sourcepath << "," << destpath << ")\n";
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, sourcepath.c_str()) == 0){ //finds the file with the source name
            //Creates the new directory entry
            dir_entry *new_file;
            new_file = (dir_entry*)malloc(sizeof(dir_entry));
            strcpy(new_file->file_name, destpath.c_str());
            new_file->size = blk[i].size;
            new_file->first_blk = diskwrite(diskread(blk[i].first_blk)); //Reads the data from the old file, writes the data from the new file and saves where it starts, how neat aint that (genuint stolt över detta)
            //Finds a spot for the file in the directory
            // DOES NOT CHECK FOR DUPLICATES, SHOULD PROBABLY BE FIXED EVENTUALLY
            for (int k = 0; k < 64; k++){
                if (strcmp(blk[k].file_name, "") == 0){
                    blk[k] = *new_file;
                    disk.write(0, (uint8_t*)blk);
                    break;
                }
            }
            break;
        }
    }
    return 0;
}

// mv <sourcepath> <destpath> renames the file <sourcepath> to the name <destpath>,
// or moves the file <sourcepath> to the directory <destpath> (if dest is a directory)
int
FS::mv(std::string sourcepath, std::string destpath)
{
    std::cout << "FS::mv(" << sourcepath << "," << destpath << ")\n";
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, sourcepath.c_str()) == 0){ //Finds the source file
            strcpy(blk[i].file_name, destpath.c_str()); //Changes its name to specified name
            disk.write(0, (uint8_t*)blk); //Writes the change to the disk
            break;
        }
    }
    return 0;
}

// rm <filepath> removes / deletes the file <filepath> 
int
FS::rm(std::string filepath)
{
    std::cout << "FS::rm(" << filepath << ")\n";
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath.c_str()) == 0){
            std::vector<int> fat_indexes = get_fats(blk[i].first_blk); //Gets where all fats of the file are stored
            for(int j = 0; j < fat_indexes.size(); j++){
                fat[fat_indexes[j]] = 0; //Nulls those scrubs
            }
            blk[i] = {0};
            disk.write(0, (uint8_t*)blk);
            disk.write(1, (uint8_t*)fat);
            break;
        }
    }
    return 0;
}

// append <filepath1> <filepath2> appends the contents of file <filepath1> to
// the end of file <filepath2>. The file <filepath1> is unchanged.
int
FS::append(std::string filepath1, std::string filepath2)
{
    std::cout << "FS::append(" << filepath1 << "," << filepath2 << ")\n";
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(0, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath1.c_str()) == 0){
            for (int j = 0; j < 64; j++){
                if (strcmp(blk[j].file_name, filepath2.c_str()) == 0){ //When it finds both files
                    std::string file1 = diskread(blk[i].first_blk); //Saves the content of file 1 and file 2
                    std::string file2 = diskread(blk[j].first_blk); 
                    diskwrite(file2.append(file1).c_str(), blk[j].first_blk); //Creates an entirely new file with the content of both files and starts at the same spot as file 2, not the best but it works.
                    blk[j].size += blk[i].size; //Adds together their size
                    disk.write(0, (uint8_t*)blk);
                    break;
                }
            }
            break;
        }
    }
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
