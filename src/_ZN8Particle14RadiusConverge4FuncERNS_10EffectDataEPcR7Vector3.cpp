//cpp
extern "C" {
void _ZN8Particle14RadiusConverge4FuncERNS_10EffectDataEPcR7Vector3(char* ed, char* p, int* vec){
  *(int*)(((long long)(int)(p+0x14)) & 0xFFFFFFFFFFFFFFFFLL) += (int)((((long long)*(short*)(ed+0xc) * (*(int*)(ed+0) - *(int*)(p+0x14))) + 0x800) >> 12);
  *(int*)(((long long)(int)(p+0x18)) & 0xFFFFFFFFFFFFFFFFLL) += (int)((((long long)*(short*)(ed+0xc) * (*(int*)(ed+4) - *(int*)(p+0x18))) + 0x800) >> 12);
  *(int*)(((long long)(int)(p+0x1c)) & 0xFFFFFFFFFFFFFFFFLL) += (int)((((long long)*(short*)(ed+0xc) * (*(int*)(ed+8) - *(int*)(p+0x1c))) + 0x800) >> 12);
}
}
