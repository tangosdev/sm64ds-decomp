
/* Metrowerks __throw_catch_compare: does a catch clause's type string
   catchtype accept the thrown type string throwtype? A null catchtype is
   catch (...) and accepts everything. A void pointer catch (P, optional V
   and K, then v) accepts any pointer throw (P or *). A class throw (!) is a
   list of !name!offset entries, the class first and then its bases; the
   first entry whose name matches the catch type puts its offset in
   *offset_result. Otherwise the pointer and reference qualifiers are
   matched (the catch may add K and V, never drop them) and the rest
   compared. Returns 1 on a match, 0 otherwise. Written in the shape of the
   Metrowerks runtime's own source; the const on both strings is load-
   bearing (without it the function is one instruction shorter). */
int __throw_catch_compare(const unsigned char *throwtype, const unsigned char *catchtype,
                          int *offset_result) {
    const unsigned char *cptr1;
    const unsigned char *cptr2;

    *offset_result = 0;
    if ((cptr2 = catchtype) == 0) {
        return 1;
    }
    cptr1 = throwtype;
    if (*cptr2 == 'P') {
        cptr2++;
        if (*cptr2 == 'V')
            cptr2++;
        if (*cptr2 == 'K')
            cptr2++;
        if (*cptr2 == 'v') {
            if (*cptr1 == 'P' || *cptr1 == '*')
                return 1;
        }
        cptr2 = catchtype;
    }
    switch (*cptr1) {
    case '*':
    case '!':
        if (*cptr1++ != *cptr2++)
            return 0;
        for (;;) {
            if (*cptr1 == *cptr2++) {
                if (*cptr1++ == '!') {
                    int offset;
                    for (offset = 0; *cptr1 != '!';)
                        offset = offset * 10 + *cptr1++ - '0';
                    *offset_result = offset;
                    return 1;
                }
            } else {
                while (*cptr1++ != '!') {
                }
                while (*cptr1++ != '!') {
                }
                if (*cptr1 == 0)
                    return 0;
                cptr2 = catchtype + 1;
            }
        }
    }
    while ((*cptr1 == 'P' || *cptr1 == 'R') && *cptr1 == *cptr2) {
        cptr1++;
        cptr2++;
        if (*cptr2 == 'K') {
            if (*cptr1 == 'K')
                cptr1++;
            cptr2++;
        }
        if (*cptr1 == 'K')
            return 0;
        if (*cptr2 == 'V') {
            if (*cptr1 == 'V')
                cptr1++;
            cptr2++;
        }
        if (*cptr1 == 'V')
            return 0;
    }
    for (; *cptr1 == *cptr2; cptr1++, cptr2++) {
        if (*cptr1 == 0)
            return 1;
    }
    return 0;
}
