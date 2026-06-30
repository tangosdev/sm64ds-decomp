//cpp
struct Vector3 { int x, y, z; };
struct Actor;

extern "C" {
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, const Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, const Vector3* v);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern unsigned char IsAreaShowing(int idx);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern short data_02082214[];
}

struct RaycastLine {
    char pad[0x78];
    RaycastLine();
    ~RaycastLine();
    int DetectClsn();
    void SetObjAndLine(const Vector3& a, const Vector3& b, Actor* obj);
};

extern "C" void func_ov002_020e7934(char* self, void* cam)
{
    Vector3 vec[2];
    int flag;
    int dist;
    unsigned int r6 = 0;

    vec[0].x = *(int*)(self + 0x5c);
    vec[0].y = *(int*)(self + 0x60);
    vec[0].z = *(int*)(self + 0x64);
    _ZN6Camera9SetLookAtERK7Vector3(cam, &vec[0]);

    vec[0].y += 0xc8000;
    vec[1] = vec[0];
    dist = Vec3_Dist(&vec[0], (Vector3*)(self + 0x46c));

    unsigned short mode = *(unsigned short*)(self + 0x496);
    flag = (((mode == 100 && (dist > 0x3e8000 || dist < 0x1f4000)) ||
             (mode != 100 && dist > 0x3e8000)) &&
            IsAreaShowing(*(signed char*)(self + 0x499))) ? 1 : 0;

    while (1) {
        if (r6 >= 0x10)
            vec[0].y -= 0x258000;
        else if (r6 >= 0xc)
            vec[0].y -= 0x12c000;
        else if (r6 >= 8)
            vec[0].y += 0x258000;
        else if (r6 >= 4)
            vec[0].y += 0x12c000;

        if (flag) {
            short ang = Vec3_HorzAngle(&vec[0], (Vector3*)(self + 0x46c));
            int k = (unsigned short)(short)(ang + ((r6 & 3) << 14)) >> 4;
            vec[0].x = data_02082214[k * 2] * 1000 + vec[0].x;
            vec[0].z = data_02082214[k * 2 + 1] * 1000 + vec[0].z;
            _ZN6Camera6SetPosERK7Vector3(cam, &vec[0]);
        } else {
            if (!IsAreaShowing(*(signed char*)(self + 0xcc)))
                return;
            short ang = Vec3_HorzAngle(&vec[0], (Vector3*)(self + 0x46c));
            int k = (unsigned short)(short)(ang + ((r6 & 3) << 14)) >> 4;
            vec[0].x += (int)(((long long)dist * data_02082214[k * 2] + 0x800) >> 12);
            vec[0].z += (int)(((long long)dist * data_02082214[k * 2 + 1] + 0x800) >> 12);
            _ZN6Camera6SetPosERK7Vector3(cam, &vec[0]);
        }

        if (!IsAreaShowing(*(signed char*)(self + 0xcc)))
            return;

        {
            RaycastLine rl;
            Vector3 a;
            Vector3 b;
            a.x = *(int*)(self + 0x5c);
            a.y = *(int*)(self + 0x60);
            a.z = *(int*)(self + 0x64);
            b.x = vec[0].x;
            b.y = vec[0].y;
            b.z = vec[0].z;
            rl.SetObjAndLine(a, b, (Actor*)self);
            if (rl.DetectClsn()) {
                r6 = (r6 + 1) & 0xff;
                vec[0] = vec[1];
                if (r6 >= 0x14) {
                    _ZN6Camera6SetPosERK7Vector3(cam, (Vector3*)(self + 0x46c));
                    return;
                }
                continue;
            }
            return;
        }
    }
}
