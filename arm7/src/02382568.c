/* storage 0x02382568 runtime 0x037FA400 size 0x70 + pool 0x74 wram ARM.
 * Walk the node list, firing the callback on id match, then rearm. Key
 * shape: for-loop with the advance in the increment slot
 * (`v3 = v4, v4 = v4 ? v4[5] : z`). The loop-carried use of the zero var
 * `z` in the increment expression keeps it homed in R4 (`MOV R4,#0`
 * pre-loop, `MOVEQ R6,R4` in-loop) where every while-form spelling folds
 * it to `MOVEQ #0`; the init site still folds to `MOVEQ R6,#0`. The 5-reg
 * frame + SUB/ADD SP slot is pure 8-byte alignment. Three calls. */
extern int dword_3806FF0[];
extern int fn_02382AC4(void);
extern void fn_02382724(int);
extern int fn_02382AD8(int);
int fn_02382568(int result) {
    int v1 = result;
    if (result != 0) {
        int v2 = fn_02382AC4();
        int *v3;
        int *v4;
        int z = 0;
        for (v3 = *(int **)dword_3806FF0, v4 = v3 != 0 ? (int *)v3[5] : 0;
             v3 != 0;
             v3 = v4, v4 = v4 != 0 ? (int *)v4[5] : (int *)z) {
            if (v3[2] == v1)
                fn_02382724((int)v3);
        }
        return fn_02382AD8(v2);
    }
    return result;
}
