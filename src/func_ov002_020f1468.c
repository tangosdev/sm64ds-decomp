typedef int Fix12i;
typedef short s16;

struct SharedFilePtr;

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* m, void* f, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* mc, void* f, void* mtx, Fix12i fx, short s, void* clps);
extern void func_020393c4(void* p, void* v);

extern char data_ov002_02110acc;
extern char data_ov002_02110ac4;
extern char data_ov002_0210d6f4;
extern void func_ov002_020f15b8(void);

int func_ov002_020f1468(char* c)
{
    void* bmd;
    void* kcl;
    int y;
    unsigned int x;
    unsigned short val;
    unsigned short* p;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_02110acc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, -1);

    *(unsigned char*)(c + 0x32d) = *(int*)(c + 8) & 0xf;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);

    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov002_02110ac4);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, kcl, c + 0x2ec, 0x199, *(s16*)(c + 0x8e),
        &data_ov002_0210d6f4);

    func_020393c4(c + 0x124, (void*)&func_ov002_020f15b8);

    *(unsigned char*)(c + 0x32c) = 0;

    /* Keep p out of the mid-block so y colors r1 and c+0x300 colors r0,
       and so set_fa rematerializes add r0,r4,#0x300 after ldrh clobbers r0. */
    y = *(int*)(c + 0x60);
    *(int*)((char*)c + 0x320) = y - 0x64000;
    x = *(unsigned int*)(c + 8);
    *(unsigned short*)(c + 0x32a) = (x >> 8) & 0xff;
    *(unsigned char*)(c + 0x32e) = *(signed char*)(c + 0xcc);
    val = *(unsigned short*)(c + 0x32a);
    p = (unsigned short*)(c + 0x300);

    /* ROM: if (val==0xff || val==0) store 0xfa; else *= 10 */
    if (val == 0xff)
        goto set_fa;
    if (val != 0)
        goto multiply;

set_fa:
    p = (unsigned short*)(c + 0x300);
    p[0x15] = 0xfa;
    goto done;

multiply:
    *(unsigned short*)(((int)c + 0x32a) & 0xFFFFFFFFFFFFFFFFULL) =
        *(unsigned short*)(((int)c + 0x32a) & 0xFFFFFFFFFFFFFFFFULL) * 0xa;

done:
    return 1;
}
