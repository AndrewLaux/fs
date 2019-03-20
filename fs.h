#ifndef FS_H
#define FS_H

#include "disk.h"
#include "file.h"
#include <cmath>
#include <vector>
#include <string.h>
#include <sys/types.h>



 

/*-----------------------------------------------------------------------------
 * fs_test namespace
 * Contains functions for the testing of fs code. Has access to anonymous 
 * functions and variables declared in this header. */
namespace fs_test {
    address get_address(int a);
    descriptor get_descriptor(int d);
    short get_count();
    short* get_table();
}


/*-----------------------------------------------------------------------------
 * Interface method make_fs:
 * Simply calls make_disc which allocates and memsets to zero. This is fine
 * for my implementation. Data table entry 0 means not in use. Filecount
 * 0 means no files present (expected state of a new file system). */
int make_fs(char *disk_name);

/*-----------------------------------------------------------------------------
 * Interface method mount_fs:
 * Calls open_disk, to make disk file available for writing and reading. Loads
 * meta data including the following: (1) copys data table into mem (2) Get 
 * number of files present on disk. (2) gets current directoy of files from
 * disk. (4) Zeros descriptor table. Returns -1 on fail. Will fail if fs_name is not null (meaning there
 * was some other fs mounted). It will also fail if the disk_open fails. */
int mount_fs(char *disk_name);

/*-----------------------------------------------------------------------------
 * Interface method: umount_fs
 * Calls close_disk. Also sets fs_name to null pointer.*/
int umount_fs(char *disk_name);


/*-----------------------------------------------------------------------------
 * Interface method: fs_create
 * Creates entry in directory. Must be less than 64 entries. Name must be 15
 * characters or less. Will not allocate any blocks of memory before file is
 * written to.*/
int fs_create(char *name);

/*-----------------------------------------------------------------------------
 * Interface method: fs_open
 * Creates a filedescriptor for the specified file. A file can have multiple 
 * file descriptors, but the library supports a total of 32 descriptors. */
int fs_open(char *name);

/*-----------------------------------------------------------------------------
 * Interface method: fs_write
 * Writes n bytes amount from buffer to location pointed to by descriptor. 
 * Leaves the filedesciptor pointing to the last location that it wrote to.
 */
int fs_write(int fildes, void* buf, size_t nbyte);

/*-----------------------------------------------------------------------------
 * Interface method: fs_read
 * Read nbytes number of bytes from file into buffer*/
int fs_read(int fildes, void* buf, size_t nbyte);

/*-----------------------------------------------------------------------------
 * Interface method: fs_close
 * Simply validates that the file descriptor is opened, then sets it to closed.
 * Returns -1 if the filedes was not valid. 0 on success.*/
int fs_close(int fildes);



/*-----------------------------------------------------------------------------
 * Interface method: fs_get_filesize
 * If the fildes is valid (is open and pointing to a file) returns the file 
 * size. Returns -1 if invalid. */
int fs_get_filesize(int fildes);


/*-----------------------------------------------------------------------------
 * Interface method: fs_delete
 * Deletes file with matching filename. Not able to delete a file with open 
 * descriptors (returns -1 in such a case). Performs deletion by freeing up 
 * blocks and deleting entry from directory. */
int fs_delete(char* name);

/*-----------------------------------------------------------------------------
 * Interface method: fs_lseek
 *  */
int fs_lseek(int fildes, off_t offset);

/*-----------------------------------------------------------------------------
 * Interface method: fs_truncate
 *  */
int fs_truncate(int fildes, off_t length);

#endif