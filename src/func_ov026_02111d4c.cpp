//cpp
#pragma opt_propagation off
struct Vector3 { int x, y, z; };

struct Player {
    char pad[0x5c];
    Vector3 pos;
    char pad2[0x6f9 - 0x68];
    unsigned char flag6f9;
    void EnterWhirlpool();
};
struct Actor {
    Player* ClosestPlayer();
    int HorzAngleToCPlayer();
};

extern "C" int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern "C" int Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" void func_ov026_02111ee0(void* c, void* p);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern "C" void MulVec3Mat4x3(const Vector3* in, const void* m, Vector3* out);

extern void* data_ov026_02113f3c;
extern char data_020a0e68;

extern "C" int func_ov026_02111d4c(char* c)
{
    Player* pl = ((Actor*)c)->ClosestPlayer();
    if (pl != 0) {
        Vector3 v;
        Vector3* p0 = (Vector3*)(((int)pl + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
        v = *p0;

        if (Vec3_HorzDist((Vector3*)(c + 0x1a8), &v) <= 0x12c000) {
            int e = *(int*)(c + 0x1ac) - v.y;
            if (e < 0) e = -e;
            if (e <= 0x12c000) {
                pl->EnterWhirlpool();
                func_ov026_02111ee0(c, &data_ov026_02113f3c);
                return 1;
            }
        }

        if (pl->flag6f9 == 0) {
            int dd = Vec3_Dist((Vector3*)(c + 0x1a8), &v);
            if (dd < 0x44c000) {
                int q;
                int t;
                Vector3 m;
                Vector3 out;
                t = 0x44c000 - dd;
                q = t / 35;
                m.x = 0;
                m.y = 0;
                m.z = q;
                out.x = 0;
                out.y = 0;
                out.z = 0;

                Matrix4x3_FromRotationY(&data_020a0e68, (short)(((Actor*)c)->HorzAngleToCPlayer() + 0x8000));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x2000);
                MulVec3Mat4x3(&m, &data_020a0e68, &out);

                Vector3* p1 = (Vector3*)(((int)(((int)pl) & 0xFFFFFFFFFFFFFFFFull) + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
                m = *p1;
                m.x += out.x;
                m.y += out.y;
                m.z += out.z;
                pl->pos = m;
            }
        }
    }
    return 1;
}
