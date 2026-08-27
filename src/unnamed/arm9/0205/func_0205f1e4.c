/* func_0205f1e4 at 0x0205f1e4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int func_0205eb58(int a0, int s58, int s5a, int b5c, int b5d, int s5e, int s60, int b62, int b63);
typedef struct { char pad[0x58]; unsigned short s58; unsigned short s5a; unsigned char b5c; unsigned char b5d; unsigned short s5e; unsigned short s60; unsigned char b62; unsigned char b63; } S;
int func_0205f1e4(int a0){
 S *p=(S*)0x027ffc80;
 unsigned short s58,s5a,s5e,s60; unsigned char b5c,b5d,b62,b63;
 s58=p->s58; s5a=p->s5a; b5c=p->b5c; b5d=p->b5d; s5e=p->s5e; s60=p->s60; b62=p->b62; b63=p->b63;
 if(s58==0&&s5e==0&&s5a==0&&s60==0){return 0;}
 return func_0205eb58(a0,s58,s5a,b5c,b5d,s5e,s60,b62,b63)==0;
}