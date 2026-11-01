/* fn_023884D0: acknowledge a completed request under the lock. */

extern int dword_3808FC4[];
extern int word_380941C[];

extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern int fn_02380FE0(int *);

int fn_023884D0(int result)
{
    int v1;
    if (dword_3808FC4[0] != 0 && dword_3808FC4[1] == 4 && dword_3808FC4[279] == result) {
        v1 = fn_02382AC4();
        dword_3808FC4[1] = 5;
        dword_3808FC4[0] = 0;
        dword_3808FC4[279] = 0;
        fn_02382AD8(v1);
        return fn_02380FE0(word_380941C);
    }
    return result;
}
