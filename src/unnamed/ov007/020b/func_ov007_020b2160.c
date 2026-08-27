typedef int Fix12;
struct Matrix4x3;

extern void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    Fix12, int, int, int, int, int, int, struct Matrix4x3*);
extern void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(
    void*, void*, void*, int, void*);
extern void func_ov007_020bff38(char* c);
extern void func_ov007_020bffb8(char* c);

extern char* data_ov007_0210342c;
extern short data_02082214[];

void func_ov007_020b2160(int mode)
{
    int a;

    switch (mode)
    {
    case 1:
        a = (int)*(unsigned short*)(*(char**)(data_ov007_0210342c + 0x34) + 0xd4) >> 4;
        _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
            data_02082214[a * 2],
            data_02082214[a * 2 + 1],
            (int)(*(long long*)(*(char**)(data_ov007_0210342c + 0x34) + 0xbc) >> 0x14),
            *(int*)(*(char**)(data_ov007_0210342c + 0x34) + 0xc4),
            *(int*)(*(char**)(data_ov007_0210342c + 0x34) + 0xc8),
            *(int*)(*(char**)(data_ov007_0210342c + 0x34) + 0xb4),
            1,
            (struct Matrix4x3*)(*(char**)(data_ov007_0210342c + 0x34) + 0x74));
        _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(
            *(char**)(data_ov007_0210342c + 0x34) + 8,
            *(char**)(data_ov007_0210342c + 0x34) + 0x20,
            *(char**)(data_ov007_0210342c + 0x34) + 0x14,
            1,
            *(char**)(data_ov007_0210342c + 0x34) + 0x44);
        break;
    case 0:
        a = (int)*(unsigned short*)(*(char**)(data_ov007_0210342c + 0x30) + 0xd4) >> 4;
        _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
            data_02082214[a * 2],
            data_02082214[a * 2 + 1],
            (int)(*(long long*)(*(char**)(data_ov007_0210342c + 0x30) + 0xbc) >> 0x14),
            *(int*)(*(char**)(data_ov007_0210342c + 0x30) + 0xc4),
            *(int*)(*(char**)(data_ov007_0210342c + 0x30) + 0xc8),
            0x1000,
            1,
            (struct Matrix4x3*)0);
        func_ov007_020bff38(*(char**)(data_ov007_0210342c + 0x30));
        func_ov007_020bffb8(*(char**)(data_ov007_0210342c + 0x30));
        _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(
            *(char**)(data_ov007_0210342c + 0x30) + 8,
            *(char**)(data_ov007_0210342c + 0x30) + 0x20,
            *(char**)(data_ov007_0210342c + 0x30) + 0x14,
            1,
            *(char**)(data_ov007_0210342c + 0x30) + 0x44);
        break;

    }
}
