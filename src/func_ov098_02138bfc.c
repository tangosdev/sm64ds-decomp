/* func_ov098_02138bfc at 0x02138bfc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
 */
typedef enum { false, true } bool;
extern int func_02035638(unsigned char *p);
extern int func_0203567c(int p);
extern int _ZNK10ClsnResult9GetClsnIDEv(int self);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
int func_ov098_02138bfc(char *c){
 int r; void *a; unsigned short type;
 if(!func_02035638((unsigned char*)(c+0x320))) return 0;
 r=func_0203567c((int)(c+0x320));
 if(_ZNK10ClsnResult9GetClsnIDEv(r)==-1) return 0;
 a=_ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(r));
 if(!a) return 0;
 type=*(unsigned short*)((char*)a+0xc);
 { bool b;
 b=(type==0x135); if(b) goto out;
 b=(type==0xa2); if(b) goto out;
 b=(type==0xa3); if(b) goto out;
 b=(type==0xa1); if(b) goto out;
 b=(type==0xa4); if(!b) return 0;
 out: return 1; }
}