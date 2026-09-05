/* recovered: the three range tests are MATERIALIZED booleans -- `sel = cond ? 1 : 0`
   held in a scalar and tested, which mwccarm only keeps under
   `#pragma opt_propagation off` (notes/mwccarm-codegen.md 6r); with propagation on it
   folds each into its branch and the whole shape drifts.
   The 0x23 arm tests `r1 < 6`, not `>= 6` -- the two tail cases have OPPOSITE
   conditions and share one `r2 = 2`, which is why they read as a single || chain. */
#pragma opt_propagation off
extern short data_ov006_0212edb8[];
extern short data_ov006_0212edba[];
extern short data_ov006_0212ed88[];
extern short data_ov006_0212ed8a[];
extern short data_ov006_0212ed8c[];
extern short data_ov006_0212ed8e[];

int func_ov006_02108b90(short *r0, int r1)
{
    int r3 = *(short *)((char *)r0 + 0x2c);
    int r2 = 0;
    int ip;

    ip = (r3 <= 0xb) ? 1 : 0;
    if (ip != 0) {
        if (r3 == r1) r2 = 0xc;
    } else {
        ip = (r3 >= 0xc && r3 <= 0x1c) ? 1 : 0;
        if (ip != 0) {
            ip = (r3 - 0xc) << 2;
            if (r1 == *(short *)((char *)data_ov006_0212edb8 + ip) ||
                r1 == *(short *)((char *)data_ov006_0212edba + ip))
                r2 = 6;
        } else {
            ip = (r3 >= 0x1d && r3 <= 0x22) ? 1 : 0;
            if (ip != 0) {
                ip = (r3 - 0x1d) << 3;
                if (r1 == *(short *)((char *)data_ov006_0212ed88 + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8a + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8c + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8e + ip))
                    r2 = 3;
            } else if ((r3 == 0x23 && r1 < 6) || (r3 == 0x24 && r1 >= 6)) {
                r2 = 2;
            }
        }
    }
    *(short *)((char *)r0 + 0x2e) = r2;
    return r2;
}
