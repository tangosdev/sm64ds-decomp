typedef int (*Fn)(void);

extern int func_02043060(void *a);
extern void func_02043180(int a, int b, int c, int d);
extern void func_020433b8(char *self);

extern unsigned short CURRENT_OBJECT_SPAWN_ID;
extern unsigned char BASE_CREATION_STATE;
extern Fn **ACTOR_SPAWN_INFO_TABLE_PTR;

int func_02043098(int idx, int b, int c, int d)
{
    int r5;
    char *c_arg;

    CURRENT_OBJECT_SPAWN_ID = (unsigned short)idx;
    BASE_CREATION_STATE = 2;
    if (func_02043060((void *)idx) == 3) return 0;

    BASE_CREATION_STATE = 3;
    func_02043180(idx, b, c, d);

    BASE_CREATION_STATE = 4;
    r5 = (*ACTOR_SPAWN_INFO_TABLE_PTR[idx])();
    if (r5 == 0) {
        BASE_CREATION_STATE = 0;
        CURRENT_OBJECT_SPAWN_ID = 0xffff;
        return 0;
    }
    BASE_CREATION_STATE = 5;
    c_arg = (char *)r5;
    func_020433b8(c_arg);
    BASE_CREATION_STATE = 0;
    CURRENT_OBJECT_SPAWN_ID = 0xffff;
    return r5;
}
