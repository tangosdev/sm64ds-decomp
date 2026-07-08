extern void func_020308b4(signed char levelID);
extern void Deallocate(void *ptr);
extern void *data_0209b2e8;
extern void *SCENE_RELATED;
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
extern void CleanCommonModelDataArr(void);
int func_ov075_0211a210(signed char levelID)
{
    func_020308b4(levelID);
    if (data_0209b2e8 != (void *)0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = (void *)0;
    }
    SCENE_RELATED = (void *)0;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    CleanCommonModelDataArr();
    return 1;
}
