typedef short s16;
typedef struct Vector3 { int x, y, z; } Vector3;
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern int RandomIntInternal(int* seed);
extern Vector3 data_ov027_02113d10;
extern int RNG_STATE;

int func_ov027_021124e4(void* thiz, void* other)
{
    char* c = (char*)thiz;
    int ang;
    if (*(unsigned char*)(c + 0x5e) != 0) return 0;
    *(int*)(c + 0x40) = data_ov027_02113d10.x;
    *(int*)(c + 0x44) = data_ov027_02113d10.y;
    *(int*)(c + 0x48) = data_ov027_02113d10.z;
    ang = Vec3_HorzAngle((Vector3*)(c + 0x40), (Vector3*)((char*)other + 0x5c));
    if (ang > 0x2af8) ang = 0x2af8;
    if (ang < 0x9de) ang = 0x9de;
    {
        int r = RandomIntInternal(&RNG_STATE);
        *(short*)(c + 0x58) = 0xc1c;
        *(short*)(c + 0x5a) = (short)(ang + ((r & 0x1fff) - 0x1000));
    }
    *(short*)(c + 0x5c) = 0;
    *(unsigned char*)(c + 0x5e) = 0x16;
    return 1;
}
