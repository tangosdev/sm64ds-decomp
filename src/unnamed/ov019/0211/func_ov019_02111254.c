int func_ov019_02111254(int unused, int r1) {
    int r0 = r1 * 0x54cc + 0x4a800;
    
    if (r0 > 0x65000) {
        r0 = 0x65000;
    }
    if (r0 < 0x30000) {
        r0 = 0x30000;
    }
    
    return r0;
}
