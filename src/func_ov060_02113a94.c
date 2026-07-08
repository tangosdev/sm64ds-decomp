typedef short s16;
typedef struct Vector3 { int x, y, z; } Vector3;
extern int Vec3_HorzLen(const Vector3* v);
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern short SINE_TABLE[];

void func_ov060_02113a94(void* thiz)
{
    char* c = (char*)thiz;
    *(unsigned char*)(c + 0x41d) = 0;
    if (*(unsigned char*)(c + 0x41c) != 0) return;
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0x60) = *(int*)(c + 0x3b4) - 0x3e8000;
    if (Vec3_HorzLen((Vector3*)(c + 0x5c)) < 0xed8000) return;
    {
        Vector3 zero;
        int a;
        zero.x = 0;
        zero.y = 0;
        zero.z = 0;
        a = (int)(unsigned short)Vec3_HorzAngle(&zero, (Vector3*)(c + 0x5c)) >> 4;
        *(int*)(c + 0x5c) = (short)SINE_TABLE[a * 2] * (short)0xed8;
        *(int*)(c + 0x64) = (short)SINE_TABLE[a * 2 + 1] * (short)0xed8;
    }
}
