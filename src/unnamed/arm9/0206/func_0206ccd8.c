typedef struct S { int f0, f1, f2, f3; void* cb; } S;

extern int func_0206cf7c(int mask);
extern void func_0206a928(void* a, int b, void* c, void* d);
extern void func_0206cbc0(void);

#pragma optimize_for_size on
void func_0206ccd8(int first, ...)
{
    S s;
    int* p;

    if (func_0206cf7c(0x20000) == 0) goto done;

    s.f0 = 0;
    s.f3 = 0;
    s.f1 = -1;
    s.cb = (void*)func_0206cbc0;

    p = (int*)(((int)&first) & ~3) + 1;
    func_0206a928(&s, first, &p, p);
done:
    ;
}
