#include <iostream>
#include "fs.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>

FS::FS(){
    std::cout << "FS::FS()... Creating file system\n";
    cwd.first_blk = 0;
    strcpy(cwd.file_name, "/");
    disk.read(1, (uint8_t*)fat);
}

FS::~FS(){

}

//Cp new name wierd

//returns bool array of (execute, write, read)
std::vector<bool>
FS::chmod_translate(std::string file_name){
    std::vector<bool> ex_wr_re = {false, false, false};
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, file_name.c_str()) == 0){ //finds the file with the right name
            int total_rights = blk[i].access_rights;

            if(total_rights >= 4){
                ex_wr_re[2] = true;
                total_rights -= 4;
            }

            if(total_rights >= 2){
                ex_wr_re[1] = true;
                total_rights -= 2;
            }
            
            if(total_rights >= 1){
                ex_wr_re[0] = true;
                total_rights -= 1;
            }
        }
    }

    return ex_wr_re;
}

// Travels to the specified path if read access, returns if it has write and execute access to the folder
std::vector<int> 
FS::traveller(std::string path){
    std::vector<std::string> separate = separate_path(path);
    int pos = 0;
    int old_cwd = cwd.first_blk;
    int write_access = 1;
    for(std::string i : separate){
        pos += 1;
        if (i == ""){
            cwd.first_blk = 0;
        }
        else {
            if (dir_exists(i) && chmod_translate(i)[2]){
                if (!chmod_translate(i)[1] || !chmod_translate(i)[0]){
                    write_access = 0;
                }
                cd(i, false); // false == no traveler extravaganza
            }
            else if(pos == separate.size()){
                //we're on the last one (checking if file exists latr)
                return {1, write_access}; //no wrong
            }
            else{
                std::cout << "Path not found!\n";
                cwd.first_blk = old_cwd;
                return {-1, write_access};
            }
        }
    }
    return {0, write_access};
}

//Separates the filepath string into its components
std::vector<std::string>
FS::separate_path(std::string filepath_string){
    std::stringstream filestream;
    filestream.str(filepath_string);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(filestream, segment, '/')){
        seglist.push_back(segment);
    }
    return seglist;
}

//Checks if a filename exists in the directory
bool
FS::exists(std::string filename){
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Writes it to the root directory
    for (int j = 0; j < 64; j++){
        if (strcmp(blk[j].file_name, filename.c_str()) == 0){
            return true;
        }
    }
    return false;
}

const int DIRECTORY = 49;

//Check if a directory exists in a directory
bool
FS::dir_exists(std::string dirname){
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Writes it to the root directory
    for (int j = 0; j < 64; j++){
        if ((strcmp(blk[j].file_name, dirname.c_str()) == 0)){
            if(blk[j].type == DIRECTORY){
                return true;
            }
        }
    }
    return false;
}

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

//Check if a filename is valid/legal
bool FS::valid_name(std::string name){
    if (name.find('/') < name.length()){
        std::cout << "Invalid filename!\n";
        return false;
    }
    return true;
}

//Returns the absolute path of the cwd
std::string
FS::absolute_path(){
    std::string full_path;

    int old_cwd = cwd.first_blk;
    bool is_in_root = false;

    while(exists((std::string)"..") && !is_in_root){
        int old_blk = cwd.first_blk;
        cd((std::string)"..");
        dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
        disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the directory
        for(int i=0; i < 64; i++){
            if (old_blk == blk[i].first_blk){
                if (blk[i].first_blk == 0){
                    is_in_root = true;
                }
                else{
                    std::string new_path_name = blk[i].file_name; // a
                    new_path_name.append((std::string)"/"); // a/
                    new_path_name.append(full_path); // a/what_it_was_before
                    full_path = new_path_name;
                }
                break;
            }
        }
    }
    std::string new_path_name = ""; // a
    new_path_name.append((std::string)"/"); // a/
    new_path_name.append(full_path); // a/what_it_was_before
    full_path = new_path_name;
    cwd.first_blk = old_cwd;
    return full_path;
}

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

    for (int i = 0; i <= blocks; i++){ //Loops for every block necessary to write to
        std::string partition = "";
        if (i == blocks){ 
            //IF LAST BLOCK
            fat[fat_indexes[i]] = -1; //EOF
            partition.append(input.substr(4096*i, size % 4096)); //Writes the last bits of the input to partition
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
FS::format(){
    uint8_t *blk = (uint8_t*)calloc(BLOCK_SIZE, sizeof(uint8_t*)); //Creates an empty block
    disk.write(cwd.first_blk, blk); //Writes it to the root directory
    memset(fat, 0, sizeof(fat)); //Resets the local fat variable
    fat[0] = -1; //Sets initial values
    fat[1] = -1;
    disk.write(1, (uint8_t*)fat); //Writes local fat to the disk
    
    
    dir_entry *old_dir;
    old_dir = (dir_entry*)calloc(1, sizeof(dir_entry));
    old_dir->type = '1';
    strcpy(old_dir->file_name, "..");
    old_dir->access_rights = 7;
    old_dir->first_blk = cwd.first_blk;
    disk.write(0, (uint8_t*)old_dir);


    std::cout << "FS::format()\n";
    return 0;
}

// create <filepath> creates a new file on the disk, the data content is
// written on the following rows (ended with an empty row)
int
FS::create(std::string filepath) {
    int old_cwd = cwd.first_blk;
    std::vector<int> valid_path = traveller(filepath); //-1 if not a real path, 0 if directory aka empty, 1 if file, 
    if(valid_path[0] != 1 || valid_path[1] == 0){ //if wrong path, we break or if its a dir
        std::cout << "Either the path couldn't be accessed or you don't have write access to the folder!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    filepath = separate_path(filepath).back().c_str();

    if(exists(filepath) || !valid_name(filepath)){
        std::cout << "Filename already exists. Use a different name and try again";
        cwd.first_blk = old_cwd;
        return -1;
    }
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
    new_file->type = '0';
    new_file->access_rights = 7;
    new_file->first_blk = diskwrite(input); //Writes the input to the disk and store where it begins
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    //Finds a spot for the file in the directory
    for (int j = 0; j < 64; j++){
        if (strcmp(blk[j].file_name, "") == 0){
            blk[j] = *new_file;
            disk.write(cwd.first_blk, (uint8_t*)blk);
            break;
        }
    }
    std::cout << "FS::create(" << filepath << ")\n";
    cwd.first_blk = old_cwd;
    return 0;
}

// cat <filepath> reads the content of a file and prints it on the screen
int
FS::cat(std::string filepath){
    int old_cwd = cwd.first_blk;
    int valid_path = traveller(filepath)[0];
    filepath = separate_path(filepath).back().c_str();
    if(valid_path != 1 || !chmod_translate(filepath)[2]){ //if wrong path, we break or if its a dir
        std::cout << "Either the file was not found or you don't have read access to it!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath.c_str()) == 0){ //Finds the file with the inputted filepath
            std::string content;
            dir_entry file = blk[i];
            content.append(diskread(file.first_blk));
            std::cout << content << "\n";
            break;
        }
    }
    cwd.first_blk = old_cwd;
    return 0;
}

// ls lists the content in the currect directory (files and sub-directories)
int
FS::ls(){
    std::cout << "FS::ls()\n";
    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    std::cout << "name" << "  |  " << "size" << "  |  " << "type" << "  |  " << "access" << "\n";
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, "") != 0){ //List every filename in directory which is not null
            std::vector<bool> chmod_access = chmod_translate(blk[i].file_name);
            std::string chmod_char = "";
            if(chmod_access[2]){
                chmod_char.append("r");
            }
            else{
                chmod_char.append("-");
            }
            
            if(chmod_access[1]){
                chmod_char.append("w");
            }
            else{
                chmod_char.append("-");
            }
            if(chmod_access[0]){
                chmod_char.append("x");
            }
            else{
                chmod_char.append("-");
            }    
            std::cout << blk[i].file_name << "   " << blk[i].size << "   " << blk[i].type << "   " << chmod_char << "\n";
        }
    }
    return 0;
}

// cp <sourcepath> <destpath> makes an exact copy of the file
// <sourcepath> to a new file <destpath>
int
FS::cp(std::string sourcepath, std::string destpath){
    std::cout << "FS::cp(" << sourcepath << "," << destpath << ")\n";
    
    int old_cwd = cwd.first_blk;

    std::vector<int> valid_path = traveller(sourcepath);

    if (valid_path[1] == 0){
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    sourcepath = separate_path(sourcepath).back().c_str();

    if (!(valid_path[0] == 1 && exists(sourcepath)) || !chmod_translate(sourcepath)[2]){
        std::cout << "Either the source file does not exist found or you don't have write accesss to it!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    int source_cwd = cwd.first_blk;

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads

    cwd.first_blk = old_cwd;
    
    valid_path = traveller(destpath);

    if (valid_path[1] == 0){
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    destpath = separate_path(destpath).back().c_str();
    
    if (exists(destpath) || valid_path[0] != 1){ //Travels to destpath. if is file and doesn not exists (not) == om den inte ar tom
        std::cout << "Cannot find destination\n";
        cwd.first_blk = old_cwd;
        return -1;
    }

    dir_entry *new_blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)new_blk); //Reads in the new directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, sourcepath.c_str()) == 0){ //finds the file with the source name
            if(valid_path[0] == -1){ //if wrong path, we break
                break;
            }
            //Creates the new directory entry
            dir_entry *new_file;
            new_file = (dir_entry*)malloc(sizeof(dir_entry));
            strcpy(new_file->file_name, destpath.c_str());
            
            new_file->size = blk[i].size;
            new_file->type = blk[i].type;
            new_file->access_rights = blk[i].access_rights;
            new_file->first_blk = diskwrite(diskread(blk[i].first_blk)); //Reads the data from the old file, writes the data from the new file and saves where it starts, how neat aint that (genuint stolt över detta)
            //Finds a spot for the file in the *current* directory
            disk.read(cwd.first_blk, (uint8_t*)blk);
            for (int k = 0; k < 64; k++){
                if (strcmp(blk[k].file_name, "") == 0){
                    blk[k] = *new_file;
                    disk.write(cwd.first_blk, (uint8_t*)blk);
                    break;
                }
            }
            break;
        }
    }
    //return to the original
    cwd.first_blk = old_cwd;
    return 0;
}

// mv <sourcepath> <destpath> renames the file <sourcepath> to the name <destpath>,
// or moves the file <sourcepath> to the directory <destpath> (if dest is a directory)
int
FS::mv(std::string sourcepath, std::string destpath){
    std::cout << "FS::mv(" << sourcepath << "," << destpath << ")\n";

    int old_cwd = cwd.first_blk;

    std::vector<int> valid_path = traveller(sourcepath);

    if (valid_path[1] == 0){
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    sourcepath = separate_path(sourcepath).back().c_str();

    if (!(valid_path[0] == 1 && exists(sourcepath)) || !chmod_translate(sourcepath)[2]){
        std::cout << "Either the source file does not exist found or you don't have write accesss to it!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    int source_cwd = cwd.first_blk;

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads

    cwd.first_blk = old_cwd;
    
    valid_path = traveller(destpath);

    if (valid_path[1] == 0){
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    destpath = separate_path(destpath).back().c_str();
    
    if (exists(destpath) || valid_path[0] != 1){ //Travels to destpath. if is file and doesn not exists (not) == om den inte ar tom
        std::cout << "Cannot find destination\n";
        cwd.first_blk = old_cwd;
        return -1;
    }

    dir_entry *new_blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)new_blk); //Reads in the new directory

    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, sourcepath.c_str()) == 0){ //Finds the source file
            for (int k = 0; k < 64; k++){
                if (strcmp(new_blk[k].file_name, "") == 0){
                    new_blk[k] = blk[i];
                    strcpy(new_blk[k].file_name, destpath.c_str());

                    if (source_cwd == cwd.first_blk){ //if we are copying localy
                        new_blk[i] = {0};
                        disk.write(cwd.first_blk, (uint8_t*)new_blk); //writes over the new one with a empty hole
                    }
                    else{
                        blk[i] = {0};
                        disk.write(source_cwd, (uint8_t*)blk); //remove the previous dir_entry
                        disk.write(cwd.first_blk, (uint8_t*)new_blk);
                    }
                    break;
                }
            }
        }
    }
    cwd.first_blk = old_cwd;
    return 0;
}

// rm <filepath> removes / deletes the file <filepath>
int
FS::rm(std::string filepath){
    std::cout << "FS::rm(" << filepath << ")\n";
    
    int old_cwd = cwd.first_blk;
    std::vector<int> valid_path = traveller(filepath);

    if (valid_path[1] == 0){
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    } //-1 if not a real path, 0 if directory aka empty, 1 if file, 
    if(valid_path[0] != 1){ //if wrong path, we break or if its a dir
        std::cout << "Specified path was not a file!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    filepath = separate_path(filepath).back().c_str();

    if(!(exists(filepath)) || !chmod_translate(filepath)[1]){
        std::cout << "Either the specified file does not exists or you don't have write access to it.\n";
        cwd.first_blk = old_cwd;
        return -1;
    }

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath.c_str()) == 0){
            std::vector<int> fat_indexes = get_fats(blk[i].first_blk); //Gets where all fats of the file are stored
            for(int j = 0; j < fat_indexes.size(); j++){
                fat[fat_indexes[j]] = 0; //Nulls those scrubs
            }
            blk[i] = {0};
            disk.write(cwd.first_blk, (uint8_t*)blk);
            disk.write(1, (uint8_t*)fat);
            break;
        }
    }
    cwd.first_blk = old_cwd;
    return 0;
}

// append <filepath1> <filepath2> appends the contents of file <filepath1> to
// the end of file <filepath2>. The file <filepath1> is unchanged.
int
FS::append(std::string filepath1, std::string filepath2){
    
    std::cout << "FS::append(" << filepath1 << "," << filepath2 << ")\n";
    int old_cwd = cwd.first_blk;

    int valid_path = traveller(filepath1)[0];
    filepath1 = separate_path(filepath1).back().c_str();

    if (!(valid_path == 1 && exists(filepath1)) || !chmod_translate(filepath1)[2]){ //Travels to filepath1
        std::cout << "Either the source file cannot be found or you don't have read access to it!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    int source_cwd = cwd.first_blk;

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads

    cwd.first_blk = old_cwd;
    
    std::vector<int> valid_path_2 = traveller(filepath2);

    if (valid_path_2[1] == 0){
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    } //-1 if not a real path, 0 if directory aka empty, 1 if file, 

    filepath2 = separate_path(filepath2).back().c_str();

    if (!(valid_path_2[0] == 1 && exists(filepath2)) || !chmod_translate(filepath2)[1]){ //Travels to destpath. if is file and doesn not exists (not) == om den inte ar tom
        std::cout << "Either the destination file cannot be found or you don't have write access to it!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }

    dir_entry *new_blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)new_blk); //Reads in the new directory
    filepath2 = separate_path(filepath2).back().c_str();
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath1.c_str()) == 0){
            for (int j = 0; j < 64; j++){
                if (strcmp(new_blk[j].file_name, filepath2.c_str()) == 0){ //When it finds both files
                    std::string file1 = diskread(blk[i].first_blk); //Saves the content of file 1 and file 2
                    std::string file2 = diskread(new_blk[j].first_blk); 
                    diskwrite(file2.append(file1).c_str(), new_blk[j].first_blk); //Creates an entirely new file with the content of both files and starts at the same spot as file 2, not the best but it works.
                    new_blk[j].size += blk[i].size; //Adds together their size
                    disk.write(cwd.first_blk, (uint8_t*)new_blk);
                    break;
                }
            }
            break;
        }
    }
    cwd.first_blk = old_cwd;
    return 0;
}

// mkdir <dirpath> creates a new sub-directory with the name <dirpath>
// in the current directory
int FS::mkdir(std::string dirpath){
    std::cout << "FS::mkdir(" << dirpath << ")\n";
    int old_cwd = cwd.first_blk;
    std::vector<int> valid_path = traveller(dirpath);

    if(valid_path[1] == 0){ //if wrong path, we break or if its a dir
        std::cout << "Write access denied\n";
        cwd.first_blk = old_cwd;
        return -1;
    } //-1 if not a real path, 0 if directory aka empty, 1 if file, 
    dirpath = separate_path(dirpath).back().c_str();

    if(valid_path[0] != 1 || exists(dirpath)){
        std::cout << "Path to directory occupied by enemy forces, we suggest starting a war\n";
        cwd.first_blk = old_cwd;
        return -1;
    }
    
    //Creates the new directory entry
    dir_entry *new_file;
    new_file = (dir_entry*)calloc(1, sizeof(dir_entry));
    strcpy(new_file->file_name, dirpath.c_str());
    new_file->size = 0;
    new_file->type = '1';
    new_file->access_rights = 7;
    new_file->first_blk = diskwrite(""); //Writes the input to the disk and store where it begins

    dir_entry *old_dir;
    old_dir = (dir_entry*)calloc(1, sizeof(dir_entry));
    old_dir->type = '1';
    strcpy(old_dir->file_name, "..");
    old_dir->access_rights = 7;
    old_dir->first_blk = cwd.first_blk;
    disk.write(new_file->first_blk, (uint8_t*)old_dir);

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    //Finds a spot for the file in the directory
    for (int j = 0; j < 64; j++){
        if (strcmp(blk[j].file_name, "") == 0){
            blk[j] = *new_file;
            disk.write(cwd.first_blk, (uint8_t*)blk);
            break;
        }
    }
    cwd.first_blk = old_cwd;
    return 0;
}

// cd <dirpath> changes the current (working) directory to the directory named <dirpath>
int FS::cd(std::string dirpath, bool first){
    if(first){
        std::cout << "FS::cd(" << dirpath << ")\n";
        traveller(dirpath);
        return 0;
    }

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, dirpath.c_str()) == 0){
            cwd.first_blk = blk[i].first_blk;
            strcpy(cwd.file_name, dirpath.c_str());
        }
    }
    return 0;
}

// pwd prints the full path, i.e., from the root directory, to the current
// directory, including the currect directory name
int
FS::pwd(){
    std::cout << absolute_path() << "\n";
    return 0;
}

// chmod <accessrights> <filepath> changes the access rights for the
// file <filepath> to <accessrights>.
int
FS::chmod(std::string accessrights, std::string filepath){
    std::cout << "FS::chmod(" << accessrights << "," << filepath << ")\n";
    int old_cwd = cwd.first_blk;
    int valid_path = traveller(filepath)[0]; //-1 if not a real path, 0 if directory aka empty, 1 if file, 
    filepath = separate_path(filepath).back().c_str();
    std::cout << filepath << "   "   << valid_path << "\n";
    if(valid_path == 0){
        cd("..");
    }
    if(valid_path == -1 || !exists(filepath)){
        std::cout << "File not found!\n";
        cwd.first_blk = old_cwd;
        return -1;
    }

    dir_entry *blk = (dir_entry*)calloc(disk.get_no_blocks(), sizeof(dir_entry)); //Creates an empty block
    disk.read(cwd.first_blk, (uint8_t*)blk); //Reads in the root directory
    
    for (int i = 0; i < 64; i++){
        if (strcmp(blk[i].file_name, filepath.c_str()) == 0){
            blk[i].access_rights = atoi(accessrights.c_str());
            disk.write(cwd.first_blk, (uint8_t*)blk);
            break;
        }
    }
    cwd.first_blk = old_cwd;
    return 0;
}
