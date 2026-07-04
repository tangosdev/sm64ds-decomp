extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void LinkSilverStarAndStarMarker(char* a, char* b);
extern void func_ov002_020e9464(char* c);
extern void func_ov002_020e8ef0(char* c, int a);

void func_ov002_020e8e80(char* c, int a)
{
    int* p;
    char* pl = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x434));
    if (pl != 0)
        LinkSilverStarAndStarMarker(pl, 0);

    *(int*)(c + 0x438) = a;
    func_ov002_020e9464(c);
    p = (int*)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
    *p = *p & ~0x40000;
    func_ov002_020e8ef0(c, a);
}
