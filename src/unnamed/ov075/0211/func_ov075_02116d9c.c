extern int func_0203da9c(void);
extern void func_02012790(int);
extern unsigned short data_020a0e5a[];
int func_ov075_02116d9c(int unused, int id){
  unsigned short x;
  if(id>=0) x = *(unsigned short*)((char*)data_020a0e5a + id*4);
  else x = *(unsigned short*)((char*)data_020a0e5a + func_0203da9c()*4);
  if(x!=0){
    if(x&0xc) return 1;
    func_02012790(0xe);
  }
  return 0;
}
