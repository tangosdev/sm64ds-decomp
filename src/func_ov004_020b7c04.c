extern int data_ov004_020bc8b8;
extern int data_ov004_020bc8b4;
extern void* SCENE_FADER;
extern char data_0209f61c[];
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void func_ov004_020ae274(int c);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(void* fb);
extern void func_0202ec9c(void* thiz, int arg1);
extern void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
extern void func_02012dd0(int c);

struct VT { int (*m[8])(void*); };
struct Obj { struct VT* vt; };

void func_ov004_020b7c04(char* c)
{
    struct Obj* o;
    int v = *(int*)(c + 0x1c);
    if (v == 0xb3) {
        func_ov004_020b0cac(8, data_ov004_020bc8b8, data_ov004_020bc8b4, -1, -1, 0xd);
        func_ov004_020ae274(3);
        return;
    }
    if (v != 0) return;
    o = *(struct Obj**)&SCENE_FADER;
    if (o->vt->m[7](o) != 0) return;
    _ZN5Scene9SetFadersEP15FaderBrightness(data_0209f61c);
    func_0202ec9c(data_0209f61c, 2);
    _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
    func_02012dd0(0x3c);
}
