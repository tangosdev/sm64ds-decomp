extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern int data_ov006_0213a338[];
extern int data_ov006_0213a568[];
extern int data_ov006_0213a4c0[];

typedef struct { int a; int b; } Pair8;

void func_ov006_020d47f4(char* c){
  int i,j;
  if(*(unsigned char*)(c+0x53dd)==0){
    func_ov004_020afdd0((void*)data_ov006_0213a338[0],0x80,0xb0,-1,0);
  }
  for(i=0;i<4;i++){
    func_ov004_020afdd0((void*)data_ov006_0213a568[((int*)(c+0x46a4))[i]],
                        ((Pair8*)(c+0x4660))[i].a,
                        ((Pair8*)(c+0x4660))[i].b,
                        -1,0);
  }
  for(j=0;j<4;j++){
    func_ov004_020afdd0((void*)data_ov006_0213a4c0[((int*)(c+0x4714))[j]],
                        0x20+j*0x40,0x78,-1,0);
  }
}
