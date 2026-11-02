/* NitroSDK FS_file.c's static FSi_FindPath: resolve path against the current
 * directory (data_020a804c). A leading slash starts at the root of the current
 * archive. An archive prefix of up to 3 characters before ':' selects that
 * archive's root instead, and fails unless FS_FindArchive finds it and it is
 * FS_ARCHIVE_FLAG_LOADED (2). The starting point and the rest of the path go
 * into p_dir, with find_directory set when the caller wants a directory
 * (p_dir_pos) rather than a file (p_file_id), and FS_COMMAND_FINDPATH (4) is
 * sent through FSi_SendCommand. */

#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern FSDirPos data_020a804c;
extern FSArchive *FS_FindArchive(const char *name, int name_len);
extern int FSi_SendCommand(FSFile *p_file, int command);

int FSi_FindPath(FSFile *p_dir, char *path, int p_file_id, int p_dir_pos) {
    FSDirPos pos;
    int i;
    FSArchive *p_arc;
    int loaded;
    unsigned char c;

    c = (unsigned char)path[0];
    if (c == '/' || c == '\\') {
        pos.own_id = 0;
        pos.arc = data_020a804c.arc;
        pos.pos = 0;
        pos.index = 0;
        path++;
    } else {
        pos = data_020a804c;
        for (i = 0; i <= 3; i++) {
            c = (unsigned char)path[i];
            if (c == 0 || c == '/' || c == '\\')
                break;
            if (c == ':') {
                p_arc = FS_FindArchive(path, i);
                if (p_arc == 0)
                    return 0;
                loaded = (p_arc->flag & 2) ? 1 : 0;
                if (loaded == 0)
                    return 0;
                pos.arc = p_arc;
                pos.pos = 0;
                pos.index = 0;
                pos.own_id = 0;
                path += i + 1;
                c = (unsigned char)*path;
                if (c == '/' || c == '\\')
                    path++;
                break;
            }
        }
    }

    p_dir->arc = pos.arc;
    p_dir->arg.findpath.path = path;
    p_dir->arg.findpath.pos = pos;
    if (p_dir_pos != 0) {
        p_dir->arg.findpath.find_directory = 1;
        p_dir->arg.findpath.result = p_dir_pos;
    } else {
        p_dir->arg.findpath.find_directory = 0;
        p_dir->arg.findpath.result = p_file_id;
    }
    return FSi_SendCommand(p_dir, 4);
}
