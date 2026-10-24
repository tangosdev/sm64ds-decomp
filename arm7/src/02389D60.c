/* storage 0x02389D60 runtime 0x03801BF8 size 0x1C + 8 pool wram ARM.
 * If a1 <= 15, publish a1 and clear the companion word, then return the
 * companion's address as an int; else return a1 unchanged. Fully
 * predicated (LE) LDR/STR/MOV sequence. Two literal-pool data refs. */

extern int unk_3809518; /* runtime 0x03809518 */
extern int unk_3809514; /* runtime 0x03809514 */

int fn_02389D60(int a1) {
    if (a1 <= 15) {
        unk_3809518 = a1;
        unk_3809514 = 0;
        a1 = (int)&unk_3809514;
    }
    return a1;
}
