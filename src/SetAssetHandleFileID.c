/* The asset handle table (see func_02018a24.c). */
extern unsigned short *data_0209d3b8;

/* Record asset handle i's FAT file id (InitializeAssetHandleTable). Coined
   name. The id is taken as an int: the one caller passes its int file id
   unnarrowed, and an unsigned short parameter would add a narrowing pair at
   that call site that the ROM does not have. */
void SetAssetHandleFileID(unsigned int i, int fileId) {
    data_0209d3b8[i] = fileId;
}
