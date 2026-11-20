extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern int fn_02381704(int, int);
extern void fn_02381050(int);
extern int dword_3806E60[];

typedef struct {
    char pad[4];
    int owner;
    int count;
} Obj17E0;

typedef struct {
    char pad[92];
    int slot;
} Cur17E0;

int fn_023817E0(int a1) {
    Obj17E0 *obj = (Obj17E0 *)a1;
    int saved = fn_02382AC4();
    Cur17E0 *cur = (Cur17E0 *)dword_3806E60[2];
    int zero = 0;
    for (;;) {
        int owner = obj->owner;
        if (owner == 0) {
            obj->owner = (int)cur;
            ++obj->count;
            fn_02381704((int)cur, a1);
            break;
        }
        if (owner == (int)cur) {
            ++obj->count;
            break;
        }
        cur->slot = a1;
        fn_02381050(a1);
        cur->slot = zero;
    }
    return fn_02382AD8(saved);
}
