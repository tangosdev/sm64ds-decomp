/* storage 0x0239CE18 runtime 0x027CED90 size 0xF4 main. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */
extern int fn_02397DD8(int); /* =0x027C9D50 */
extern int fn_02397DA4(int); /* =0x027C9D1C */
extern int fn_02397D60(int); /* =0x027C9CD8 */
extern int fn_02397CCC(int); /* =0x027C9C44 */
extern int fn_02397C90(int); /* =0x027C9C08 */
extern int fn_02397BB0(int); /* =0x027C9B28 */
extern int fn_02397B8C(int); /* =0x027C9B04 */
extern int fn_02397B28(int); /* =0x027C9AA0 */
extern int fn_02397AEC(int); /* =0x027C9A64 */
extern int fn_02397AB0(int); /* =0x027C9A28 */
extern int fn_02397A78(int); /* =0x027C99F0 */
extern int fn_023979B0(int, int); /* =0x027C9928 */
extern int fn_02397980(int); /* =0x027C98F8 */
extern int fn_023978A4(int); /* =0x027C981C */
extern int fn_02397880(int); /* =0x027C97F8 */

int fn_0239CE18(unsigned short *a1, int a2) {
    int result = 1;
    *(unsigned short *)(a2 + 2) = result;
    if (*(unsigned short *)(off_380FFF4 + 784) == 16) {
        int v = fn_02397DD8((int)(a1 + 8));
        v |= fn_02397DA4(a1[11]);
        v |= fn_02397D60(a1[12]);
        v |= fn_02397CCC(a1[14]);
        v |= fn_02397C90(a1[15]);
        v |= fn_02397BB0(a1[16]);
        v |= fn_02397B8C(a1[17]);
        v |= fn_02397B28((int)(a1 + 18));
        v |= fn_02397AEC(a1[58]);
        v |= fn_02397AB0(a1[59]);
        v |= fn_02397A78(a1[60]);
        v |= fn_023979B0(a1[61], 0);
        v |= fn_02397980((int)(a1 + 62));
        v |= fn_023978A4(a1[78]);
        return v | fn_02397880(a1[79]);
    }
    return result;
}
