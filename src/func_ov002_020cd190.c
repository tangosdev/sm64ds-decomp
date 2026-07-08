extern void func_ov002_020ce8bc(void* c, int x);
extern int WATER_HEIGHT;
void func_ov002_020cd190(char* c){
  if(*(unsigned char*)(c+0x703)==0) return;
  if(*(int*)(c+0x60) + 0x180000 <= WATER_HEIGHT) return;
  func_ov002_020ce8bc(c, *(int*)(c+0x98));
}
