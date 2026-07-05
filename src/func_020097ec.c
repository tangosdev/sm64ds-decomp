typedef struct Vec3i { int x, y, z; } Vec3i;

typedef struct Other {
    char pad0[0x5c];
    Vec3i vec;            /* 0x5c */
    char pad1[0x8c - 0x68];
    short s8c;            /* 0x8c */
    short s8e;            /* 0x8e */
} Other;

typedef struct Obj {
    char pad0[0x8c];
    Vec3i vec;            /* 0x8c */
    char pad1[0x110 - 0x98];
    Other *other;         /* 0x110 */
    char pad2[0x17c - 0x114];
    short s17c;           /* 0x17c */
    short s17e;           /* 0x17e */
    char pad3[0x186 - 0x180];
    short s186;           /* 0x186 */
    char pad4[0x19c - 0x188];
    short s19c;           /* 0x19c */
    short s19e;           /* 0x19e */
} Obj;

extern void func_0200cb58(Obj *obj, int index);

int func_020097ec(Obj *obj)
{
    Vec3i *src;

    func_0200cb58(obj, 0x18);
    src = (Vec3i *)(((int)obj->other + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    obj->vec.x = src->x;
    obj->vec.y = src->y;
    obj->vec.z = src->z;
    obj->s19e = obj->other->s8e + 0x8000;
    obj->s17c = obj->s19e;
    obj->s186 = 0;
    obj->s17e = obj->other->s8c;
    obj->s19c = 0;
    return 1;
}
