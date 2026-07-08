extern void func_0202345c(void);
extern void func_02018efc(void);
extern void _ZN3OAM5ResetEv(void);
extern int func_02019018(void);
extern void func_0200f4b4(void);
extern void func_0200f468(void);
extern void func_02018ec0(void);

typedef int (*VFN)(void* o);
typedef struct Obj {
    VFN* vt;
} Obj;

extern Obj* SCENE_RELATED;

void func_02019390(void)
{
    Obj* o = SCENE_RELATED;
    if (o != 0) {
        if (o->vt[0](o) == 0) {
            func_0202345c();
            func_02018efc();
            return;
        }
    }
    _ZN3OAM5ResetEv();
    if (func_02019018() != 0) {
        func_0200f4b4();
    }
    func_0200f468();
    func_0202345c();
    func_02018efc();
    func_02018ec0();
}
