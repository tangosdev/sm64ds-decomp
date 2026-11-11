/* NitroSDK FS_SeekFile: move the file position relative to the file's top,
 * its current position or its bottom, clamped to [top, bottom].
 *
 * The lower clamp compares unsigned and the upper one signed. That is the
 * ROM's (movlo, then movgt), so this SDK compares offset against the u32
 * top uncast and casts only bottom, where pret's later FS_file.c casts
 * both. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

int FS_SeekFile(FSFile *p_file, int offset, FSSeekFileMode origin) {
    switch (origin) {
    case FS_SEEK_SET:
        offset += p_file->prop.file.top;
        break;
    case FS_SEEK_CUR:
        offset += p_file->prop.file.pos;
        break;
    case FS_SEEK_END:
        offset += p_file->prop.file.bottom;
        break;
    default:
        return 0;
    }
    if (offset < p_file->prop.file.top)
        offset = p_file->prop.file.top;
    if (offset > (int)p_file->prop.file.bottom)
        offset = p_file->prop.file.bottom;
    p_file->prop.file.pos = offset;
    return 1;
}
