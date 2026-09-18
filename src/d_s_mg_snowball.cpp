//cpp
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  1 func_ov006_*                unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  1 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 */

extern "C" void* _ZN7fBase_cnwEj(unsigned int);
extern "C" void* func_ov006_021295ac(void*);
/* Reconstructed source-style name: SM64DS proves dScMgSnowball_c through RTTI,
 * allocation size, vtable identity, and the MG_SNOWBALL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgSnowballSlalom_Spawn. */
extern "C" void* dScMgSnowball_c_classInit(void)
{
    void* p = _ZN7fBase_cnwEj(0xc59c);
    if (p) {
        return func_ov006_021295ac(p);
    }
    return p;
}
