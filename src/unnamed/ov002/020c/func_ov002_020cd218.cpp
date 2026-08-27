//cpp
extern "C" {
extern short data_02082214[];
void func_ov002_020cd218(char* c, int v, int* out1, int* out2){
  *out1 = (int)(((long long)v * data_02082214[(*(unsigned short*)(c+0x92) >> 4)*2+1] + 0x800) >> 12);
  *out2 = -(int)(((long long)v * data_02082214[(*(unsigned short*)(c+0x92) >> 4)*2] + 0x800) >> 12);
}
}
