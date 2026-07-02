typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

typedef struct { int x, y, z; } Vec3;

enum Bool { FALSE, TRUE };

extern void SetAnim(void* self, void* f, int a, int b, int fix, u32 j);
extern void func_02012694(int a, void* p);
extern void ApproachLinear(short* a, short b, short c);
extern s16 Vec3_HorzAngle(const void* a, const void* b);
extern s16 Vec3_VertAngle(const void* a, const void* b);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* a, void* b, void* c);
extern void* Actor_FindWithID(u32 id);
extern void Player_Unk(void* self, u32 a);
extern int WithMeshClsn_IsOnGround(void* self);
extern void func_ov073_0211f2c0(void* self, int a);
extern void func_ov073_0212157c(void* self, void* p);

extern void* data_ov073_021232b0[];
extern int data_020a0e68[];
extern void* data_ov073_02123290[];
extern void* data_ov073_02123380[];

int func_ov073_021200e0(u8* thiz)
{
    u16 state = *(u16*)(thiz + 0x100);
    if (state != 0) {
        if (state == 1) {
            SetAnim(thiz + 0x30c, data_ov073_021232b0[1], 4, 0x40000000, 0x1000, 0);
            *(int*)(thiz + 0x368) = 0x1000;
            *(int*)(thiz + 0xa8) = 0x46000;
            *(u8*)(thiz + 0x4c5) = 0xff;
            *(int*)(thiz + 0x9c) = -0x3000;
            if (*(u8*)(thiz + 0x4ca) != 0) {
                func_02012694(0x16f, thiz + 0x74);
            }
        }
        return 1;
    }
    /* state == 0 */
    if (*(int*)(thiz + 0x4b4) == 0) {
        Vec3 offset;
        Vec3 rotated;
        s16 horz;
        ApproachLinear((short*)(thiz + 0x8c), 0, 0x2000);
        horz = Vec3_HorzAngle((const void*)(thiz + 0x5c), (const void*)(thiz + 0x3d8));
        Vec3_VertAngle((const void*)(thiz + 0x5c), (const void*)(thiz + 0x3d8));
        ApproachLinear((short*)(thiz + 0x94), horz, 0x800);
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        if (*(u8*)(thiz + 0x4cb) == 2) {
            offset.z = 0x32000;
        } else {
            offset.z = 0x1e000;
        }
        Matrix4x3_FromRotationY(data_020a0e68, horz);
        MulVec3Mat4x3(&offset, data_020a0e68, &rotated);
        *(int*)(thiz + 0xa4) = rotated.x;
        *(int*)(thiz + 0xac) = rotated.z;
        if (*(int*)(thiz + 0xa8) < 0) {
            int id = *(int*)(thiz + 0x134);
            if (id != 0) {
                void* actor = Actor_FindWithID((u32)id);
                if (actor != 0) {
                    enum Bool eq = (enum Bool)(*(u16*)((u8*)actor + 0xc) == 0xbf);
                    if (eq != FALSE) {
                        Vec3 pos = *(Vec3*)((u8*)actor + 0x5c);
                        if (*(int*)(thiz + 0x60) > pos.y) {
                            Player_Unk(actor, 1);
                        }
                    }
                }
            }
        }
    }
    /* 0x1a8 */
    if (WithMeshClsn_IsOnGround(thiz + 0x150) != 0) {
        if (*(int*)(thiz + 0x4b4) == 0) {
            *(int*)(thiz + 0x98) = 0;
            *(int*)(thiz + 0xa4) = 0;
            *(int*)(thiz + 0xa8) = 0;
            *(int*)(thiz + 0xac) = 0;
            func_ov073_0211f2c0(thiz, 0x1388000);
            func_02012694(0x16d, thiz + 0x74);
        }
        if (*(u8*)(thiz + 0x4cb) == 1 && *(u8*)(thiz + 0x4ca) == 0) {
            *(u8*)(thiz + 0x4c5) = 0;
            if (*(int*)(thiz + 0x4b4) == 0) {
                SetAnim(thiz + 0x30c, data_ov073_02123290[1], 4, 0x40000000, 0x1000, 0);
            }
            *(int*)(((int)thiz + 0x4b4) & 0xFFFFFFFFFFFFFFFF) += 1;
            if (*(int*)(thiz + 0x4b4) < 0x82) {
                return 1;
            }
        }
        func_ov073_0212157c(thiz, data_ov073_02123380);
    }
    *(s16*)(thiz + 0x8e) = *(s16*)(thiz + 0x94);
    return 1;
}
