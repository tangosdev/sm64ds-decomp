extern void func_0204fed0(void* p);

struct Ent {
  char pad[0xf0];
  int b0 : 1;
  int flag : 1;
  int brest : 30;
  char pad2[0x12c - 0xf4];
};

extern struct Ent data_020a5bd4[];

void func_02052234(void){
  int i;
  for (i = 0; i < 4; i++){
    if (data_020a5bd4[i].flag) {
      func_0204fed0(&data_020a5bd4[i]);
    }
  }
}
