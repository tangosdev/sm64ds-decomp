//cpp
extern "C" {
void func_ov007_020bc02c(void* a, void* b);
void func_ov007_020be9ac(void* a, void* b, void* c);
void func_ov007_020bbff0(void* c);
}
extern char* data_ov007_0210342c;
#define LAUNDER(x) ((char*)(int)(((long long)(int)(x)) & 0xffffffffffffffffLL))

extern "C" void func_ov007_020b2370(void){
  char* g = *(char**)&data_ov007_0210342c;
  void* f4 = *(void**)(g + 0xf4);
  void** arr = (void**)LAUNDER(g + 0x100);
  func_ov007_020bc02c(f4, *(char**)(g + 0x30) + 0x44);
  {
    char* g2 = *(char**)&data_ov007_0210342c;
    func_ov007_020be9ac(arr[0], *(void**)(g2 + 0xf4), *(char**)(g2 + 0x30) + 0x44);
  }
  {
    char* g2 = *(char**)&data_ov007_0210342c;
    func_ov007_020be9ac(arr[1], *(void**)(g2 + 0xf4), *(char**)(g2 + 0x30) + 0x44);
  }
  {
    char* g2 = *(char**)&data_ov007_0210342c;
    int t = *(int*)(g2 + 0xf8);
    long long v = ((long long)(0x1000 - t) << 8) + ((long long)t << 12);
    int res = (int)(v >> 12);
    *(int*)((char*)f4 + 0x48) = res;
    *(int*)((char*)f4 + 0x44) = *(int*)((char*)f4 + 0x48);
    *(int*)((char*)f4 + 0x40) = *(int*)((char*)f4 + 0x44);
  }
  func_ov007_020bbff0(f4);
  func_ov007_020bbff0(*(void**)((char*)arr[0] + 4));
  func_ov007_020bbff0(*(void**)((char*)arr[1] + 4));
}
