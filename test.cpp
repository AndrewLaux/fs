/******************************************************************************
Written by: Andrew Laux
Test driver for file system.
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "fs.h"
using namespace std;

void tests();

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

    int u = umount_fs(drive_name);
    if (u != -1) tests();


    return 0;
}

void tests() {

    printf("********************************\n");
    printf("********  Phase 2  *************\n");
    printf("********************************\n\n");

    char* disk = (char*)"G.drive";
    make_fs(disk);
    mount_fs(disk);
    fs_create((char*)"rs.txt");
    int fd0 = fs_open((char*)"rs.txt");
    int fd1 = fs_open((char*)"rs.txt");

    char r[BLOCK_SIZE];
    char s[BLOCK_SIZE];
    char q[BLOCK_SIZE];
    char t[BLOCK_SIZE];
    char read[BLOCK_SIZE];

    for(int i = 0; i < BLOCK_SIZE; i++) {
        r[i] = 'r';
        s[i] = 's';
        q[i] = 'q';
        t[i] = 't';
    }

    printf("Four blocks of S from fd1:\n");

    fs_write(fd1, s, BLOCK_SIZE);
    fs_write(fd1, s, BLOCK_SIZE);
    fs_write(fd1, s, BLOCK_SIZE);
    fs_write(fd1, s, BLOCK_SIZE);

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

    printf("\nTwo blocks of R from fd0:\n");

    fs_write(fd0, r, BLOCK_SIZE);
    fs_write(fd0, r, BLOCK_SIZE);

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

    fs_create((char*)"q.txt");
    int fd2 = fs_open((char*)"q.txt");
    fs_write(fd2, q, BLOCK_SIZE);

    printf("Create q, write block to fd2:\n");

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

    //Print data_table
    short* table = fs_test::get_table();
    for(int t = 0; t < 10; t++) {
        printf("[%i]", table[t]);
    }

    printf("\n\nTruncate fd0 by a block:");

    fs_truncate(fd0, BLOCK_SIZE);

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

    //Print data_table
    printf("\n\n");
    for(int t = 0; t < 10; t++) {
        printf("[%i]", table[t]);
    }


    // Check zeroed 
    block_read(2, read);
    if(read[0] == 0 && read[4095] == 0) {
        printf("\n[x] zeroed block 2.\n");
    } 

    block_read(0, read);
    if(read[0] == 'r' && read[4095] == 'r') {
        printf("\n[x] Block 0 reads r's.\n");
    }

    printf("\n\nCreate a new file in hole:");
    printf("\n\nWrite two blocks to t to fd3:\n");

    fs_create((char*)"t.txt");
    int fd3 = fs_open((char*)"t.txt");
    fs_write(fd3, t, BLOCK_SIZE);
    fs_write(fd3, t, BLOCK_SIZE);

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

    //Print data_table
    printf("\n\n");
    for(int t = 0; t < 10; t++) {
        printf("[%i]", table[t]);
    }

    // Try to read from apend fs.
    int read_fd = fs_read(fd0, read, BLOCK_SIZE);
    printf("\nRead %i bytes from fd0.\n", read_fd);
    read_fd = fs_read(fd1, read, BLOCK_SIZE);
    printf("\nRead %i bytes from fd1.\n", read_fd);

    printf("2 1/2 blocks to fd1.\n");

    fs_write(fd1, s, BLOCK_SIZE);
    fs_write(fd1, s, 2048);

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

    //Print data_table
    printf("\n\n");
    for(int t = 0; t < 10; t++) {
        printf("[%i]", table[t]);
    }

    char b0[BLOCK_SIZE];
    char b1[BLOCK_SIZE];
    char b2[BLOCK_SIZE];
    char b3[BLOCK_SIZE];
    char b4[BLOCK_SIZE];
    char b5[BLOCK_SIZE];
    char b6[BLOCK_SIZE];
    char b7[BLOCK_SIZE];
    char b8[BLOCK_SIZE];
    char b9[BLOCK_SIZE];
    block_read(0, b0);
    block_read(1, b1);
    block_read(2, b2);
    block_read(3, b3);
    block_read(4, b4);
    block_read(5, b5);
    block_read(6, b6);
    block_read(7, b7);
    block_read(8, b8);
    block_read(9, b9);
    printf("\n[%c , %c]\n", b0[0], b0[4095]);
    printf("[%c , %c]\n", b1[0], b1[4095]);
    printf("[%c , %c]\n", b2[0], b2[4095]);
    printf("[%c , %c]\n", b3[0], b3[4095]);
    printf("[%c , %c]\n", b4[0], b4[4095]);
    printf("[%c , %c]\n", b5[0], b5[4095]);
    printf("[%c , %c]\n", b6[0], b6[4095]);
    printf("[%c , %c]\n", b7[0], b7[4095]);
    printf("[%c , %c]\n", b8[0], b8[4095]);
    printf("[%c , %c]\n", b9[0], b9[4095]);

    printf("\n Truncating fd1.\n");
    fs_truncate(fd1, 500);
    
    block_read(0, b0);
    block_read(1, b1);
    block_read(2, b2);
    block_read(3, b3);
    block_read(4, b4);
    block_read(5, b5);
    block_read(6, b6);
    block_read(7, b7);
    block_read(8, b8);
    block_read(9, b9);
    printf("\n[%c , %c]\n", b0[0], b0[4095]);
    printf("[%c , %c]\n", b1[0], b1[4095]);
    printf("[%c , %c]\n", b2[0], b2[4095]);
    printf("[%c , %c]\n", b3[0], b3[4095]);
    printf("[%c , %c]\n", b4[0], b4[4095]);
    printf("[%c , %c]\n", b5[0], b5[4095]);
    printf("[%c , %c]\n", b6[0], b6[4095]);
    printf("[%c , %c]\n", b7[0], b7[4095]);
    printf("[%c , %c]\n", b8[0], b8[4095]);
    printf("[%c , %c]\n", b9[0], b9[4095]);

    fs_lseek(fd0, 0);
    fs_write(fd0, r, BLOCK_SIZE);

    fs_read(fd1, b0, BLOCK_SIZE);
    fs_read(fd1, b1, BLOCK_SIZE);

    printf("\n[%c , %c]\n", b0[0], b0[4095]);
    printf("[%c , %c]\n", b1[0], b1[4095]);


}