extern int data_ov006_02142f70;
extern char* data_ov006_02142f68;
extern int data_ov006_02142f64;
void func_ov006_02120ca0(void){
    int i;
    for(i=0;i<data_ov006_02142f70;i++){
        *(short*)(data_ov006_02142f68 + i*0x24 + 0x20)=0;
    }
    data_ov006_02142f64=0;
}
