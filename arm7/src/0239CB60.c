/* storage 0x0239CB60 runtime 0x027CEAD8 size 0x38 (code 0x34 + pool) main ARM.
 * Store 4 at a2+2, call fn_023969D8(a2+6, unk_380FFF4+0x2E8), return 0.
 * One pool word (unk_380FFF4), one BL reloc. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
extern int fn_023969D8 /* =0x027C8950 */(int a1, int a2);

int fn_0239CB60(int a1, int a2) {
    *(unsigned short *)(a2 + 2) = 4;
    fn_023969D8(a2 + 6, unk_380FFF4 + 0x2E8);
    return 0;
}
