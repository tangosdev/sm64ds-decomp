/* storage 0x0238092C runtime 0x037F87C4 size 0x40 (+0xC pool) wram ARM.
 * Three u32 slots of a 12-byte-stride record at unk_3806E14, then
 * fn_023808C0(1 << (a1+3)) and a flag store. Returns unk_3806E18. */

extern char unk_3806E14[];
extern char unk_3806E18[];
extern char unk_3806E1C[];
extern int fn_023808C0(int);

void *fn_0238092C(int a1, int a2, int a3) {
    int v3 = 12 * a1;
    *(int *)(unk_3806E14 + v3) = a2;
    *(int *)(unk_3806E1C + v3) = a3;
    fn_023808C0(1 << (a1 + 3));
    *(int *)(unk_3806E18 + v3) = 1;
    return unk_3806E18;
}
