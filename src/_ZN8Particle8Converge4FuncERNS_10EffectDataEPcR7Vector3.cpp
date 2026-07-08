//cpp
extern "C" {
void _ZN8Particle8Converge4FuncERNS_10EffectDataEPcR7Vector3(char* ed, char* p, int* vec){
  vec[0] += (int)*(short*)(ed+0xc) * ((*(int*)(ed+0) - *(int*)(p+0x14)) - *(int*)(p+0x20)) >> 12;
  *(int*)(((long long)(int)(vec+1)) & 0xFFFFFFFFFFFFFFFFLL) += (int)*(short*)(ed+0xc) * ((*(int*)(ed+4) - *(int*)(p+0x18)) - *(int*)(p+0x24)) >> 12;
  *(int*)(((long long)(int)(vec+2)) & 0xFFFFFFFFFFFFFFFFLL) += (int)*(short*)(ed+0xc) * ((*(int*)(ed+8) - *(int*)(p+0x1c)) - *(int*)(p+0x28)) >> 12;
}
}
