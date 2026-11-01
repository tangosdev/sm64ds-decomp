extern int myFS_ConvertPathToFileID(void *, int);
/* the ROM archive (see func_02018598.c) */
extern int data_0209d3b4;

/* Resolve a ROM path to its FAT file id, keeping the archive the FSFileID
   names (buf[0]) for func_02018598. myFS_ConvertPathToFileID crashes on a
   path that does not resolve. */
int func_020189f0(int path) {
    int buf[3];
    myFS_ConvertPathToFileID(buf, path);
    data_0209d3b4 = buf[0];
    return buf[1];
}
