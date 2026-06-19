extern void func_ov004_020adf70(void* tbl, char* c, short a1, short a2, int* r3, int* sp0);
struct E { char pad[0x1a]; short f; char pad2[0x20-0x1c]; };
extern struct E data_ov004_020bebe8[];
void func_ov004_020adfc4(char* c, short a1, short a2, int* r3, int* sp0, int* sp4){
  if(a2==0) return;
  int i;
  for(i=0;i<0x40;i++){
    if(data_ov004_020bebe8[i].f == 0){
      func_ov004_020adf70(&data_ov004_020bebe8[i], c, a1, a2, r3, sp0);
      return;
    }
  }
}
