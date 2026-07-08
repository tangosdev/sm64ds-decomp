extern unsigned char* LEVEL_FILE;
int StartWithFarCamera(void)
{
    return (LEVEL_FILE[0x15] & 1) != 0;
}
