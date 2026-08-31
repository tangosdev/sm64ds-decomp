/* Deliberately remains a C/manual factory. A natural C++
 * `return new QuestionSwitch` reproduces the instruction bytes, but mwccarm
 * relocates the allocation call to unresolved `_Znwm` instead of the ROM's
 * fBase_c::operator new at 0x02043444 (linkcheck BLIND-1). The explicit call
 * below preserves the allocator identity rather than hiding that ABI wall. */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern void _ZN10dBgW_KcMbgC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern int _ZTV19daObjHatenaSwitch_c[];
int *QuestionSwitch_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x724);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV19daObjHatenaSwitch_c;
        _ZN10dBgW_KcMbgC1Ev((char *)p + 0x324);
        _ZN10dBgW_KcMbgC1Ev((char *)p + 0x4ec);
        _ZN9ModelAnimC1Ev((char *)p + 0x6b4);
    }
    return p;
}
