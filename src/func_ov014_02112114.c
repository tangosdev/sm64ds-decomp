extern void *_ZN5Actor10FindWithIDEj(unsigned id);
extern void func_ov014_02111ebc(void *c, int i);
extern int Vec3_HorzAngle(void *a, void *b);
extern int AngleDiff(int a, int b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *o, void *v, unsigned a, int fx, unsigned b, unsigned d, unsigned e);
extern void func_ov102_0214ae1c(void *o);

void func_ov014_02112114(void *cc)
{
    char *c = (char*)cc;
    char *e;
    int id;
    unsigned h;
    int ang;

    if ((unsigned)(*(int*)(c + 0x610) - 3) <= 1)
        return;
    id = *(int*)(c + 0x134);
    if (id == 0)
        return;
    e = (char*)_ZN5Actor10FindWithIDEj(id);
    if (e == 0)
        return;

    h = *(unsigned short*)(e + 0xc);
    if ((int)(h == 9) != 0) {
        if (*(int*)(c + 0x130) & 0x2000) {
            *(int*)(c + 0x80) = 0x2000;
            *(int*)(c + 0x84) = *(int*)(c + 0x80);
            *(int*)(c + 0x88) = *(int*)(c + 0x84);
            func_ov014_02111ebc(c, 0);
            return;
        }
    }

    if (*(int*)(c + 0x130) & 0x4000) {
        *(int*)(c + 0x80) = 0x2000;
        *(int*)(c + 0x84) = *(int*)(c + 0x80);
        *(int*)(c + 0x88) = *(int*)(c + 0x84);
        func_ov014_02111ebc(c, 4);
        return;
    }

    if ((int)(h == 0xbf) != 0) {
        ang = Vec3_HorzAngle(c + 0x5c, e + 0x5c);
        if (*(int*)(c + 0x130) & 0x10) {
            func_ov014_02111ebc(c, 5);
            *(short*)(c + 0x94) = *(short*)(e + 0x8e);
        }
        if (AngleDiff(ang, *(short*)(c + 0x8e)) < 0x4000) {
            int v[3];
            v[0] = *(int*)(c + 0x5c);
            v[1] = *(int*)(c + 0x60);
            v[2] = *(int*)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(e, v, 3, 0xc000, 1, 0, 1);
        }
    }

    if (*(int*)(c + 0x610) == 4)
        return;
    if ((int)(*(unsigned short*)(e + 0xc) == 0xce) != 0) {
        func_ov102_0214ae1c(e);
        func_ov014_02111ebc(c, 4);
    }
}
