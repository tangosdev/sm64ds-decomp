extern unsigned int STAR_CAMERA_SETTINGS;
unsigned char GetStarCameraSetting(int idx)
{
    return (STAR_CAMERA_SETTINGS >> (idx * 4)) & 0xf;
}
