/* storage 0x02385EF0 runtime 0x037FDD88 size 0x38 wram ARM.
 * Align pointer down to 4, publish it plus the next 4 words into unk_38075A8[6].
 * One literal-pool data ref (unk_38075A8), no calls. */

extern int unk_38075A8[];

int *fn_02385EF0(int *a1) {
    int *v1 = (int *)((unsigned int)a1 & ~3u);
    int *result = unk_38075A8;
    result[0] = (int)v1;
    result[1] = (int)(v1 + 4);
    result[2] = v1[0];
    result[3] = v1[1];
    result[4] = v1[2];
    result[5] = v1[3];
    return result;
}
