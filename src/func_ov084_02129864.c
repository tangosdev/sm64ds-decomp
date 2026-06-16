extern int _ZN5Actor9TrackStarEjj(void*,unsigned int,unsigned int);
extern int data_0209f208[];
extern int data_0209f344[];
void func_ov084_02129864(char *c){
    if(*(unsigned char*)(c+0x464)!=2) return;
    if(*(signed char*)(c+0x465)>=0) return;
    unsigned int b3=*(unsigned char*)data_0209f208;
    char *p=*(char**)data_0209f344;
    unsigned int m=*(unsigned char*)(p + b3);
    unsigned int v=*(unsigned char*)(c+0x466);
    if(v!=m) return;
    *(unsigned char*)(c+0x465)=(unsigned char)_ZN5Actor9TrackStarEjj(c,v,1);
}
