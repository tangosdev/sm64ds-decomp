//cpp
extern "C" {
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
extern void* data_ov002_0210b998[];
void func_ov002_020f3978(char* c){
  int i;
  for(i = 0; i < 4; i++){
    if(*(unsigned char*)(c+0x45)){
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, data_ov002_0210b998[i],
        *(int*)c >> 0xc, *(int*)(c+4) >> 0xc, -1, 1, 0);
    }
    c += 0x4c;
  }
}
}
