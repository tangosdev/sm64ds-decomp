#ifndef MSL_FILE_STRUC_H
#define MSL_FILE_STRUC_H

/*
 * The FILE proc types of MSL's file_struc.h that this tree's console procs
 * take (src/__read_console.c, src/__write_console.c, src/__close_console.c):
 * the read/write/close_proc of each console FILE in MSL's __files table.
 */

/* The host handle a FILE's procs are given. */
typedef unsigned long __file_handle;

/* Run by an io_proc while it waits. */
typedef void (*__idle_proc)(void);

#endif /* MSL_FILE_STRUC_H */
