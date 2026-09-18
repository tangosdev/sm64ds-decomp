//cpp
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  1 func_*_* + 1 data_*         unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  2 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 *  ~1 *(T *)(p + 0x..)           class layout does not name these offsets.
 */

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
