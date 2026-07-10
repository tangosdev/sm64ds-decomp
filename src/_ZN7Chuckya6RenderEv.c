extern void func_ov062_02116edc(void *c);
extern int _ZN5Model6RenderEPK7Vector3(void *m, void *v);
int _ZN7Chuckya6RenderEv(void *c)
{
    void *r1 = (void*)*(int*)((char*)c + 0x3f8);
    if (r1 != 0) {
        int r0 = *(int*)((char*)c + 0xb0);
        int flag = (r0 & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int*)((char*)r1 + 0xc8) != 0) {
                func_ov062_02116edc(c);
            }
        }
    }
    _ZN5Model6RenderEPK7Vector3((char*)c + 0x300, (void*)0);
    return 1;
}
