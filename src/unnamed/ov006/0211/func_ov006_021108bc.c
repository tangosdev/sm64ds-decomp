int func_ov006_021108bc(int *a, int *b){
    int ax = a[2];
    int bx = b[0];
    if(bx >= ax-0xe000 && bx < ax+0xe000){
        int ay = a[3];
        int by = b[1];
        if(by >= ay-0x48000 && by < ay+0x28000){
            if(bx < ax-0xa000) return 1;
            return (bx >= ax+0xa000);
        }
    }
    return 0;
}
