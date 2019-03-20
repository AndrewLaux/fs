#include "fs.h"

/*-----------------------------------------------------------------------------
 * Anonymous namespace
 * Provides scope needed for constants and file system memory.
 */
namespace  {

    // Constants.
    const int DATA_BLOCKS = 4096;                   // How many blocks for data.
    const int MAX_FILES = 64;                       // 32 files open at a time.
    char EMPTY_BLOCK[BLOCK_SIZE] = {0};       // Empty block definition.
    const int MAX_DES = 32;
    

    // Block locations.
    const int TABLE_START = DATA_BLOCKS;            
    const int DIRECTORY_START = DATA_BLOCKS + (int)ceil((sizeof(short)*DATA_BLOCKS)/BLOCK_SIZE);
    const int FILE_COUNT = DIRECTORY_START + 1;

    /******************************
     *       FILE SYSTEM MEM      *
     ******************************/

    // Holds the name of the file system that is mounted.
    char* fs_name = nullptr;

    // Each entry points to the next allocated block of mem.
    short data_table[DATA_BLOCKS];

    // Holds a copy of the file directory for mounted fs.
    address directory[MAX_FILES];

    // A vector of opened files.
    descriptor des_table[MAX_DES];

    // Hold number of files on disk.
    short file_count;

    /******************************
    *     Helper Definitions      *
    ******************************/

    void load_table() {
        int table_blocks = DIRECTORY_START - DATA_BLOCKS;
        char* buf = new char[table_blocks * BLOCK_SIZE];
        for( int b = 0; b < table_blocks; b++) {
            block_read(TABLE_START + b, &buf[b * BLOCK_SIZE]);
        }
        short* buf_ptr = (short*)buf;
        for( int i = 0; i < DATA_BLOCKS; i++) {
            data_table[i] = buf_ptr[i];
        }
        delete[] buf;
    }

    void save_table() {
        char* table_ptr = (char*)data_table;
        int table_blocks = DIRECTORY_START - DATA_BLOCKS;
        for(int b = 0; b < table_blocks; b++) {
            block_write(TABLE_START + b, &table_ptr[b * BLOCK_SIZE]);
        }
    }

    void load_directory() {
        char buf[BLOCK_SIZE] = {0};
        block_read(DIRECTORY_START, buf);
        address* adr_ptr = (address*)buf;
        for(int a = 0; a < MAX_FILES; a++) {
            directory[a] = adr_ptr[a];
        }
    }

    void load_count() {
        char buf[BLOCK_SIZE] = {0};
        block_read(FILE_COUNT, buf);
        short* count_ptr = (short*)buf;
        file_count = count_ptr[0];
    }

    void wipe_descriptors() {
        for (int i = 0; i < 32; i++) {
            des_table[i].opened = false;
        }
    }

    void save_directory() {
        char buf[BLOCK_SIZE] = {0};
        address* buf_ptr = (address*)buf;
        for(int a = 0; a < MAX_FILES; a++) {
            buf_ptr[a] = directory[a];
        }
        block_write(DIRECTORY_START, buf);
    }

    void save_count() {
        char buf[BLOCK_SIZE] = {0};
        short* buf_ptr = (short*)buf;
        buf_ptr[0] = file_count;
        block_write(FILE_COUNT, buf);
    }

    int num_allocated(short head, int count = 0) {
        if (head == -1) return count;
        else if (data_table[head] == 0) return -1;
        else return num_allocated(data_table[head], count + 1);
    }

    int last_block_from_head(short head) {
        if (head == -1 || data_table[head] == 0) return -1;
        else if(data_table[head] == -1) return head;
        else return last_block_from_head(data_table[head]); 
    }

    int get_block_by_offset(short head, int offset) {
        if (offset == 0) return head;
        else if(head == -1 || offset < 0) return -1;
        else return get_block_by_offset(data_table[head], offset - 1);
    }

    int trunc_blocks(short head, const int last_offset, int off = 0) {
        if(head < -1 || head > 4095) return -1; // We lost control!
        if(head < 0) return off;
        int next = data_table[head]; 
        if(off == last_offset) data_table[head] = -1;
        if(off > last_offset){
            data_table[head] = 0;
            block_write(head, EMPTY_BLOCK);
        }  
        return trunc_blocks(next, last_offset, off + 1);
    }


}








/*****************************************************************************
 *   Definitions: Interface                                                  *
 *****************************************************************************/







/*****************************************************************************/
/* Definition - int make_fs -------------------------------------------------*/
int make_fs(char *disk_name) {
    return make_disk(disk_name);
} /*-------------------------------------------------------------------------*/








/*****************************************************************************/
/* Definition - int mount fs ------------------------------------------------*/
int mount_fs(char *disk_name) {
    if(fs_name != nullptr) return -1;
    if(open_disk(disk_name) == -1) return -1;
    else fs_name = disk_name;
    load_directory();
    load_table();
    load_count();
    wipe_descriptors();
    return 0;     
} /*-------------------------------------------------------------------------*/ 








/*****************************************************************************/
/* Definition - int umount_fs -----------------------------------------------*/
int umount_fs(char *disk_name) {
    if(strcmp(fs_name, disk_name) != 0) return -1;
    if(close_disk() == -1) return -1;
    else fs_name = nullptr;
    return 0;
} /*-------------------------------------------------------------------------*/








/*****************************************************************************/
/* Definition - int fs_create -----------------------------------------------*/
int fs_create(char *name) {

    // validate candidate file name:
    int result = -1;
    if(strcmp(name, (char*)"") == 0) return -1;
    if(strnlen(name, 16) == 16) return -1;
    if(file_count >= 64) return -1;

    // Disallow duplicate names:
    for(int a = 0; a < MAX_FILES; a++) {
        if(strcmp(directory[a].name, name) == 0) return -1;
    }

    // Find next empty directory slot:
    for(int a = 0; a < MAX_FILES; a++) {
        if(strcmp(directory[a].name, (char*)"") == 0) {
            strncpy(directory[a].name, name, 16);
            directory[a].head = -1;
            directory[a].filesize = 0;
            result = 0;
            break;
        }
    }

    // Update disk and memory:
    if(result == 0) {
        save_directory();
        file_count++;
        save_count();
    } 
    return result;
} /*-------------------------------------------------------------------------*/








/*****************************************************************************/
/* Definition - int fs_open -------------------------------------------------*/
int fs_open(char *name) {
    bool found = false;
    int dir_entry;
    int des;

    // Search for file in directory:
    for(int a = 0; a < MAX_FILES; a++) {
        if(strcmp(directory[a].name, name) == 0) {
            found = true;
            dir_entry = a;
        }
    }

    // If not found return fail flag:
    if(!found) return -1;

    // Else look for a free descriptor and set it:
    else {
        found = false;
        for(int d = 0; d < MAX_DES; d++) {
            if (des_table[d].opened == false) {
                found = true;
                des_table[d].directory_num = dir_entry;
                des_table[d].offset_block = 0;
                des_table[d].offset_byte = 0;
                des_table[d].opened = true;
                des = d;
            }
            if(found) break;
        }
    }

    // Return if table is full;
    if(!found) return -1;
    return des;  
} /*-------------------------------------------------------------------------*/








/*****************************************************************************/
/* Definition - fs_write ----------------------------------------------------*/
int fs_write(int fildes, void* buf, size_t nbyte) {

    // Validate filedes:
    if(!des_table[fildes].opened) return -1;
    descriptor temp = des_table[fildes];

    // If there are no bytes to write
    if(!nbyte) return 0;

    // File needs a head block
    if(directory[temp.directory_num].head == -1) {
        bool found = false;
        for(int b = 0; b < DATA_BLOCKS; b++) {
            if(data_table[b] == 0) {
                found = true;
                directory[temp.directory_num].head = b;
                data_table[b] = -1;
                break;
            }
        }
        if(found == false) return nbyte;
    }

    

    // Compute needed space for offset + blocks to be written:
    int required_bytes = temp.offset_block * BLOCK_SIZE + 
    temp.offset_byte + nbyte;
    int required_blocks = required_bytes / BLOCK_SIZE;
    if(required_bytes % BLOCK_SIZE != 0) required_blocks++;

    // Allocate if needed needed:
    int need_to_allocate = required_blocks - 
    num_allocated(directory[temp.directory_num].head);
    if (need_to_allocate > 0) {
        int last_block = last_block_from_head(directory[temp.directory_num].head);
        for(int n = 0; n < need_to_allocate; n++) {
            for(int i = 0; i < DATA_BLOCKS; i++) {
                if(data_table[i] == 0) {
                    data_table[last_block] = i;
                    data_table[i] = -1;
                    last_block = i;
                    break;
                }
            }
        }
    }

    // Write data block by block;
    char* buf_ptr = (char*)buf;
    int head = get_block_by_offset(directory[temp.directory_num].head, 
    temp.offset_block);
    int bytes_wrote = 0;

    // Each block:
    do {

        // Copy old block
        char wrt_buf[BLOCK_SIZE];
        block_read(head, wrt_buf);


        for(int b = temp.offset_byte; b < BLOCK_SIZE; b++) {
            if(nbyte) {
                wrt_buf[b] = *buf_ptr;
                buf_ptr++;
                temp.offset_byte++;
                nbyte--;
                bytes_wrote++;
            } 

        }

        //Prep for next block:
        block_write(head, wrt_buf);
        head = data_table[head];
        if(temp.offset_byte == 4096) {
            temp.offset_block++;
            temp.offset_byte = 0;
        }
    } while(head != -1);


    // Save metadata to disk.
    des_table[fildes] = temp;
    directory[temp.directory_num].filesize = temp.offset_block * BLOCK_SIZE
    + temp.offset_byte;
    save_table();
    save_directory();

    // Return bytes not written.
    return bytes_wrote;

} /*--------------------------------------------------------------------------*/








/*****************************************************************************/
/* Definition - fs_close ----------------------------------------------------*/
int fs_close(int fildes) {

    // Validate filedes:
    if(!des_table[fildes].opened) return -1;
    else {
        des_table[fildes].opened = false;
        return 0;
    }
}
/*---------------------------------------------------------------------------*/








/*****************************************************************************/
/* Definition - fs_get_filesize ---------------------------------------------*/
int fs_get_filesize(int fildes) {

    // Validate filedes:
    if(!des_table[fildes].opened) return -1;
    else {
        return directory[des_table[fildes].directory_num].filesize;
    }
}
/*---------------------------------------------------------------------------*/









/*****************************************************************************/
/* Definition - fs_delete ---------------------------------------------------*/
int fs_delete(char* name) {

    // No open descriptors:
    for(int d = 0; d < MAX_DES; d++) {
        if(des_table[d].opened) {
            char* fn = directory[des_table[d].directory_num].name;
            if (strncmp(fn, name, 16) == 0) return -1;
        }
    }

    // No empty names:
    if (strncmp(name, "", 16) == 0) return -1;

    // Validate filename:
    int found = -1;
    for(int a = 0; a < MAX_FILES; a++) {
        if(strncmp(name, directory[a].name, 16) == 0) {
            found = a;
            break;
        }
    }
    if(found == -1) return -1;

    // Free blocks:
    short next = directory[found].head;
    while(next != -1) {
        short cur = data_table[next];
        data_table[next] = 0;
        block_write(next, EMPTY_BLOCK);
        next = data_table[cur];
    }

    // Delete directory entry:
    strncpy(directory[found].name, "", 16);
    directory[found].filesize = 0;
    directory[found].head = -1;
    file_count--;

    // Save to disk:
    save_count;
    save_directory;
    save_table;

    // Fin.
    return 0;

}
/*---------------------------------------------------------------------------*/









/*****************************************************************************/
/* Definition - fs_read -----------------------------------------------------*/
int fs_read(int fildes, void* buf, size_t nbyte) {

    // Validate filedes:
    if(!des_table[fildes].opened) return -1;
    descriptor temp = des_table[fildes];

    // If there no bytes are to be read:
    if(nbyte < 1) return 0;

    // Prepare to read:
    int bytes_read = 0;
    char* buf_ptr = (char*)buf;
    int head = get_block_by_offset(directory[temp.directory_num].head, 
    temp.offset_block);

    // If there are no bytes to read:
    if(head < 0) return 0;

    // Compute possible bytes:
    int bytes_possible = directory[temp.directory_num].filesize - 
    temp.offset_block * BLOCK_SIZE - temp.offset_byte;
    if(bytes_possible < 1) return 0;

    // For each block:
    char temp_block[BLOCK_SIZE];
    do {

        // Load block from disk;
        block_read(head, temp_block);

        //Perform read:
        for(int b = temp.offset_byte; b < BLOCK_SIZE; b++) {
            if(nbyte && bytes_read < bytes_possible) {
                buf_ptr[0] = temp_block[b];
                buf_ptr++;
                nbyte--;
                bytes_read++;
                temp.offset_byte++;
            }
        }

        //Prepare for next block:
        head = data_table[head];
        if(temp.offset_byte == 4096) {
            temp.offset_block++;
            temp.offset_byte = 0;
        }

    } while(head != -1);

    // Save meta data:
    des_table[fildes] = temp;

    // Fin
    return bytes_read;
}
/*---------------------------------------------------------------------------*/









/*****************************************************************************/
/* Definition - fs_lseek -----------------------------------------------------*/
int fs_lseek(int fildes, off_t offset) {

    // Validate filedes:
    if(!des_table[fildes].opened) return -1;
    descriptor temp = des_table[fildes];

    // Validate offset:
    if(offset < 0) return -1;
    if(offset > fs_get_filesize(fildes)) return -1;

    // Set offset of fildes:
    des_table[fildes].offset_block = offset / BLOCK_SIZE;
    des_table[fildes].offset_byte = offset % BLOCK_SIZE;

    // Return success:
    return 0;
}
/*---------------------------------------------------------------------------*/









/*****************************************************************************/
/* Definition - fs_truncate -------------------------------------------------*/
int fs_truncate(int fildes, off_t length) {
    
    // Validate filedes:
    if(!des_table[fildes].opened) return -1;
    descriptor temp = des_table[fildes];

    // Check for am oversized/negative truncation:
    if(length > fs_get_filesize(fildes) || length < 0) return -1;

    // Truncation:
    directory[temp.directory_num].filesize = length;

    // Possible repositionning of fildes:
    int file_pointer = temp.offset_block * BLOCK_SIZE + 
    temp.offset_byte;
    if(file_pointer > length) fs_lseek(fildes, length);

    // Truncate extra blocks **DANGER
    int traversals = trunc_blocks(directory[des_table[fildes].directory_num].head,
    des_table[fildes].offset_block);
    if(des_table[fildes].offset_block > traversals) return -1;

    // Truncate extra bytes:
    char wrt_blk[BLOCK_SIZE];
    short last_blk = get_block_by_offset(directory[des_table[fildes].directory_num].head,
    des_table[fildes].offset_block);
    block_read(last_blk, wrt_blk);
    for(int c = des_table[fildes].offset_byte; c < BLOCK_SIZE; c++) {
        wrt_blk[c] = 0;
    }
    block_write(last_blk, wrt_blk);

    //May need to deallocate head block:
    if(fs_get_filesize(fildes) == 0) {
        data_table[directory[des_table[fildes].directory_num].head] = 0;
        directory[des_table[fildes].directory_num].head = -1;
    }
    return 0;


}
/*----------------------------------------------------------------------------*/











/*****************************************************************************
*   Definitions: Test Interface                                              *
/*****************************************************************************/





/*****************************************************************************/
/* Definition - fs_test::get_count ------------------------------------------*/
short fs_test::get_count() {
    return file_count;
} /*-------------------------------------------------------------------------*/





/*****************************************************************************/
/* Definition - fs_test::get_directory --------------------------------------*/
address fs_test::get_address(int a) {
    return directory[a];
} /*-------------------------------------------------------------------------*/





/*****************************************************************************/
/* Definition - fs_test::get_descriptor -------------------------------------*/
descriptor fs_test::get_descriptor(int d) {
    return des_table[d];
} /*-------------------------------------------------------------------------*/


short* fs_test::get_table() {
    return data_table;
}