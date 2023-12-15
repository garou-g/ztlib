/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

/* Includes */
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

extern int errno;
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

__attribute__((weak)) int _read(int file, char* ptr, int len)
{
    for (int i = 0; i < len; ++i) {
        *ptr++ = __io_getchar();
    }
    return len;
}

__attribute__((weak)) int _write(int file, char* ptr, int len)
{
    for (int i = 0; i < len; ++i) {
        __io_putchar(*ptr++);
    }
    return len;
}

register char * stack_ptr asm("sp");

__attribute__((weak)) caddr_t _sbrk(int incr)
{
    extern char end asm("end");
    static char* heap_end;
    char* prev_heap_end;

    if (heap_end == 0)
        heap_end = &end;

    prev_heap_end = heap_end;
    if (heap_end + incr > stack_ptr) {
        // write(1, "Heap and stack collision\n", 25);
        // abort();
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;
    return (caddr_t)prev_heap_end;
}

__attribute__((weak)) int _close(int file)
{
    return -1;
}

__attribute__((weak)) int _lseek(int file, int ptr, int dir)
{
    return 0;
}

__attribute__((weak)) int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

__attribute__((weak)) int _getpid(void)
{
    return 1;
}

__attribute__((weak)) int _isatty(int file)
{
    return 1;
}

__attribute__((weak)) int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}
