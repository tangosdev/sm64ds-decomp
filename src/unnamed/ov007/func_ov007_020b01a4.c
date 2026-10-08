extern char* data_ov007_0210342c;
extern void func_ov007_020b0244(void);
void func_ov007_020b01a4(void){
 if(*(int*)(*(char**)(data_ov007_0210342c+8)+0xc)!=0) return;
 func_ov007_020b0244();
 *(short*)(*(char**)(data_ov007_0210342c+0xc)+2)=9;
}
