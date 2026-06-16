typedef signed int s32;
typedef unsigned int u32;
typedef s32 Fix12i;
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir, void* callback);
void func_ov027_02112424(char* c){
  if(*(unsigned char*)(c+0x5e)==0) return;
  *(u32*)(c+0x3c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(u32*)(c+0x3c), 0x111, *(Fix12i*)(c+0x40), *(Fix12i*)(c+0x44), *(Fix12i*)(c+0x48),
    (void*)0, (void*)0);
}
