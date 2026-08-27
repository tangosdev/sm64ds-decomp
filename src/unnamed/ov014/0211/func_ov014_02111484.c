extern int DecIfAbove0_Short(void*);
extern int func_ov014_02111ebc(void*, int);
extern int _Z14ApproachLinearRiii(int*, int, int);
void func_ov014_02111484(char* c){
  if(DecIfAbove0_Short(c+0x5fc)==0)
    func_ov014_02111ebc(c, 1);
  if(*(unsigned char*)(c+0x61c)!=0)
    *(int*)(c+0xa8)=*(int*)(c+0x98);
  _Z14ApproachLinearRiii((int*)(c+0x98), 0, 0x2000);
}
