/* storage 0x0239CC08 runtime 0x027CEB80 size 0x48 (code 0x44 + pool) main ARM.
 * Store 1 at a2+2; if *(unk_380FFF4+784) >= 0x10 return fn_023976A4(a1[8]),
 * else return 1 via a single shared return.
 * One pool word (unk_380FFF4), one BL reloc. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
extern int fn_023976A4 /* =0x027C961C */(int a1);

int fn_0239CC08(unsigned short *a1, int a2) {
    int v2 = 1;
    *(unsigned short *)(a2 + 2) = 1;
    if (*(unsigned short *)(unk_380FFF4 + 784) >= 0x10)
        v2 = fn_023976A4(a1[8]);
    return v2;
}
