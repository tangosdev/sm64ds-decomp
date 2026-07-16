/* func_ov002_020d8360 at 0x020d8360
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
typedef struct Vector3 { int x, y, z; } Vector3;

extern int func_ov002_020d82f0(void *c);
extern int func_ov002_020d853c(char *c, char *o);
extern int func_ov002_020d8944(void *self, char *enemy, char *v1);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, Vector3 const *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void func_ov002_020d8d10(void *self, Vector3 *pos);
extern void func_020105cc(void *thiz, unsigned int flags);
extern unsigned short data_ov002_020ff29c[];

int func_ov002_020d8360(void *self, void *other, unsigned int flags)
{
    char *c = self;
    char *o = other;

    {
        int r = func_ov002_020d82f0(c);
        if (!r) return r;
    }

    {
        unsigned char t = *(unsigned char *)(o + 0x726);
        if (t) return t;
    }

    if (func_ov002_020d853c(c, o) == 0) {
        unsigned int m = *(unsigned int *)(c + 0x2f4) & 0x3c0;
        if (m == 0) return m;

        {
            int r = func_ov002_020d8944(c, o, c);
            if (r != 0) {
                r = 1;
                *(unsigned char *)(o + 0x726) = r;
                *(unsigned char *)(c + 0x726) = r;
            }
            return r;
        }
    }

    {
        int flag = flags & 0x380;
        if (flag != 0) {
            *(unsigned char *)(c + 0x6de) = 1;
            *(unsigned char *)(c + 0x6df) = 0;
        }
        {
            Vector3 out, tr, v, cp;
            int *p = (int *)(int)(((long long)(int)(o + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            v.x = p[0];
            v.y = p[1];
            v.z = p[2];
            {
                int hr = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(c, &v, 1, 0, 0,
                        (*(unsigned int *)(o + 8) + 1) & 0xff, 1);
                if (!hr) return hr;
            }
            tr.x = 0;
            tr.y = 0x50000;
            tr.z = 0x64000;
            Vec3_RotateYAndTranslate((int *)&out, (int *)(c + 0x5c), *(short *)(c + 0x8e), (int *)&tr);
            cp.x = out.x;
            cp.y = out.y;
            cp.z = out.z;
            func_ov002_020d8d10(c, &cp);
        }
        func_020105cc(c, flags);
        {
            unsigned char b6f9 = *(unsigned char *)(c + 0x6f9);
            unsigned short idx = (unsigned short)(*(int *)(c + 8) + (*(int *)(o + 8) << 2));
            int t98;
            if (b6f9 != 0) idx = 0x11;
            t98 = data_ov002_020ff29c[idx] << 3;
            *(int *)(c + 0x98) = t98;
            *(unsigned char *)(c + 0x6de) = 1;
            *(unsigned char *)(c + 0x6df) = 0;
            if (flag != 0) {
                *(int *)(c + 0xa8) = t98 + 0x20000;
                {
                    int *hp = (int *)(int)(((long long)(int)(c + 0x98)) & 0xFFFFFFFFFFFFFFFFLL);
                    int half = *hp >> 1;
                    *hp = half;
                    return half;
                }
            }
            *(int *)(c + 0xa8) = 0x18000;
            return 0x18000;
        }
    }
}
