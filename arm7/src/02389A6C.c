/* storage 0x02389A6C runtime 0x03801904 size 0x134 wram ARM, no pools.
 * Dense 16-case switch (CMP + ADDLS PC branch table), all paths converge
 * on one epilogue. Cases 15/14 are in that source order (blocks lay out in
 * source order). Callees: out-of-pack fn_02389D60/fn_023833CC, in-pack
 * fn_02389A04/fn_02389874 swapped. fn_02389848 stays enrolled separately. */

extern void fn_02389D60(int a1);
extern int *fn_02389A04(int a1);
extern int fn_02389848(int a1);
extern int fn_02389874(int a1);
extern void fn_023833CC(void);

int *fn_02389A6C(int a1) {
    int *result;
    switch ((unsigned int)a1) {
    case 1:
        fn_02389D60(1);
        result = fn_02389A04(1);
        break;
    case 2:
        fn_02389D60(3);
        result = fn_02389A04(3);
        break;
    case 3:
        fn_02389D60(2);
        result = fn_02389A04(2);
        break;
    case 4:
        result = (int *)fn_02389874(4);
        break;
    case 5:
        result = (int *)fn_02389848(4);
        break;
    case 6:
        result = (int *)fn_02389874(8);
        break;
    case 7:
        result = (int *)fn_02389848(8);
        break;
    case 8:
        result = (int *)fn_02389874(12);
        break;
    case 9:
        result = (int *)fn_02389848(12);
        break;
    case 10:
        result = (int *)fn_02389874(1);
        break;
    case 11:
        result = (int *)fn_02389848(1);
        break;
    case 12:
        result = (int *)fn_02389874(2);
        break;
    case 13:
        result = (int *)fn_02389848(2);
        break;
    case 15:
        result = (int *)fn_02389848(64);
        break;
    case 14:
        fn_023833CC();
        fn_02389848(1);
        result = (int *)fn_02389874(64);
        break;
    default:
        result = (int *)a1;
        break;
    }
    return result;
}
