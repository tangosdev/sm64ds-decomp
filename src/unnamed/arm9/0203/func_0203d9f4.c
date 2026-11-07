extern int WM_GetLinkLevel(void);
extern unsigned short data_020a0f1c;
/* While link bit 0x8000 of data_020a0f1c is set (func_0203e20c sets it
   with 0x1000), WM_GetLinkLevel's value, else 0. func_0203083c indexes its
   link icon table data_020927a0 with it. */
int func_0203d9f4(void){
    if ((data_020a0f1c & 0x8000) == 0)
        return 0;
    return WM_GetLinkLevel();
}
