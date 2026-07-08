struct Vector3 { int x, y, z; };

extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, struct Vector3* v, void* rot, int e, int f);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* p);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* p, void* f, int a, int b);
extern void func_ov010_0211146c(char* c);
extern void func_ov010_021113f0(char* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* p);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* p, void* f, void* m, int fix, short s, void* clps);
extern void func_020393c4(int* p, int v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* p, void* a);

extern short SINE_TABLE[];
extern void* data_ov010_02112d08;
extern void* data_ov010_02112d00;
extern void* data_ov010_021122f8;
extern int func_ov010_02111984[];

int func_ov010_02111654(char* c)
{
    *(unsigned char*)(c + 0x3aa) = 0;
    *(int*)(c + 0x3ac) = 0;

    if ((*(int*)(c + 8) & 0xff) == 0xff) {
        struct Vector3 v;
        int idx;
        int sx, sz;
        int x, y, z;
        void* sp;

        *(unsigned char*)(c + 0x3ab) = 1;
        *(int*)(c + 0x3a4) = 0;

        idx = ((int)(*(unsigned short*)(c + 0x8e)) >> 4) * 2;
        sx = SINE_TABLE[idx + 1];
        sz = SINE_TABLE[idx];
        z = *(int*)(c + 0x64) + sz * 0x15d;
        x = *(int*)(c + 0x5c) - sx * 0x15d;
        y = *(int*)(c + 0x60);
        v.x = x;
        v.y = y;
        v.z = z;
        sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x24, 0, &v, c + 0x8c, *(signed char*)(c + 0xcc), -1);
        *(int*)((char*)sp + 0x3ac) = *(int*)(c + 4);

        idx = ((int)(*(unsigned short*)(c + 0x8e)) >> 4) * 2;
        sz = SINE_TABLE[idx];
        sx = SINE_TABLE[idx + 1];
        z = *(int*)(c + 0x64) - sz * 0x15d;
        x = sx * 0x15d + *(int*)(c + 0x5c);
        y = *(int*)(c + 0x60);
        v.x = x;
        v.y = y;
        v.z = z;
        sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x24, 1, &v, c + 0x8c, *(signed char*)(c + 0xcc), -1);
        *(int*)((char*)sp + 0x3ac) = *(int*)(c + 4);

        return 1;
    }

    *(unsigned char*)(c + 0x3ab) = 0;
    {
        void* f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov010_02112d08);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x320, f, 1, -1);
    }
    func_ov010_0211146c(c);
    func_ov010_021113f0(c);
    {
        void* f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov010_02112d00);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, f, c + 0x370, 0x1000, *(short*)(c + 0x8e), &data_ov010_021122f8);
    }
    func_020393c4((int*)(c + 0x124), (int)func_ov010_02111984);
    _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);
    *(unsigned short*)(c + 0x3a8) = 0;
    *(int*)(c + 0x3a0) = 0;

    if ((*(int*)(c + 8) & 0xff) == 1) {
        short* pa = (short*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
        *pa = *pa + 0x8000;
    }
    return 1;
}
