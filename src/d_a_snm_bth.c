// @symbol daSnmBth_c_classInit
/* The pinned compiler cannot express the ROM's folded factory as genuine C++:
 * `new SnowmanBreath` calls the unavailable global operator new, while the
 * cartridge calls fBase_c::operator new. Placement/class-specific new are
 * rejected by mwccarm 2004/b56, so this remains the measured C allocator wall. */
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void _ZN21SnowmanBreathParticleD1Ev(void*);
extern void _ZN21SnowmanBreathParticleC1Ev(void*);
extern void* _ZTV13SnowmanBreath[];
/* Reconstructed source-style name: SM64DS proves daSnmBth_c through RTTI,
 * allocation size, vtable identity, and the SNOWMAN_BREATH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SnowmanBreath_Spawn. */
int* daSnmBth_c_classInit(void){
  int* p = (int*)_ZN7fBase_cnwEj(0x13d4);
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV13SnowmanBreath;
    func_020733a8((char*)p+0xd4, 0x32, 0x60,
                 _ZN21SnowmanBreathParticleC1Ev,
                 _ZN21SnowmanBreathParticleD1Ev);
  }
  return p;
}
