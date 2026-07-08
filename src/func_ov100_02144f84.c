extern unsigned short data_ov100_02148708[];
extern int MUSIC_VOLUME_LSL_12[];
extern int MESSAGE_SOUND_VOLUME_LSL_12[];

int func_ov100_02144f84(void)
{
    if (*data_ov100_02148708 != 0) goto fail;
    if (*MUSIC_VOLUME_LSL_12 != 0x7f000) goto fail;
    if (*MESSAGE_SOUND_VOLUME_LSL_12 == 0) return 1;
fail:
    return 0;
}
