/* storage 0x02387410 runtime 0x037FF2A8 size 0xF8 + pool 0xFC wram ARM.
 * Mode-change callback (.registered for event 11): latch a2, run the dense
 * mode switch (b56 B-table), then rearm or advance the sub-state. */
extern int dword_38087A0[];
extern int fn_02380FAC(int);
int fn_02387410(int result, int a2, int a3) {
    if (result == 11 && a3 != 0) {
        int *v3 = dword_38087A0;
        if (v3[2] == 0)
            v3[1] = a2;
        switch (v3[1]) {
            case 0:
                if (v3[2] != 0 && v3[2] == 1) {
                    v3[0] = a2;
                    v3[14] |= 0x10;
                }
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
                v3[14] |= 0x10;
                break;
            default:
                break;
        }
        if ((v3[14] & 0x10) == 0) {
            int *v4 = v3 + 2;
            result = *v4 + 1;
            *v4 = result;
            return result;
        }
        v3[2] = 0;
        if ((v3[14] & 4) != 0)
            return fn_02380FAC(v3[46]);
        return fn_02380FAC((int)(v3 + 16));
    }
    return result;
}
