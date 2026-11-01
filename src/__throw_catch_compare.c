
/* Metrowerks __throw_catch_compare: does a catch clause's type string b
   accept the thrown type string a? A null b is catch (...) and accepts
   everything. A void pointer catch (P, optional V and K, then v) accepts
   any pointer throw (P or *). A class throw (!) is a list of !name!offset
   entries, the class first and then its bases; the first entry whose name
   matches the catch type puts its offset in *out. Otherwise the pointer
   and reference qualifiers are matched (the catch may add K and V, never
   drop them) and the rest compared. Returns 1 on a match, 0 otherwise. */
static inline int IsQualifiedPtr(unsigned char ca, unsigned char *p) {
    return ((ca == 'P') || (ca == 'R')) && (ca == (*p));
}

int __throw_catch_compare(unsigned char *a, unsigned char *b, int *out) {
    unsigned char *p = b;
    unsigned char *q;
    unsigned char ca;
    unsigned char cb;
    int num;
    unsigned char *new_var;
    *out = 0;
    if (b == 0) {
        return 1;
    }
    if ((*b) == 'P') {
        if ((*(++p)) == 'V') {
            p++;
        }
        if ((*p) == 'K') {
            p++;
        }
        if ((*p) == 'v') {
            if (((*a) == 'P') || ((*a) == '*')) {
                return 1;
            }
        }
        p = b;
    }
    ca = *((volatile unsigned char *)a);
    if ((ca == '!') || (ca == '*')) {
        ca = *a;
        cb = *(p++);
        q = a + 1;
        if (ca != cb) {
            return 0;
        }
        a = q;
        for (;;) {
            ca = *a;

            cb = *(p++);
            if (ca == cb) {
                a++;
                new_var = b;
                if (ca != '!') {
                    continue;
                }
                num = 0;
                if ((*((volatile unsigned char *)a)) != '!') {
                    do {
                        num = ((num * 10) + (*a)) - '0';
                        a++;
                    } while ((*a) != '!');
                }
                *out = num;
                return 1;
            }
            do {
                ca = *(a++);
            } while (ca != '!');

            do {
                ca = *(a++);
            } while (ca != '!');
            if ((*a) == 0) {
                return 0;
            }
            p = new_var + 1;
        }
    }
    while (IsQualifiedPtr(ca = *a, p)) {
        cb = *(++p);

        a++;
        if (cb == 'K') {
            p++;
            if ((*a) == 'K') {
                a++;
            }
        }
        ca = *a;
        if (ca == 'K') {
            return 0;
        }
        if ((*p) == 'V') {
            if (ca == 'V') {
                a++;
            }
            p++;
        }
        if ((*((volatile unsigned char *)a)) == 'V') {
            return 0;
        }
    }
    cb = *p;
    if (ca == cb) {
        do {
            if (ca == 0) {
                return 1;
            }
            ca = *(++a);
            cb = *(++p);
        } while (ca == cb);
    }
    return 0;
}
