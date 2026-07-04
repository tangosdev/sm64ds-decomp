//cpp
extern "C" {
extern void* func_0205a588(void*, int, int);
void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(void* self, void* fsd){
  func_0205a588(self, 0, 0x44);
  *(int*)self = 0x30303038;
  *(unsigned char*)((char*)self + 0x41) = 3;
  *(int*)(((long long)(int)((char*)self + 8)) & 0xFFFFFFFFFFFFFFFFLL) |= 8;
  *(unsigned char*)((char*)self + 0x42) = 0;
}
}
