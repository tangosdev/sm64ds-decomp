typedef unsigned int u32;
typedef int Fix12i;
typedef short s16;
typedef unsigned char u8;

struct Vec3 { Fix12i x, y, z; };

extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_RotateYAndTranslate(struct Vec3* out, void* m, s16 ang, struct Vec3* in);
extern void ChangeArea(int areaID);
extern u8 data_0209f250;
extern char* data_0209f394[];
extern char data_020a0ebc;

int _ZN9CameraTag8BehaviorEv(char* c)
{
    char* obj;
    struct Vec3 d;
    struct Vec3 r;
    int v;

    obj = data_0209f394[data_0209f250];
    Vec3_Sub(&d, (struct Vec3*)(obj + 0x5c), (struct Vec3*)(c + 0x5c));
    Vec3_RotateYAndTranslate(&r, &data_020a0ebc, *(s16*)(c + 0x8e), &d);

    v = r.x;
    if (v < 0) v = -v;
    if (v < *(int*)(c + 0x80)) {
        if (r.y > -0x96000) {
            if (r.y < *(int*)(c + 0x84)) {
                int z = r.z;
                int az = (z < 0) ? -z : z;
                if (az > 0x64000 && az < 0x190000) {
                    int area = (z < 0) ? *(s16*)(c + 0x8c) : *(s16*)(c + 0x90);
                    *(char*)(obj + 0xcc) = (char)area;
                    ChangeArea((char)area);
                }
            }
        }
    }
    return 1;
}
