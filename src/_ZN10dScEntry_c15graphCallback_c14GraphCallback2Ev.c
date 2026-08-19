extern int _ZN3G2S12GetBG2ScrPtrEv();
extern int DecompressLZ16();
extern void Deallocate(void*);
extern int func_ov075_021160dc(char*);
int _ZN10dScEntry_c15graphCallback_c14GraphCallback2Ev(char *c){
 unsigned short *reg=(unsigned short*)0x400100c;
 unsigned short v=(unsigned short)(*reg & ~0x1f00);
 *reg=v | (*(int*)(c+0xc)<<8);
 if(*(void**)(c+4)!=0){
   int p=_ZN3G2S12GetBG2ScrPtrEv();
   DecompressLZ16(*(void**)(c+4),p);
   Deallocate(*(void**)(c+4));
   *(void**)(c+4)=0;
 }
 if(*(void**)(c+8)!=0) func_ov075_021160dc(*(char**)(c+8));
 return 1;
}
