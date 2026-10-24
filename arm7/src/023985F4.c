/* storage 0x023985F4 runtime 0x027CA56C size 0x58 + pool 0x68 main ARM.
 * If the latched half changed, rotate the mailbox words and flag 0x80. */
extern int dword_380FFF4[];
int fn_023985F4(void) {
    int *pp = (int *)dword_380FFF4;
    int result = *(unsigned short *)((char *)*pp + 1162);
    int v2 = 0xFFFF;
    if (result != v2) {
        unsigned v4 = *(unsigned short *)0x4808094;
        *(unsigned short *)0x4808094 = result;
        *(unsigned short *)0x4808030 |= 0x80;
        *(unsigned short *)0x4808094 = v4;
        result = (int)*pp + 1024;
        *(unsigned short *)((char *)*pp + 1162) = v2;
    }
    return result;
}
