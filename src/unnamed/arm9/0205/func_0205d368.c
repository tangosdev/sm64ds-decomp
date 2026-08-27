int func_0205d368(int *o, int r1, int sel)
{
    switch (sel) {
    case 0: r1 += o[8]; break;     /* 0x20 */
    case 1: r1 += o[10]; break;    /* 0x28 */
    case 2: r1 += o[9]; break;     /* 0x24 */
    default: return 0;
    }
    if ((unsigned)r1 < (unsigned)o[8]) r1 = o[8];  /* cmp; movlo (unsigned) */
    if (r1 > o[9]) r1 = o[9];                       /* cmp; movgt (signed)   */
    o[10] = r1;
    return 1;
}
