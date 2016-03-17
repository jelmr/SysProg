#ifndef MYSYS_H
#define MYSYS_H
int my_open(const char *path, int oflag, int mode);
int my_close(int fd);
long my_read(int fd, void *buf, unsigned long nbyte);
long my_write(int fd, const void *buf, unsigned long nbyte);
long my_lseek(int fd, long offset, int whence);

  #if defined(__FreeBSD__)
  #  define SYSCALL         "int $0x80"
  #  define O_RDONLY        0x0000
  #  define O_WRONLY        0x0001
  #  define O_RDWR          0x0002
  #  define O_APPEND        0x0008
  #  define O_CREAT         0x0200
  #  define O_TRUNC         0x0400
  #  define SYS_read        3
  #  define SYS_write       4
  #  define SYS_open        5
  #  define SYS_close       6
  #  define SYS_lseek       478
  #  define REG_A           "a"
  #  define REG_B           "b"
  #  define REG_C           "c"
  #  define REG_D           "d"
  #elif defined(__linux__)
  #  define O_RDONLY        00
  #  define O_WRONLY        01
  #  define O_RDWR          02
  #  define O_CREAT         0100
  #  define O_TRUNC         01000
  #  define O_APPEND        02000
  #  if defined(__i386__)
  #    define SYSCALL       "int $0x80"
  #    define SYS_read      3
  #    define SYS_write     4
  #    define SYS_open      5
  #    define SYS_close     6
  #    define SYS_lseek     19
  #    define REG_A         "a"
  #    define REG_B         "b"
  #    define REG_C         "c"
  #    define REG_D         "d"
  #  elif defined(__x86_64)
  #    define SYSCALL       "syscall;"
  #    define SYS_read      0
  #    define SYS_write     1
  #    define SYS_open      2
  #    define SYS_close     3
  #    define SYS_lseek     8
  #    define REG_A         "a"
  #    define REG_B         "D"
  #    define REG_C         "S"
  #    define REG_D         "d"
  #  endif
  #elif defined(__APPLE__)
  #  define SYSCALL         "syscall;"
  #  define O_RDONLY        0x0000
  #  define O_WRONLY        0x0001
  #  define O_RDWR          0x0002
  #  define O_APPEND        0x0008
  #  define O_CREAT         0x0200
  #  define O_TRUNC         0x0400
  #  define SYS_read        0x2000003
  #  define SYS_write       0x2000004
  #  define SYS_open        0x2000005
  #  define SYS_close       0x2000006
  #  define SYS_lseek       0x2000478
  #  define REG_A           "a"
  #  define REG_B           "D"
  #  define REG_C           "S"
  #  define REG_D           "d"
  #endif
#endif
