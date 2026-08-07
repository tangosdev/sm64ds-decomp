#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern int data_ov006_0213a338[];
extern int data_ov006_0213a568[];
extern int data_ov006_0213a4c0[];

void func_ov006_020d47f4(char* c){
  int fix = -1;
  int i;
  int j;
  int x;
  if(*(unsigned char*)(c+0x53dd)==0){
    func_ov004_020afdd0((void*)data_ov006_0213a338[0],0x80,0xb0,fix,0);
  }
  for(i=0;i<4;i++){
    func_ov004_020afdd0((void*)data_ov006_0213a568[((int*)(c+0x46a4))[i]],
                        *(int*)(c+i*8+0x4660),
                        *(int*)(c+i*8+0x4664),
                        fix,0);
  }
  j = 0;
  x = 0x20;
  for(; j < 4; x += 0x40, j++){
    func_ov004_020afdd0((void*)data_ov006_0213a4c0[((int*)(c+0x4714))[j]],
                        x,0x78,fix,0);
  }
}
