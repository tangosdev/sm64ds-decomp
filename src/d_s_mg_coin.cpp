//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int sz);
extern int _ZN11dScMgBase_cC2Ev(void* p);
extern void func_0203b9b4(int *p, int v);
extern int data_ov006_0213bf50[];
/* Reconstructed source-style name: SM64DS proves dScMgCoin_c through RTTI,
 * allocation size, vtable identity, and the MG_COIN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgCoincentration_Spawn. */
void* dScMgCoin_c_classInit(void){
  char* o = (char*)_ZN7fBase_cnwEj(0x51e0);
  if(o != 0){
    _ZN11dScMgBase_cC2Ev(o);
    *(int*)o = (int)data_ov006_0213bf50;
    func_0203b9b4((int*)(o+0x51c4), 1);
  }
  return o;
}
}
