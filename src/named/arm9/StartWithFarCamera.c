extern unsigned char* data_0209f340;
int StartWithFarCamera(void)
{
    return (data_0209f340[0x15] & 1) != 0;
}
