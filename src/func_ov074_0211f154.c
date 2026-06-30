/* func_ov074_0211f154 at 0x0211f154
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov074).
 */
struct Vector3 { int x, y, z; };
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(void* a, void* m, void* b);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern void* data_0209f318;
extern int data_020a0e68[];

void func_ov074_0211f154(char* c) {
    struct Vector3 look, pos, in, out;
    void* cam;
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    look.x = *(int*)(c + 0x5c);
    look.y = *(int*)(c + 0x60);
    look.z = *(int*)(c + 0x64);
    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    look.y += 0x1a0000;
    in.z = 0x59c000;
    Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.y = pos.y + 0x300000;
    pos.z = pos.z + out.z;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);
}