/* Differential probe: a spread of constructs where CodeWarrior point-releases
 * tend to diverge. Compiled with each candidate mwccarm version; tools/probe_
 * versions.py flags any function whose bytes differ across versions. */

/* --- floating point --- */
float  f_add(float a, float b)   { return a + b; }
float  f_div(float a, float b)   { return a / b; }
double d_mul(double a, double b) { return a * b; }
double d_div(double a, double b) { return a / b; }
int    f_to_i(float a)          { return (int)a; }
float  i_to_f(int a)            { return (float)a; }
int    f_cmp(float a, float b)  { return a < b ? 1 : 0; }

/* --- integer division / modulo (library calls) --- */
int      idiv(int a, int b)           { return a / b; }
int      imod(int a, int b)           { return a % b; }
unsigned udiv(unsigned a, unsigned b) { return a / b; }
int      div_const(int a)             { return a / 10; }

/* --- 64-bit --- */
long long ll_add(long long a, long long b) { return a + b; }
long long ll_mul(long long a, long long b) { return a * b; }
long long ll_shr(long long a, int b)       { return a >> b; }

/* --- control flow --- */
int sw_table(int x) {
    switch (x) {
        case 0:  return 11;
        case 1:  return 22;
        case 2:  return 33;
        case 3:  return 44;
        case 4:  return 55;
        case 5:  return 66;
        default: return -1;
    }
}
int tern_chain(int a) { return a > 100 ? a * 2 : (a < 0 ? -a : a + 7); }
int loop_sum(const int *p, int n) { int s = 0; for (int i = 0; i < n; i++) s += p[i]; return s; }
int nested(int n) { int s = 0; for (int i = 0; i < n; i++) for (int j = 0; j < i; j++) s += i * j; return s; }

/* --- struct copy / memory --- */
struct Six { int a, b, c, d, e, f; };
void scopy(struct Six *d, const struct Six *s) { *d = *s; }
void zero_n(int *p, int n) { for (int i = 0; i < n; i++) p[i] = 0; }

/* --- register pressure / scheduling --- */
int pressure(int a, int b, int c, int d, int e, int f) {
    int x = a * b, y = c * d, z = e * f;
    int u = x + y, v = y + z, w = x + z;
    return (u * v) - (w * u) + (v - w) + x - y + z;
}

/* --- bit twiddling --- */
int count_leading(unsigned x) { int n = 0; while (n < 32 && !(x & 0x80000000u)) { x <<= 1; n++; } return n; }
unsigned rotl(unsigned x, int r) { return (x << r) | (x >> (32 - r)); }

/* --- scaled / fixed-point (fx32 idiom from the game) --- */
int fix_mul(int a, int b) { return (int)(((long long)a * b + 0x800) >> 12); }
int scaled_index(const int *p, int i) { return p[i * 3 + 2]; }
