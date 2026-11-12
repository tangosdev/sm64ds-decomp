#ifndef MSL_FILE_STRUC_H
#define MSL_FILE_STRUC_H

#include "types.h"

/*
 * MSL's file_struc.h: the FILE of the __files table (0x0209a330) and the
 * proc types of its read/write/close_proc. The console procs
 * (src/__read_console.c, src/__write_console.c, src/__close_console.c) are
 * three of those procs; fflush, __flush_all, __flush_buffer and
 * __prep_buffer (src/cstd4.cpp) read the FILE itself.
 */

/* The host handle a FILE's procs are given. */
typedef unsigned long __file_handle;

/* Run by an io_proc while it waits. */
typedef void (*__idle_proc)(void);

typedef int (*__pos_proc)(__file_handle file, unsigned long *position, int mode,
                          __idle_proc idle_proc);
typedef int (*__io_proc)(__file_handle file, unsigned char *buff, size_t *count,
                         __idle_proc idle_proc);
typedef int (*__close_proc)(__file_handle file);

/* mode.io_mode */
enum __io_modes { __read = 1, __write = 2, __read_write = 3, __append = 4 };

/* mode.file_kind */
enum __file_kinds { __closed_file, __disk_file, __console_file, __unavailable_file };

/* state.io_state */
enum __io_states { __neutral, __writing, __reading, __rereading };

typedef struct {
    unsigned int open_mode : 2;
    unsigned int io_mode : 3;
    unsigned int buffer_mode : 2;
    unsigned int file_kind : 3;
    unsigned int file_orientation : 2;
    unsigned int binary_io : 1;
} __file_modes;

typedef struct {
    unsigned int io_state : 3;
    unsigned int free_buffer : 1;
    unsigned char eof;
    unsigned char error;
} __file_state;

/* 0x4c bytes. The three console FILEs' initializers in __files fix the
 * tail: buffer, buffer_size and buffer_ptr at 0x1c..0x24, the console
 * procs at 0x3c..0x44, and no next_file_struct after idle_proc, so
 * __flush_all walks __files by index. state is eight bytes under mwccarm
 * (fflush reads state.error at 0x0d), which leaves 0x10..0x17 for the
 * single-byte fields: no wide ungetwc_buffer fits before position. */
struct _FILE {
    __file_handle handle;                   /* 0x00 */
    __file_modes mode;                      /* 0x04 */
    __file_state state;                     /* 0x08 */
    unsigned char is_dynamically_allocated; /* 0x10 */
    char char_buffer;                       /* 0x11 */
    char char_buffer_overflow;              /* 0x12 */
    char ungetc_buffer[2];                  /* 0x13 */
    unsigned long position;                 /* 0x18 */
    unsigned char *buffer;                  /* 0x1c */
    unsigned long buffer_size;              /* 0x20 */
    unsigned char *buffer_ptr;              /* 0x24 */
    unsigned long buffer_len;               /* 0x28 */
    unsigned long buffer_alignment;         /* 0x2c */
    unsigned long saved_buffer_len;         /* 0x30 */
    unsigned long buffer_pos;               /* 0x34 */
    __pos_proc position_proc;               /* 0x38 */
    __io_proc read_proc;                    /* 0x3c */
    __io_proc write_proc;                   /* 0x40 */
    __close_proc close_proc;                /* 0x44 */
    __idle_proc idle_proc;                  /* 0x48 */
};
typedef struct _FILE FILE;

#ifndef SM64DS_PLATFORM_PC
typedef char FILE_size_must_be_0x4c[sizeof(FILE) == 0x4c ? 1 : -1];
#endif

#endif /* MSL_FILE_STRUC_H */
