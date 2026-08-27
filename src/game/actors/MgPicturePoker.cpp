//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov006/MgPicturePoker  (3 function(s))
 *
 * The default stock build compiles this file once, derives its 3
 * exact text contributions, and substitutes them for retained one-function
 * comparison objects. The manifest partial-link proof and ordinary ROM build
 * both require the linked module and final ROM to remain byte-identical.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020dbd54  src/minigames/MgPicturePoker/MgPicturePoker_Spawn.cpp
 *   [1] 0x020dbe14  src/unnamed/ov006/020d/func_ov006_020dbe14.c
 *   [2] 0x020dbe30  src/unnamed/ov006/020d/func_ov006_020dbe30.c
 */

extern "C" {
extern "C" void* _ZN7fBase_cnwEj(unsigned int);
extern "C" void func_ov004_020b2adc(void*);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void*);
extern "C" void func_ov006_020c1d80(void*);
extern "C" void func_020733a8(void*, int, int, void*, void*);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV11dScMgCard_c;
extern "C" void func_ov006_020d96e0(void);
extern "C" void func_ov006_020dbe30(int *);   /* reconciled: this TU defines it with a real signature */
extern "C" void func_ov006_020d96f0(void);
extern "C" void func_ov006_020dbe14(int *);   /* reconciled: this TU defines it with a real signature */
extern int data_ov006_0213bccc[];
extern int data_ov006_0213bcf4[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020dbe30, 0x020dbe30, size 0x10 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020dbe30(int *p) { p[0] = (int)data_ov006_0213bccc; }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov006_020dbe14, 0x020dbe14, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020dbe14(int *c){
  *c=(int)data_ov006_0213bccc;
  *c=(int)data_ov006_0213bcf4;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- MgPicturePoker_Spawn, 0x020dbd54, size 0xc0 */
/* -------------------------------------------------------------------------- */
extern "C" void* MgPicturePoker_Spawn(void){
  char* p = (char*)_ZN7fBase_cnwEj(0x539c);
  if (p) {
    func_ov004_020b2adc(p);
    *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
    *(int*)p = (int)&_ZTV11dScMgCard_c;
    func_ov006_020c1d80(p + 0x4f38);
    func_020733a8(p + 0x51a8, 5, 0x30, (void*)func_ov006_020dbe30, (void*)func_ov006_020d96e0);
    func_020733a8(p + 0x5298, 5, 0x30, (void*)func_ov006_020dbe14, (void*)func_ov006_020d96f0);
  }
  return p;
}
