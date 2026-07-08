typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef long long s64;
struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };
extern u16 DecIfAbove0_Short(u16* p);
extern s16 Vec3_VertAngle(const struct Vector3* v1, const struct Vector3* v0);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const struct Vector3* pos, const struct Vector3_16* ang, int e, int f);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern s16 SINE_TABLE[];
extern u8 data_ov074_02122d80[];

int func_ov074_0211f38c(u8* c){
    struct Vector3 p0, p1;
    struct Vector3_16 ang;
    int mult;
    int r5;
    void* pl;
    p0.x=0; p0.y=0; p0.z=0;
    ang.x=0; ang.y=0; ang.z=0;
    p0.x=*(int*)(c+0x5c);
    p0.y=*(int*)(c+0x60);
    p0.z=*(int*)(c+0x64);
    p0.y+=0xe4000;
    p1.x=p0.x;
    p1.y=p0.y;
    p1.z=p0.z;
    ang.y=(short)*(short*)(c+0x8e);
    if(DecIfAbove0_Short((u16*)(c+0x5fc))==0){
        u8 t=c[0x602];
        if(t<=6){
            ang.y=(short)(t*0x2492+ang.y);
            mult=0x18c000;
        } else if(t<=0xb){
            p0.y+=0xc4000;
            ang.y=(short)((c[0x602]-7)*0x3333+ang.y);
            mult=0x108000;
        } else if(t>0xe){
            p0.y+=0x24c000;
            mult=0;
        } else {
            p0.y+=0x188000;
            ang.y=(short)((c[0x602]-0xc)*0x5555+ang.y);
            mult=0x84000;
        }
        {
            int s=((u16)ang.y>>4)<<1;
            p0.x += (int)(((s64)mult * SINE_TABLE[s] + 0x800) >> 0xc);
            p0.z += (int)(((s64)mult * SINE_TABLE[s+1] + 0x800) >> 0xc);
        }
        ang.x=Vec3_VertAngle(&p1, &p0);
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xc7, 0x1111, &p0, &ang, (signed char)c[0xcc], -1);
        {
            u8* pc=(u8*)(((long long)(int)(c+0x602))&0xFFFFFFFFFFFFFFFFLL);
            *pc+=1;
        }
        *(short*)((c+0x500)+0xfc)=2;
    }
    r5=data_ov074_02122d80[c[0x604]];
    pl=_ZN5Actor13ClosestPlayerEv(c);
    if(c[0x604]==1){ if(*(int*)((char*)pl+8)!=3) r5+=1; }
    return c[0x602]>=r5 ? 1 : 0;
}
