//cpp
extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
extern int data_ov089_02132880[];
void func_ov089_0213115c(char *c, int i){
    if(i==0 || i>=5){
        *(int*)(c+0x448)=0;
        return;
    }
    *(int*)(c+0x448)=i;
    void *file=*(void**)(*(int*)((char*)data_ov089_02132880 + i*4) + 4);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x114, file, 0x40000000, 0x1000, 0);
}
}
