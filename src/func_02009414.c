struct Vector3 {
    int x, y, z;
};

extern int func_02008b4c(void* a0, void* p1, void* p2, void* tbl);
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern short Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);
extern void func_020089f8(void *obj);

int func_02009414(char *obj) {
    int r4 = 1;
    void *tbl1 = *(void**)(obj + 0x150);
    
    if (tbl1 != 0) {
        r4 = func_02008b4c(obj + 0x8c, obj + 0x198, obj + 0x16c, tbl1);
    }
    
    void *tbl2 = *(void**)(obj + 0x14c);
    if (tbl2 != 0) {
        if (func_02008b4c(obj + 0x80, obj + 0x198, obj + 0x16c, tbl2) == 0) {
            r4 = 0;
        }
    }
    
    *(short*)(obj + 0x17c) = Vec3_HorzAngle((struct Vector3*)(obj + 0x80), (struct Vector3*)(obj + 0x8c));
    *(short*)(obj + 0x17e) = Vec3_VertAngle((struct Vector3*)(obj + 0x80), (struct Vector3*)(obj + 0x8c));
    
    func_020089f8(obj);
    return r4;
}
