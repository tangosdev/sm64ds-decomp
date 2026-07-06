// NONMATCHING: loop-setup scheduling: `shift=0` (mov r3,lr) is emitted before the `while(i<n)` entry guard, but the ROM schedules it after the guard. Declaration order and loop-form permutations do not move it. (div=3)
/* func_0205d304 at 0x0205d304 (arm9), size 0x64
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc */
int func_0205d304(unsigned char* s, int n)
{
    int result = 0;
    if (n <= 3) {
        int i = 0;
        int shift = 0;
        while (i < n) {
            unsigned int c = s[i];
            unsigned int t;
            if (c == 0) break;
            t = c - 0x41;
            if (t <= 0x19) t = t + 0x61;
            else t = t + 0x41;
            result |= t << shift;
            i++;
            shift += 8;
        }
    }
    return result;
}