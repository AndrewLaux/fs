/******************************************************************************
Written by: Andrew Laux
Test driver for file system.
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "fs.h"
using namespace std;

int main() {

    // Create file system.
    char* drive_name = (char*)"E.drive";
    int res = make_fs(drive_name);
    if(res == 0) printf("[x] Created %s successfully.\n", drive_name);
    else{
        printf("[ ] Make filesystem returned %i\n", res);    
    } 

    // Mount file system.
    res = mount_fs(drive_name);
    if(res == 0) printf("[x] Mounted %s successfully.\n", drive_name);
    else{
        printf("[ ] Mounting returned %i\n", res);    
    } 

    // Unmount system.
    res = umount_fs(drive_name);
    if(res == 0) printf("[x] Unmounted %s successfully.\n", drive_name);
    else{
        printf("[ ] Unmounting returned %i\n", res);    
    }

    // Create file system.
    drive_name = (char*)"A.drive";
    res = make_fs(drive_name);
    if(res == 0) printf("[x] Created %s successfully.\n", drive_name);
    else{
        printf("[ ] Make filesystem returned %i\n", res);    
    }

    // Mount file system.
    res = mount_fs(drive_name);
    if(res == 0) printf("[x] Mounted %s successfully.\n", drive_name);
    else{
        printf("[ ] Mounting returned %i\n", res);    
    }

    // Bad unmount system.
    res = umount_fs((char*)"E.drive");
    if(res != 0) printf("[x] Bad unmount %s rejected successfully.\n", (char*)"E.drive");
    else{
        printf("[ ] Bad unmounting returned %i\n", res);    
    }

    // Check file count.
    if (fs_test::get_count() == 0) printf("[x] Found correct file count.\n");
    else {
        printf("[ ] %i files found, should be none.\n");
    }

    // Check create files.
    char* filename = (char*)"hello.txt";
    res = -1;
    if(fs_create(filename) == 0) {
        if(strncmp(fs_test::get_address(0).name, filename, 16) == 0) {
            if(fs_test::get_count() == 1) res = 0;
        }
    }
    if(res == 0) printf("[x] %s created successfully.\n", filename);
    else {
        printf("[ ] Error creating %s.\n", filename);
    }
    
    // Create some bad files.
    filename = (char*)"helllllllllllllllllllllllllllllllllllllllllllllo.txt";
    res = -1;
    if(fs_create(filename) == 0) {
        if(strncmp(fs_test::get_address(0).name, filename, 16) == 0) {
            if(fs_test::get_count() == 2) res = 0;
        }
    }
    if(res == -1) printf("[x] Bad filename ignored successfully.\n");
    else {
        printf("[ ] Error: created %s.\n", filename);
    }

    // Create some more good files.
    filename = (char*)"truth.txt";
    res = -1;
    if(fs_create(filename) == 0) {
        if(strncmp(fs_test::get_address(1).name, filename, 16) == 0) {
            if(fs_test::get_count() == 2) res = 0;
        }
    }
    if(res == 0) printf("[x] Created %s successfully.\n", filename);
    else {
        printf("[ ] Error creating %s.\n", filename);
    } 

    // Create some more good files.
    filename = (char*)"ministry.txt";
    res = -1;
    if(fs_create(filename) == 0) {
        if(strncmp(fs_test::get_address(2).name, filename, 16) == 0) {
            if(fs_test::get_count() == 3) res = 0;
        }
    }
    if(res == 0) printf("[x] Created %s successfully.\n", filename);
    else {
        printf("[ ] Error creating %s.\n", filename);
    } 

    // Create some more good files.
    filename = (char*)"bathory.bat";
    res = -1;
    if(fs_create(filename) == 0) {
        if(strncmp(fs_test::get_address(3).name, filename, 16) == 0) {
            if(fs_test::get_count() == 4) res = 0;
        }
    }
    if(res == 0) printf("[x] Created %s successfully.\n", filename);
    else {
        printf("[ ] Error creating %s.\n", filename);
    } 

    // Create duplicate file.
    res = -1;
    if(fs_create(filename) == 0) {
        if(strncmp(fs_test::get_address(4).name, filename, 16) == 0) {
            if(fs_test::get_count() == 5) res = 0;
        }
    }
    if(res == -1) printf("[x] Duplicate filename ignored successfully.\n");
    else {
        printf("[ ] Error: created duplicate %s.\n", filename);
    }

    // Check file count.
    if (fs_test::get_count() == 4) printf("[x] Found correct file count 4.\n");
    else {
        printf("[ ] %i files found, should be none.\n");
    }

    // Print Files.
    printf("\nFiles found on disk");
    printf("\n-------------------\n");
    for (int i = 0; i < 64; i++) {
        if(strcmp(fs_test::get_address(i).name, (char*)"") != 0){
            address temp = fs_test::get_address(i);
            printf("%s %i \n", temp.name, temp.head);
        }
    }
    printf("\n");

    // Open file.
    int des1 = fs_open((char*)"hello.txt");
    if (des1 >= 0) printf("[x] Got file descriptor %i successfully.\n", des1);
    else {
        printf("[ ] Error: Not able to open file.\n");
    }

    // Open same file.
    int des2 = fs_open((char*)"hello.txt");
    if (des2 >= 0) printf("[x] Got file descriptor %i successfully.\n", des2);
    else {
        printf("[ ] Error: Not able to open file.\n");
    }

    // Open new file.
    int des3 = fs_open((char*)"truth.txt");
    if (des3 >= 0) printf("[x] Got file descriptor %i successfully.\n", des3);
    else {
        printf("[ ] Error: Not able to open file.\n");
    }

    //Print descriptor table.
    printf("\nDescriptor Table");
    printf("\n----------------\n");

    for(int d = 0; d < 32; d++) {
        if(fs_test::get_descriptor(d).opened == true) {
            descriptor temp = fs_test::get_descriptor(d);
            address add = fs_test::get_address(temp.directory_num);
            printf("%i - %s @ block %i + %i, size: %i\n",
            d, add.name, temp.offset_block, temp.offset_byte, add.filesize);
        }
    }

    // Try out a write;
    int des = 0;
    int nbyte = 64;
    printf("\n");
    char* text = (char*)"Hello my sweet little wagie <3!";
    char wrt_buf[100];
    strncpy(wrt_buf, text, 99);
    int wrote = fs_write(des, wrt_buf, nbyte);
    if(wrote >= 0) printf("[X] fs_write returned successfully.\n");
    else {
        printf("[ ] Error: writing to file failed (%i).\n", wrote);
    }
    if(fs_test::get_address(
    fs_test::get_descriptor(des).directory_num).filesize == nbyte)
        printf("[x] Filesize of hello.txt updated successfully.\n");
    else {
        printf("[ ] Found wrong filesize.\n", wrote);
    }

    // Close a fildes
    int close_des = 2;
    if(fs_close(close_des) == 0) {
        printf("[x] Closed descriptor %i successfully.\n", close_des);
    }
    else {
        printf("[ ] Failed to close.\n");
    }

    
    // Open same file.
    int des4 = fs_open((char*)"bathory.bat");
    if (des4 >= 0) printf("[x] Got file descriptor %i successfully.\n", des4);
    else {
        printf("[ ] Error: Not able to open file.\n");
    }

    // Open new file.
    int des5 = fs_open((char*)"truth.txt");
    if (des5 >= 0) printf("[x] Got file descriptor %i successfully.\n", des5);
    else {
        printf("[ ] Error: Not able to open file.\n");
    }

    //Print descriptor table.
    printf("\n\nDescriptor Table");
    printf("\n----------------\n");

    for(int d = 0; d < 32; d++) {
        if(fs_test::get_descriptor(d).opened == true) {
            descriptor temp = fs_test::get_descriptor(d);
            address add = fs_test::get_address(temp.directory_num);
            printf("%i - %s @ block %i + %i, size: %i\n",
            d, add.name, temp.offset_block, temp.offset_byte, add.filesize);
        }
    }

    // Try to delete bathory.bat:
    printf("\n");
    char* name_to_delete = (char*)"bathory.bat";
    int del_res = fs_delete(name_to_delete);
    if(del_res == -1) {
        printf("[x] Protected open file %s from deletion.\n", name_to_delete);
    } 
    else {
        printf("[] Error deleted open file.\n");
    }

    // Close and delete
    del_res = fs_close(2);
    if (del_res == 0) {
        printf("[x] Closed bathory.bat successfully.\n");
        del_res = fs_delete((char*)"bathory.bat");
        if (del_res == 0) printf("[x] Deleted %s successfully.\n");
        else printf("[ ] Failed to delete.");
    }
    else printf("[ ] Could not close file.");


    // Print Files.
    printf("\nFiles found on disk");
    printf("\n-------------------\n");
    for (int i = 0; i < 64; i++) {
        if(strcmp(fs_test::get_address(i).name, (char*)"") != 0){
            address temp = fs_test::get_address(i);
            printf("%s %i \n", temp.name, temp.head);
        }
    }
    printf("\n"); 

    // Read block 1
    char block1[BLOCK_SIZE];
    
    // Read from fd 1;
    char read_in[100];
    int read_des = 1;
    int read_res = fs_read(read_des, read_in, 100);
    if(read_res > 0) printf("[x] Read bytes from fd %i.\n", read_res, read_des);
    else printf("[ ] Failed to read any bytes.\n");
    if(strncmp(read_in, text, 100) == 0) printf("[x] MSG: %s\n", read_in);
    else printf("[ ] Wrong msg read from file.\n");
    if(read_res == 64) printf("[x] Read the correct amount of bytes from %i.\n",
    read_des);
    else if(read_res > 64) printf("[ ] Read past EOF.\n");  


    


    return 0;
}