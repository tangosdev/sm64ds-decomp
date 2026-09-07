extern int data_020a6134[];

void func_020584d0(char* node)
{
    /* cur/prev are pointers, not ints: with int-typed nodes mwcc CSEs the
       prev+0x68 address into r2 for the read and the write (addne r2,ip,#0x68 /
       ldrne r1,[r2] / strne r0,[r2]), one word over the cartridge's
       ldrne r1,[ip,#0x68] / strne r0,[ip,#0x68]. */
    char* cur = (char*)data_020a6134[3];
    char* prev = 0;

    while (cur != 0 && *(unsigned int*)(cur + 0x70) < *(unsigned int*)(node + 0x70)) {
        prev = cur;
        cur = *(char**)(cur + 0x68);
    }

    if (prev == 0) {
        *(int*)(node + 0x68) = data_020a6134[3];
        data_020a6134[3] = (int)node;
    } else {
        *(int*)(node + 0x68) = *(int*)(prev + 0x68);
        *(int*)(prev + 0x68) = (int)node;
    }
}
