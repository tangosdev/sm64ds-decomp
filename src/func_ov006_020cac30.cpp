//cpp
// @symbol func_ov006_020cac30
/* ov006, 0x020cac30, size 0x6c. Held the name
 * _ZN6Player12St_Null_InitEv until that symbol was moved to ov002, where an
 * eight-byte `return 1` sits at the same shared address and is reached from
 * ov002's Player::State pointer-to-member table. This function is reached
 * instead by two direct arm_calls from ov006 (module:overlay(6)) and touches
 * no Player field, so nothing here was ever Player's. Back to a placeholder
 * name until its own class is identified.
 */
extern "C" {
extern int data_ov006_021405bc;
extern char* data_ov006_02140554;
void func_ov006_020c8f20(void*);
void func_ov006_020ce46c(void*,int);
void func_ov006_020c8a64(void);
void func_ov006_020cac30(void){
  int i=0;
  if(data_ov006_021405bc>0){
    int off=0;
    do{
      func_ov006_020c8f20(data_ov006_02140554+off);
      func_ov006_020ce46c(data_ov006_02140554+off,i);
      i++;
      off+=0xdc;
    }while(i<data_ov006_021405bc);
  }
  func_ov006_020c8a64();
}
}
