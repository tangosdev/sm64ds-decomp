//cpp
extern "C" { extern int data_0209caa0; }

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

#pragma optimize_for_size on
extern "C" int _ZN13QuestionBlock6RenderEv(char* c) {
    if (*(int*)(c + 0x3e8) == 2)
        goto done;
    if ((*(int*)((char*)&data_0209caa0 + 4) & 0x80000000) == 0) {
        int b = (*(unsigned short*)(c + 0xc) == 0x14);
        if (b != 0) {
            Sub* s = (Sub*)(c + 0x320);
            s->m(0);
            goto done;
        }
    }
    {
        Sub* s2 = (Sub*)(c + 0xd4);
        s2->m((int)(c + 0x80));
    }
done:
    return 1;
}
