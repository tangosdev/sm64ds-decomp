//cpp
struct Vector3 { int x, y, z; };
struct BCA_File;

extern "C" {
int func_ov032_02111350(unsigned char* c);
int func_ov032_02111254(unsigned char* c);
void func_ov032_02111ff4(unsigned char* c, void* p);
void ModelAnim_SetAnim(void* self, BCA_File* f, int a, int fix, unsigned int j);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
int RandomIntInternal(int* seed);
void ApproachLinear(short& a, short b, short c);
int Vec3_HorzDist(const Vector3* a, const Vector3* b);
int AngleDiff(int a, int b);
short Vec3_VertAngle(const Vector3* a, const Vector3* b);

extern int data_ov032_02113a50;
extern int data_ov032_02113a8c;
extern int data_ov032_02113a48;
extern int data_ov032_02113a7c;
extern int data_ov032_02113abc;
extern int RNG_STATE;
}

extern "C" int func_ov032_02111b9c(unsigned char* thiz)
{
    if (*(unsigned short*)(thiz + 0x100) != 0) {
        if (func_ov032_02111350(thiz) == 1) goto init;
        if (func_ov032_02111254(thiz) != 0) goto track;
    }
init:
    {
        ModelAnim_SetAnim(thiz + 0x34c,
            *(BCA_File**)((unsigned char*)&data_ov032_02113a50 + 4),
            0, 0x1000, 0);
        *(short*)(thiz + 0x42a) = 0x64;
        func_ov032_02111ff4(thiz, &data_ov032_02113a8c);
        return 1;
    }
track:
    {
        *(short*)(thiz + 0x430) = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x418));
        unsigned int r = (unsigned int)RandomIntInternal(&RNG_STATE);
        int s5 = (int)(((r >> 8) & 3) << 0x1c) >> 0x10;
        ApproachLinear(*(short*)(thiz + 0x94), *(short*)(thiz + 0x430), 0x200);
        if (Vec3_HorzDist((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x418)) < 0x258000) {
            int d = AngleDiff(*(short*)(thiz + 0x8e), *(short*)(thiz + 0x430));
            if (d < (int)(short)(s5 + 0x200)) {
                *(short*)(thiz + 0x430) = Vec3_VertAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x418));
                *(int*)(thiz + 0xb0) = 0;
                *(int*)(thiz + 0x3a8) = 0x1000;
                *(int*)(thiz + 0x434) = *(int*)(thiz + 0x60);
                *(unsigned short*)(thiz + 0x100) = 0;
                *(int*)(thiz + 0x98) = 0x14000;
                thiz[0x429] = 0;
                ModelAnim_SetAnim(thiz + 0x34c,
                    *(BCA_File**)((unsigned char*)&data_ov032_02113a48 + 4),
                    0x40000000, 0x1000, 0);
                if (*(short*)(thiz + 0x430) > 0) {
                    func_ov032_02111ff4(thiz, &data_ov032_02113a7c);
                } else {
                    func_ov032_02111ff4(thiz, &data_ov032_02113abc);
                }
                return 1;
            }
        }
        return 1;
    }
}
