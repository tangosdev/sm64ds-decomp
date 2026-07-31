extern int func_ov004_020adafc(void);
extern int _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player(int);
extern char* data_ov004_020beb68[];
extern int data_0209b308[];
void func_ov004_020adb1c(int self){
  char* r2 = data_ov004_020beb68[0];
  if(r2 == 0) return;
  if(data_0209b308[2] == 5){
    *(int*)(r2 + 0x4000 + 0x64c) = self;
    if(func_ov004_020adafc() != 0){
      if((unsigned)self >= (unsigned)func_ov004_020adafc()) return;
    }
    _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player(self);
  } else {
    int o = data_0209b308[3];
    if(o != 0){
      if((unsigned)self > (unsigned)o) self = o;
    }
    *(int*)(r2 + 0x4000 + 0x64c) = self;
    if((unsigned)self <= (unsigned)func_ov004_020adafc()) return;
    _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player(self);
  }
}
