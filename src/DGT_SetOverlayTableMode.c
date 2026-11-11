/* NitroSDK DGT_SetOverlayTableMode: set the flag that makes the SHA-1 block
 * function skip each overlay-table entry's file_id (func_0205e3d4), and
 * return the previous setting. FS_AttachOverlayTable turns it on around the
 * overlay table's digest check. */
extern int data_020a80c4;

int DGT_SetOverlayTableMode(int flag) {
    int prev = data_020a80c4;
    data_020a80c4 = flag;
    return prev;
}
