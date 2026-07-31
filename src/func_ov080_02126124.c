// @symbol func_ov080_02126124
// @emits CrazedCrate_Kill
/* recovered: renamed to Class_Method */
/* daBttBk_c::Kill - recovered from vtable slot identity */
typedef unsigned char u8;
typedef unsigned int u32;
void CrazedCrate_Kill(u32* c){
  *(int*)((char*)c+0x140)=0;
  *(int*)((char*)c+0x144)=0;
  *(int*)((char*)c+0x148)=0;
  *(int*)((char*)c+0x150)=0x20000000;
  *(int*)((char*)c+0x154)=0x8000000;
  *(int*)((char*)c+0x158)=(u8)(*(u32*)((char*)c+8)&0xf)*0x64000+0x64000;
  *(int*)((char*)c+0x15c)=0;
  *(int*)((char*)c+0x160)=0;
  *(int*)((char*)c+0x168)=0x20000000;
  *(int*)((char*)c+0x16c)=0x8000800;
  *(int*)((char*)c+0x170)=(u8)(*(u32*)((char*)c+8)&0xf)*0x64000+0x64000;
  *(int*)((char*)c+0x174)=(u8)((*(u32*)((char*)c+8)>>4)&0xf)*0x64000+0x64000;
  *(int*)((char*)c+0x178)=0;
  *(int*)((char*)c+0x180)=0x20000000;
  *(int*)((char*)c+0x184)=0x800;
  *(int*)((char*)c+0x188)=0;
  *(int*)((char*)c+0x18c)=(u8)((*(u32*)((char*)c+8)>>4)&0xf)*0x64000+0x64000;
  *(int*)((char*)c+0x190)=0;
  *(int*)((char*)c+0x198)=0x20000000;
  *(int*)((char*)c+0x19c)=0;
}
