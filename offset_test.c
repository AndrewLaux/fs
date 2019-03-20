#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fs.h"

void cleanup();

// MACROS
// clang-format off
#define DEBUGGING true
#define USE_DETERMISTIC_RANDSEED true
#define dprint(debugmsg)if (DEBUGGING) printf("%s\n", debugmsg)
#define tryf(ret, caller) if (!(ret)){printf("ERROR %s failed\n", caller); \
    cleanup();                                                          \
    exit(-1);}
#define BUFSIZE 4098
// clang-format on

char *rand_data;
char *half_rand_data;
char *xs;
char *buf;

int main(int argc, char *argv[]) {
  char disk_name[] = "myfs2";
  char file1[] = "file1";
  int i, j;
  int f1, f2, f3;
  int ret;

  if (USE_DETERMISTIC_RANDSEED) {
    // deterministic seed
    srand(42);
  } else {
    // non deterministic seed
    srand(time(NULL));
  }

  dprint("calloc buffers");
  tryf((rand_data = (char *)calloc(BUFSIZE, 1)) != NULL, "rand_data calloc");
  tryf((buf = (char *)calloc(BUFSIZE, 1)) != NULL, "buf calloc");
  tryf((xs = (char *)calloc(BUFSIZE / 2, 1)) != NULL, "xs calloc");
  tryf((half_rand_data = (char *)calloc(BUFSIZE, 1)) != NULL,
       "half_rand_data calloc");

  dprint("Generate random data");
  for (i = 0; i < BUFSIZE; ++i) {
    *((char *)rand_data + i) = (char)(rand() % (126 - 32) + 32);
  }

  memcpy(half_rand_data, rand_data, BUFSIZE);
  memset(xs, 'x', BUFSIZE / 2);
  memset(half_rand_data + BUFSIZE / 2, 'x', BUFSIZE / 2);

  dprint("make myfs");
  tryf(make_fs(disk_name) == 0, "make_fs");

  dprint("mount myfs");
  tryf(mount_fs(disk_name) == 0, "mount_fs");

  dprint("create file1");
  tryf(fs_create(file1) == 0, "fs_create");

  dprint("open f1");
  tryf((f1 = fs_open(file1)) >= 0, "fs_open");

  dprint("write to f1");
  tryf(fs_write(f1, rand_data, BUFSIZE) == BUFSIZE, "fs_write");

  dprint("lseek f1");
  tryf(fs_lseek(f1, BUFSIZE / 2) == 0, "fs_lseek");

  dprint("write xs to f1");
  tryf(fs_write(f1, xs, BUFSIZE / 2) == BUFSIZE / 2, "fs_write");

  dprint("open f2");
  tryf((f2 = fs_open(file1)) >= 0, "fs_open");

  dprint("read f2");
  tryf(fs_read(f2, buf, BUFSIZE / 2) == BUFSIZE / 2, "fs_read f2");

  dprint("compare buf to rand_data");
  tryf(memcmp(buf, rand_data, BUFSIZE / 2) == 0, "buf[:sz/2] == expected?");

  dprint("read f2");
  tryf(fs_read(f2, buf + BUFSIZE / 2, BUFSIZE / 2) == BUFSIZE / 2,
       "fs_read f2");

  dprint("compare buf to half_rand_data");
  tryf(memcmp(buf, half_rand_data, BUFSIZE) == 0, "buf == expected?");

  dprint("close f2");
  tryf(fs_close(f2) == 0, "fs_close");

  dprint("close f1");
  tryf(fs_close(f1) == 0, "fs_close");

  dprint("===========\nTEST PASSED");

  return 0;
}

void cleanup() {
  if (rand_data) {
    free(rand_data);
  }
  if (buf) {
    free(buf);
  }
}
