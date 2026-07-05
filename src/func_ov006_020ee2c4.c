extern void func_ov006_020c712c(void);
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b0a54(void* c);
extern int data_ov006_02140434;
extern int data_ov006_02140418;
extern char* func_020beb68;

typedef struct { int a, b; } P;
extern P data_ov006_0213cb7c;

void func_ov006_020ee2c4(char* c){
  *(int *)(((int)c + 0x500c) & 0xFFFFFFFFFFFFFFFF) += *(int*)(c + 0x5010) >> 12;
  *(int *)(((int)c + 0x5010) & 0xFFFFFFFFFFFFFFFF) += 0x600;
  func_ov006_020c712c();
  int eq = (int)(data_ov006_02140434 == data_ov006_02140418);
  if(eq == 0) return;
  char* g = func_020beb68;
  if(g != 0){
    if(*(int*)(g+0xb4) < 0x270f){
      *(int *)(((int)g + 0xb4) & 0xFFFFFFFFFFFFFFFF) += 1;
    }
    if(*(int*)(g+0xb4) > *(int*)(g+0xb8)){
      *(int*)(g+0xb8) = *(int*)(g+0xb4);
    }
  }
  func_ov004_020adb1c(*(int*)(c+0xb4));
  func_ov004_020b0a54(0);
  *(unsigned char*)(c+0xc3) = 0;
  *(short*)(c+0x5014) = 0;
  *(P*)(c+0x5004) = data_ov006_0213cb7c;
}
