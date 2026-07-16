void func_ov007_020c92d0(char *state)
{
    short pending = *(short *)(state + 2);

    if (pending != -1) {
        short current = *(short *)state;

        if (pending != current) {
            *(short *)(state + 8) = current;
            *(short *)state = *(short *)(state + 2);
        }
        *(short *)(state + 2) = -1;
    }

    if (*(short *)state != *(short *)(state + 8)) {
        *(int *)(state + 12) = 0;
        *(short *)(state + 10) = *(short *)(state + 8);
        *(int *)(state + 4) = 0;
    } else if ((unsigned int)*(int *)(state + 12) < 0xffffffffU) {
        int *counter = (int *)(((long long)((int)state + 12)) & 0xffffffffffffffffLL);
        ++*counter;
    }

    *(short *)(state + 8) = *(short *)state;
}
