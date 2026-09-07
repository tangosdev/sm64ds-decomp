//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN12dEnemyBase_cC2Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void _ZN9ModelAnimC1Ev(void*);
extern void _ZN15MaterialChangerC1Ev(void*);
extern void _ZN15TextureSequenceC1Ev(void*);
extern void _ZN18TextureTransformerC1Ev(void*);
extern void _ZN10dBgCh_ActrC1Ev(void*);
extern void _ZN10dCcAcPos_cD1Ev(void*);
extern void _ZN10dCcAcPos_cC1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN11ShadowModelC1Ev(void*);
extern void _ZN7Vector3D1Ev(void*);
extern void func_0203d384(void*);
extern void* _ZTV8Goomboss[];
/* Reconstructed source-style name: SM64DS proves daKuriKing_c through RTTI,
 * allocation size, vtable identity, and the KURIKING_VANISH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ExplosionGoomba_Spawn. */
void* daKuriKing_c_classInit_KURIKING_VANISH(void){
  char* p=(char*)_ZN7fBase_cnwEj(0x610);
  if(p){
    _ZN12dEnemyBase_cC2Ev(p);
    *(void***)p=(void**)_ZTV8Goomboss;
    func_020733a8(p+0x110, 4, 0x40, _ZN10dCcAcPos_cC1Ev, _ZN10dCcAcPos_cD1Ev);
    _ZN9ModelAnimC1Ev(p+0x210);
    func_020733a8(p+0x274, 3, 0x28, _ZN11ShadowModelC1Ev, _ZN11ShadowModelD1Ev);
    func_020733a8(p+0x3ac, 3, 0xc, func_0203d384, _ZN7Vector3D1Ev);
    _ZN15MaterialChangerC1Ev(p+0x3d0);
    _ZN15TextureSequenceC1Ev(p+0x3e4);
    _ZN18TextureTransformerC1Ev(p+0x3f8);
    _ZN10dBgCh_ActrC1Ev(p+0x40c);
  }
  return p;
}
}
