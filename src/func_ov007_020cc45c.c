extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern int func_ov007_020b6f4c(void);
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
extern void func_0203cbc0(void* a);
extern int SCENE_RELATED;
extern int data_0209b340[];
extern void* data_0209b33c;

int func_ov007_020cc45c(void) {
    SCENE_RELATED = 0;
    _ZN5Scene20SetAndStopColorFaderEv();
    data_0209b340[0] = func_ov007_020b6f4c();
    data_0209b340[1] = 2;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    func_0203cbc0(data_0209b33c);
    data_0209b33c = 0;
    return 1;
}
