/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208ecf4..0x0208edf8, 260 bytes, 1 configured symbols.
 */
#include "types.h"
#include "private/arm9_archive_table.h"

extern char data_0208eb5c[];
extern char data_0208ecb0[];
extern char data_0208eb74[];
extern char data_0208ebd4[];
extern char data_0208eb70[];
extern char data_0208ebfc[];
extern char data_0208eb58[];
extern char data_0208ec10[];
extern char data_0208eb78[];
extern char data_0208ec38[];
extern char data_0208eb84[];
extern char data_0208ec9c[];
extern char data_0208eb60[];
extern char data_0208ec88[];
extern char data_0208eb7c[];
extern char data_0208ebac[];
extern char data_0208eb68[];
extern char data_0208ebe8[];
extern char data_0208eb64[];
extern char data_0208ec24[];
extern char data_0208eb88[];
extern char data_0208ec60[];
extern char data_0208eb80[];
extern char data_0208ebc0[];
extern char data_0208eb6c[];
extern char data_0208ec4c[];

/* 0x0208ecf4: 260 bytes. */
ArchiveTableEntry data_0208ecf4[13] = {
    {0, 0, 0x8000, 0x80c6, (char *)&data_0208eb5c, (char *)&data_0208ecb0},
    {0, 0, 0x8400, 0x8427, (char *)&data_0208eb74, (char *)&data_0208ebd4},
    {0, 0, 0x8800, 0x8805, (char *)&data_0208eb70, (char *)&data_0208ebfc},
    {0, 0, 0x8c00, 0x8c05, (char *)&data_0208eb58, (char *)&data_0208ec10},
    {0, 0, 0x9000, 0x9005, (char *)&data_0208eb78, (char *)&data_0208ec38},
    {0, 0, 0x9400, 0x9405, (char *)&data_0208eb84, (char *)&data_0208ec9c},
    {0, 0, 0x9800, 0x9810, (char *)&data_0208eb60, (char *)&data_0208ec88},
    {0, 0, 0x9c00, 0x9c11, (char *)&data_0208eb7c, (char *)&data_0208ebac},
    {0, 0, 0xa000, 0xa010, (char *)&data_0208eb68, (char *)&data_0208ebe8},
    {0, 0, 0xa400, 0xa410, (char *)&data_0208eb64, (char *)&data_0208ec24},
    {0, 0, 0xa800, 0xa810, (char *)&data_0208eb88, (char *)&data_0208ec60},
    {0, 0, 0xac00, 0xac10, (char *)&data_0208eb80, (char *)&data_0208ebc0},
    {0, 0, 0xb000, 0xb010, (char *)&data_0208eb6c, (char *)&data_0208ec4c},
};
