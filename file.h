#ifndef file_h
#define file_h

#include <vector>


const int MAX_FILENAME = 16;            // Max number of chars for filename.

/*-----------------------------------------------------------------------------
 * Struct file
 * Stores necessary information for interfacing with a file that has been 
 * opened on file system. */
struct descriptor {
    bool opened;
    short offset_block;
    short offset_byte;
    short directory_num;
};


/*------------------------------------------------------------------------------
 * Struct address
 * How files are listen in fs directory.*/
struct address {
    char name[MAX_FILENAME];
    short head;
    int filesize;
};

#endif