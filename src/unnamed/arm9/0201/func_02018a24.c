/* The asset handle table: one FAT file id per asset handle, allocated by
   AllocAssetHandleTable and filled by InitializeAssetHandleTable. */
extern unsigned short *data_0209d3b8;

/* The file id for asset handle i (below 0x8000); an id of 0x8000 or more
   (an archived file) passes through unchanged. */
unsigned int func_02018a24(unsigned int i) {
    return i < 0x8000 ? data_0209d3b8[i] : i;
}
