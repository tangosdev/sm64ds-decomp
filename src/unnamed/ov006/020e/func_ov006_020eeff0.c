extern int data_ov006_021421bc;
extern char* data_ov006_021421b0;
extern int data_ov006_021421c0;
void func_ov006_020eeff0(void){
    int i;
    for(i=0;i<data_ov006_021421bc;i++){
        *(short*)(data_ov006_021421b0 + i*0x24 + 0x20)=0;
    }
    data_ov006_021421c0=0;
}
