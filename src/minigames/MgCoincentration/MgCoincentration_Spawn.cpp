//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern int func_ov004_020b2adc(void* p);
extern void func_0203b9b4(int *p, int v);
extern int data_ov006_0213bf50[];
void* MgCoincentration_Spawn(void){
  char* o = (char*)_ZN7fBase_cnwEj(0x51e0);
  if(o != 0){
    func_ov004_020b2adc(o);
    *(int*)o = (int)data_ov006_0213bf50;
    func_0203b9b4((int*)(o+0x51c4), 1);
  }
  return o;
}
}
