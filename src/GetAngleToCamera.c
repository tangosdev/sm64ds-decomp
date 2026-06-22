extern short data_020a1164[];
short GetAngleToCamera(int i) {
    return *(short*)((char*)data_020a1164 + i * 0x24);
}
