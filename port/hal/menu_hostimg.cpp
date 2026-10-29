/* menu_hostimg.cpp -- see menu_hostimg.h (run menu1, lane ASSET2). */
#include "menu_hostimg.h"
#include "../third_party/stb/stb_image.h"
#include "ui_art.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <emmintrin.h>
#include <malloc.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>


namespace {

struct Img {
    int tried = 0, w = 0, h = 0;
    std::vector<uint32_t> px;           /* premultiplied, source size */
    /* ASSET5: TWO cached scaled sizes (the title's logo is drawn big and, after the choreography, small; a size change must not
       cost a rescale every frame) */
    int cw = 0, ch = 0;                 /* the cached scaled size, slot 0 */
    std::vector<uint32_t> scaled;
    int cw1 = 0, ch1 = 0;               /* slot 1 */
    std::vector<uint32_t> scaled1;
    int last_slot = 0;                  /* the slot filled last (the other one is replaced next) */
};
Img g_img[MENU_IMG_EXT_END];
/* Where each picture lives: the file name under "menu/" of the HD UI Textures mod (hd_ui/menu/<name>, found through ui_art_path). The port
   carries NO picture bytes: a name no enabled mod provides is "no picture" and the recipe keeps the ROM's own art. */
const char *const kSrc[MENU_IMG_COUNT] = {
    0, "logo_sm64_pc.png", "btn_mp.png", "btn_adventure.png", "btn_rec.png", "arrow_up.png", "arrow_down.png", "pattern.png",
    "title_star.png", "btn_save_cont.png", "btn_save_quit.png", "btn_cont.png", "arrow_orange_down.png",
    "go_click.png", "go_gradient.png",
    "go_l0.png", "go_l1.png", "go_l2.png", "go_l3.png", "go_l4.png", "go_l5.png", "go_l6.png", "go_l7.png",
};

bool take(const unsigned char *rgba, int w, int h, Img *I)
{
    if (!rgba || w < 8 || h < 8 || w > 8192 || h > 8192) return false;
    I->w = w; I->h = h;
    I->px.resize((size_t)w * h);
    for (int i = 0; i < w * h; ++i) {
        const unsigned r = rgba[i * 4], g = rgba[i * 4 + 1], b = rgba[i * 4 + 2], a = rgba[i * 4 + 3];
        I->px[i] = (a << 24) | (((r * a + 127) / 255) << 16) | (((g * a + 127) / 255) << 8) | ((b * a + 127) / 255);
    }
    return true;
}

/* WARMFIX1 F4: the environment strings this file reads, copied once (first use is always on the game thread, before any worker exists) so
   a worker never calls getenv while the game thread may be inside _putenv_s. */
struct Env { bool has_logo = false, has_dir = false; std::string logo, dir; };
const Env &env()
{
    static const Env e = [] {
        Env v;
        const char *a = getenv("SM64DS_MENU_LOGO"), *b = getenv("SM64DS_MENU_ART_DIR");
        if (a) { v.has_logo = true; v.logo = a; }
        if (b) { v.has_dir = true; v.dir = b; }
        return v;
    }();
    return e;
}

/* WARMFIX1 F1: one mutex for g_img. Held only for the swap / the slot lookup, never across a decode or a scale. A pointer handed out by
   menu_hostimg_scaled stays valid for a while because a replaced vector goes to g_grave and is freed two installs later, not at the swap. */
std::mutex g_mx;
std::vector<std::vector<uint32_t>> g_grave, g_grave_prev;

void load_into(int id, Img &I)
{
    int w = 0, h = 0, comp = 0;
    unsigned char *p = 0;
    char path[1100];
    /* SM64DS_MENU_LOGO (id 1) or SM64DS_MENU_ART_DIR=<folder> (every id) point somewhere else for a dev run; a name that cannot be read there
       means "no picture" (the recipe keeps the DS art). Otherwise the HD UI Textures mod (any enabled mod: hd_ui/menu/<name>) is the only
       source: nothing is embedded in the exe and no kit folder is read. */
    const Env &E = env();
    const char *over = id == MENU_IMG_LOGO && E.has_logo ? E.logo.c_str() : 0;
    const char *dir = E.has_dir ? E.dir.c_str() : 0;
    const char *efile = 0;
    const bool ext = id >= MENU_IMG_EXT_BASE && menu_ext_art_src(id, &efile);
    if (id >= MENU_IMG_COUNT && !ext) return;
    const char *fname = ext ? efile : kSrc[id];     /* ext: "pause3/<name>" */
    if (over && *over) {
        p = stbi_load(over, &w, &h, &comp, 4);
        fprintf(stderr, "[menu] art %d: SM64DS_MENU_LOGO=%s -> %s\n", id, over, p ? "loaded" : "unreadable, DS art kept");
    } else if (dir && *dir && strlen(dir) + 60 < sizeof path) {
        snprintf(path, sizeof path, "%s/%s", dir, fname);
        p = stbi_load(path, &w, &h, &comp, 4);
        fprintf(stderr, "[menu] art %d: %s -> %s\n", id, path, p ? "loaded" : "unreadable, DS art kept");
    } else {
        char rel[160];
        snprintf(rel, sizeof rel, "menu/%s", fname);
        if (ui_art_path(rel, path, sizeof path)) {
            p = stbi_load(path, &w, &h, &comp, 4);
            if (p) fprintf(stderr, "[menu] art %d: mod file %s (%dx%d)\n", id, path, w, h);
        }
    }
    if (p) {
        if (!take(p, w, h, &I)) I.px.clear();
        stbi_image_free(p);
    }
}

/* MENUPACE1: SM64DS_ART_TRACE=1 prints what a decode and a scale cost on the thread that did them */
bool art_trace()
{
    static const bool on = getenv("SM64DS_ART_TRACE") != 0;
    return on;
}
double art_ms()
{
    static LARGE_INTEGER f;
    LARGE_INTEGER n;
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&n);
    return (double)n.QuadPart * 1000.0 / (double)f.QuadPart;
}

void load_img(int id)
{
    Img &I = g_img[id];
    {
        std::lock_guard<std::mutex> lk(g_mx);
        if (I.tried) return;
    }
    env();
    Img t;
    const double t0 = art_trace() ? art_ms() : 0.0;
    load_into(id, t);                  /* decoded off to the side, published whole */
    if (art_trace()) fprintf(stderr, "[menu-art] id %d decoded on the logic thread: %.2f ms\n", id, art_ms() - t0);
    std::lock_guard<std::mutex> lk(g_mx);
    if (I.tried) return;               /* the background install won */
    I.tried = 1; I.w = t.w; I.h = t.h; I.px = std::move(t.px);
}

/* one axis of a separable resample: for each destination index, the source span and weights (triangle filter whose
   support grows with the reduction, so a shrink is an area average and an enlarge is bilinear) */
struct Tap { int first, n; std::vector<float> w; };
void taps(int sn, int dn, std::vector<Tap> *out)
{
    out->resize((size_t)dn);
    const double sc = (double)sn / dn, sup = sc > 1.0 ? sc : 1.0;
    for (int d = 0; d < dn; ++d) {
        const double c = (d + 0.5) * sc;
        int a = (int)floor(c - sup), b = (int)ceil(c + sup);
        if (a < 0) a = 0;
        if (b > sn) b = sn;
        Tap &t = (*out)[d];
        t.first = a; t.n = b - a; t.w.assign((size_t)t.n, 0.f);
        double sum = 0;
        for (int s = a; s < b; ++s) {
            double x = fabs((s + 0.5) - c) / sup;
            double wv = x < 1.0 ? 1.0 - x : 0.0;
            t.w[s - a] = (float)wv; sum += wv;
        }
        if (sum <= 0) { t.first = a < sn ? a : sn - 1; t.n = 1; t.w.assign(1, 1.f); sum = 1; }
        for (int s = 0; s < t.n; ++s) t.w[s] = (float)(t.w[s] / sum);
    }
}

void scale_to_ref(Img *I, int dw, int dh, std::vector<uint32_t> *outv)
{
    std::vector<Tap> tx, ty;
    taps(I->w, dw, &tx);
    taps(I->h, dh, &ty);
    std::vector<float> mid((size_t)dw * I->h * 4);
    for (int y = 0; y < I->h; ++y) {
        const uint32_t *row = &I->px[(size_t)y * I->w];
        for (int x = 0; x < dw; ++x) {
            float a = 0, r = 0, g = 0, b = 0;
            const Tap &t = tx[x];
            for (int k = 0; k < t.n; ++k) {
                const uint32_t c = row[t.first + k];
                const float wv = t.w[k];
                a += wv * (c >> 24); r += wv * ((c >> 16) & 255); g += wv * ((c >> 8) & 255); b += wv * (c & 255);
            }
            float *m = &mid[((size_t)y * dw + x) * 4];
            m[0] = a; m[1] = r; m[2] = g; m[3] = b;
        }
    }
    outv->assign((size_t)dw * dh, 0);
    for (int y = 0; y < dh; ++y) {
        const Tap &t = ty[y];
        for (int x = 0; x < dw; ++x) {
            float a = 0, r = 0, g = 0, b = 0;
            for (int k = 0; k < t.n; ++k) {
                const float *m = &mid[((size_t)(t.first + k) * dw + x) * 4];
                const float wv = t.w[k];
                a += wv * m[0]; r += wv * m[1]; g += wv * m[2]; b += wv * m[3];
            }
            int ia = (int)(a + 0.5f), ir = (int)(r + 0.5f), ig = (int)(g + 0.5f), ib = (int)(b + 0.5f);
            if (ia > 255) ia = 255;
            if (ir > ia) ir = ia;
            if (ig > ia) ig = ia;
            if (ib > ia) ib = ia;
            (*outv)[(size_t)y * dw + x] = ((uint32_t)ia << 24) | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
        }
    }
}

/* MENUPACE1: the same resample as scale_to, bit for bit (same weights, same order of the float adds per channel), with the four channels in
   one SSE register, flat tap tables, and no zero-filled scratch. */
struct FTaps { std::vector<int> first, n, off; std::vector<float> w; };
static void ftaps(int sn, int dn, FTaps *out)
{
    out->first.resize((size_t)dn); out->n.resize((size_t)dn); out->off.resize((size_t)dn);
    out->w.clear();
    const double sc = (double)sn / dn, sup = sc > 1.0 ? sc : 1.0;
    for (int d = 0; d < dn; ++d) {
        const double c = (d + 0.5) * sc;
        int a = (int)floor(c - sup), b = (int)ceil(c + sup);
        if (a < 0) a = 0;
        if (b > sn) b = sn;
        int n = b - a, first = a;
        const size_t o = out->w.size();
        out->w.resize(o + (size_t)(n > 1 ? n : 1), 0.f);
        double sum = 0;
        for (int s = a; s < b; ++s) {
            double x = fabs((s + 0.5) - c) / sup;
            double wv = x < 1.0 ? 1.0 - x : 0.0;
            out->w[o + (size_t)(s - a)] = (float)wv; sum += wv;
        }
        if (sum <= 0) { first = a < sn ? a : sn - 1; n = 1; out->w[o] = 1.f; sum = 1; }
        for (int s = 0; s < n; ++s) out->w[o + (size_t)s] = (float)(out->w[o + (size_t)s] / sum);
        out->first[d] = first; out->n[d] = n; out->off[d] = (int)o;
    }
}
static void scale_to_fast(const uint32_t *px, int sw, int sh, int dw, int dh, std::vector<uint32_t> *outv)
{
    FTaps tx, ty;
    ftaps(sw, dw, &tx);
    ftaps(sh, dh, &ty);
    float *mid = (float *)_aligned_malloc((size_t)dw * sh * 16, 16);       /* lanes: b g r a */
    const __m128i z = _mm_setzero_si128();
    for (int y = 0; y < sh; ++y) {
        const uint32_t *row = px + (size_t)y * sw;
        float *mo = mid + (size_t)y * dw * 4;
        for (int x = 0; x < dw; ++x) {
            __m128 acc = _mm_setzero_ps();
            const float *wp = &tx.w[(size_t)tx.off[x]];
            const uint32_t *rp = row + tx.first[x];
            for (int k = 0, nk = tx.n[x]; k < nk; ++k) {
                const __m128 pv = _mm_cvtepi32_ps(_mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_cvtsi32_si128((int)rp[k]), z), z));
                acc = _mm_add_ps(acc, _mm_mul_ps(_mm_set1_ps(wp[k]), pv));
            }
            _mm_store_ps(mo + (size_t)x * 4, acc);
        }
    }
    outv->assign((size_t)dw * dh, 0);
    for (int y = 0; y < dh; ++y) {
        const float *wp = &ty.w[(size_t)ty.off[y]];
        const int nk = ty.n[y], f0 = ty.first[y];
        for (int x = 0; x < dw; ++x) {
            __m128 acc = _mm_setzero_ps();
            for (int k = 0; k < nk; ++k)
                acc = _mm_add_ps(acc, _mm_mul_ps(_mm_set1_ps(wp[k]), _mm_load_ps(mid + ((size_t)(f0 + k) * dw + x) * 4)));
            alignas(16) float f[4];
            _mm_store_ps(f, acc);
            int ib = (int)(f[0] + 0.5f), ig = (int)(f[1] + 0.5f), ir = (int)(f[2] + 0.5f), ia = (int)(f[3] + 0.5f);
            if (ia > 255) ia = 255;
            if (ir > ia) ir = ia;
            if (ig > ia) ig = ia;
            if (ib > ia) ib = ia;
            (*outv)[(size_t)y * dw + x] = ((uint32_t)ia << 24) | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
        }
    }
    _aligned_free(mid);
}

/* the reference above is kept for SM64DS_MENU_SCALE_REF=1 (a proof aid: the two give the same pixels) */
void scale_to(Img *I, int dw, int dh, std::vector<uint32_t> *outv)
{
    static const bool ref = getenv("SM64DS_MENU_SCALE_REF") != 0;
    if (ref) { scale_to_ref(I, dw, dh, outv); return; }
    scale_to_fast(I->px.data(), I->w, I->h, dw, dh, outv);
}

Img *img_of(int id) { return id >= 1 && id < MENU_IMG_EXT_END ? &g_img[id] : 0; }

/* PAUSEFIX1: a worker thread that decodes and scales a picture off the game thread. A job touches only its own local Img; the game thread
   installs a finished job with menu_hostimg_prefetch_poll (vector moves, no copying), so no tick pays for a decode or a scale. */
struct Job { int id, dw, dh; };
struct Done { int id, dw, dh; Img img; bool ok; bool exc = false; };
struct Worker {
    std::mutex m;
    std::condition_variable cv;
    std::deque<Job> in;
    std::deque<Done> out;
    int busy = 0;                 /* jobs queued or running */
    std::deque<Job> live;          /* the same jobs, kept until installed (so a repeat call queues nothing) */
    bool started = false;
    std::map<int, Img> src;       /* WARMFIX1 F6: the source of a 2x2 pre-queue job, kept until the same picture's real-size job takes it (one decode, not two) */
};
Worker *g_wk;      /* never freed: the thread outlives static destruction */

void worker_main(Worker *W)
{
    for (;;) {
        Job j;
        {
            std::unique_lock<std::mutex> lk(W->m);
            W->cv.wait(lk, [W] { return !W->in.empty(); });
            j = W->in.front(); W->in.pop_front();
        }
        Done d;
        d.id = j.id; d.dw = j.dw; d.dh = j.dh; d.ok = false;
        const double t0 = art_trace() ? art_ms() : 0.0;
        try {
            bool have = false;
            if (!(j.dw == 2 && j.dh == 2)) {
                std::lock_guard<std::mutex> lk(W->m);
                auto it = W->src.find(j.id);
                if (it != W->src.end()) { d.img.w = it->second.w; d.img.h = it->second.h; d.img.px = std::move(it->second.px); W->src.erase(it); have = true; }
            }
            if (!have) load_into(j.id, d.img);
            if (j.dw == 2 && j.dh == 2 && !d.img.px.empty()) {
                Img k; k.w = d.img.w; k.h = d.img.h; k.px = d.img.px;
                std::lock_guard<std::mutex> lk(W->m);
                if (W->src.size() < 16) W->src[j.id] = std::move(k);
            }
            if (!d.img.px.empty() && d.img.w > 0) {
                if (j.dw > 0 && j.dh > 0) {          /* MENUPACE1: a job of size 0 x 0 is a decode alone (menu_hostimg_predecode) */
                    std::vector<uint32_t> sc;
                    scale_to(&d.img, j.dw, j.dh, &sc);
                    d.img.scaled.swap(sc);
                }
                d.ok = true;
            }
        } catch (...) {                /* WARMFIX1 F3: memory pressure: an empty failed result; the picture loads on first use */
            d.ok = false; d.exc = true; d.img = Img();
        }
        if (art_trace()) fprintf(stderr, "[menu-art] id %d worker %s %dx%d: %.2f ms\n", j.id, j.dw > 0 ? "decode+scale" : "decode", j.dw, j.dh, art_ms() - t0);
        std::lock_guard<std::mutex> lk(W->m);
        try { W->out.push_back(std::move(d)); }
        catch (...) {
            for (size_t i = 0; i < W->live.size(); ++i)
                if (W->live[i].id == j.id && W->live[i].dw == j.dw && W->live[i].dh == j.dh) { W->live.erase(W->live.begin() + i); break; }
        }
        --W->busy;
    }
}

bool slot_has(const Img &I, int dw, int dh)
{
    return (I.cw == dw && I.ch == dh && !I.scaled.empty()) || (I.cw1 == dw && I.ch1 == dh && !I.scaled1.empty());
}

}  // namespace

extern "C" int menu_hostimg_ready(int id)
{
    Img *I = img_of(id);
    if (!I) return 0;
    load_img(id);
    std::lock_guard<std::mutex> lk(g_mx);
    return I->w > 0 && !I->px.empty();
}

extern "C" int menu_hostimg_size(int id, int *w, int *h)
{
    if (!menu_hostimg_ready(id)) return 0;
    Img *I = img_of(id);
    std::lock_guard<std::mutex> lk(g_mx);
    *w = I->w; *h = I->h;
    /* ARROWART1: the selection arrows are drawn in the fixed 22 x 15 footprint of the ROM's art (its aspect, tip at the bottom or top
       centre), whatever the PNG's own pixel size: a hi-res picture of the same drawing drops in with no code change */
    if (id == MENU_IMG_ARROW_UP || id == MENU_IMG_ARROW_DOWN || id == MENU_IMG_ARROW_ORANGE) { *w = 22; *h = 15; }
    return 1;
}

extern "C" const uint32_t *menu_hostimg_scaled(int id, int dw, int dh)
{
    if (!menu_hostimg_ready(id) || dw < 1 || dh < 1 || dw > 8192 || dh > 8192) return 0;
    Img *I = img_of(id);
    {
        std::lock_guard<std::mutex> lk(g_mx);
        if (I->cw == dw && I->ch == dh && !I->scaled.empty()) return &I->scaled[0];
        if (I->cw1 == dw && I->ch1 == dh && !I->scaled1.empty()) return &I->scaled1[0];
    }
    /* a miss: scale off the lock (the source pixels never change once the picture is ready), then swap in; the slot not filled last is replaced */
    std::vector<uint32_t> sc;
    const double t0 = art_trace() ? art_ms() : 0.0;
    scale_to(I, dw, dh, &sc);
    if (art_trace()) fprintf(stderr, "[menu-art] id %d scaled: %.2f ms\n", id, art_ms() - t0);
    std::lock_guard<std::mutex> lk(g_mx);
    if (I->cw == dw && I->ch == dh && !I->scaled.empty()) return &I->scaled[0];           /* the background install got there first */
    if (I->cw1 == dw && I->ch1 == dh && !I->scaled1.empty()) return &I->scaled1[0];
    /* a miss replaces the slot that was not filled last */
    if (art_trace()) fprintf(stderr, "[menu-art] id %d scale MISS %dx%d on the logic thread\n", id, dw, dh);
    if (I->last_slot == 0) { g_grave.push_back(std::move(I->scaled1)); I->scaled1 = std::move(sc); I->cw1 = dw; I->ch1 = dh; I->last_slot = 1; return &I->scaled1[0]; }
    g_grave.push_back(std::move(I->scaled)); I->scaled = std::move(sc); I->cw = dw; I->ch = dh; I->last_slot = 0;
    return &I->scaled[0];
}

/* PAUSE2: the ids the compose treats as planks (lit when selected, sunk when pressed) */
extern "C" int menu_hostimg_is_plank(int id)
{
    return (id >= MENU_IMG_BTN_MP && id <= MENU_IMG_BTN_REC) || (id >= MENU_IMG_BTN_SAVE_CONT && id <= MENU_IMG_BTN_CONT) || menu_ext_art_is_plank(id);
}

/* PAUSEFIX1: 1 when the picture is decoded and already cached at dw x dh. Otherwise queues one background decode + scale (once; a repeat call
   for a job already queued or running changes nothing) and returns 0, or -1 when MENU_HOSTIMG_QUEUE jobs are already waiting (try again later). */
extern "C" int menu_hostimg_prefetch(int id, int dw, int dh)
{
    Img *I = img_of(id);
    if (!I || dw < 1 || dh < 1 || dw > 8192 || dh > 8192) return 1;
    try {
        env();                                                      /* WARMFIX1 F4: before any worker can exist */
        {
            std::lock_guard<std::mutex> lk(g_mx);
            if (I->tried && I->px.empty()) return 1;                /* no such picture: nothing to wait for */
            if (!I->px.empty() && slot_has(*I, dw, dh)) return 1;
        }
        if (!g_wk) g_wk = new Worker();
        Worker *W = g_wk;
        std::lock_guard<std::mutex> lk(W->m);
        for (const Job &q : W->live)
            if (q.id == id && q.dw == dw && q.dh == dh) return 0;
        if (W->busy >= 4) return -1;
        if (!W->started) {
            try { std::thread(worker_main, W).detach(); }
            catch (...) { return 1; }                               /* WARMFIX1 F3: no thread: load on first use, no flag left set */
            W->started = true;
        }
        W->live.push_back(Job{ id, dw, dh });
        try { W->in.push_back(Job{ id, dw, dh }); }
        catch (...) { W->live.pop_back(); return 1; }
        ++W->busy;
        W->cv.notify_one();
        return 0;
    } catch (...) {
        return 1;
    }
}

/* installs every finished background job; 1 while a job is still running or waiting to be installed */
extern "C" int menu_hostimg_prefetch_poll(void)
{
    if (!g_wk) return 0;
    Worker *W = g_wk;
    for (;;) {
        Done d;
        {
            std::lock_guard<std::mutex> lk(W->m);
            if (W->out.empty()) return W->busy > 0;
            d = std::move(W->out.front()); W->out.pop_front();
            for (size_t i = 0; i < W->live.size(); ++i)
                if (W->live[i].id == d.id && W->live[i].dw == d.dw && W->live[i].dh == d.dh) { W->live.erase(W->live.begin() + i); break; }
        }
        Img &I = g_img[d.id];
        std::lock_guard<std::mutex> lk(g_mx);                       /* WARMFIX1 F1: the swap, and only the swap */
        if (!d.ok) { if (!d.exc && !I.tried) I.tried = 1; continue; }     /* an exception is not "no such picture" */
        if (!I.tried) {
            I.tried = 1; I.w = d.img.w; I.h = d.img.h; I.px = std::move(d.img.px);
        }
        if (d.dw <= 0 || d.dh <= 0) continue;           /* MENUPACE1: a decode alone has no scaled copy to install */
        if (I.px.empty() || I.w != d.img.w || I.h != d.img.h || slot_has(I, d.dw, d.dh)) continue;
        g_grave_prev.swap(g_grave); g_grave.clear();                 /* what was replaced two installs ago is freed now */
        if (I.last_slot == 0) { g_grave.push_back(std::move(I.scaled1)); I.scaled1 = std::move(d.img.scaled); I.cw1 = d.dw; I.ch1 = d.dh; I.last_slot = 1; }
        else { g_grave.push_back(std::move(I.scaled)); I.scaled = std::move(d.img.scaled); I.cw = d.dw; I.ch = d.dh; I.last_slot = 0; }
    }
}

/* MENUPACE1: decodes the picture on the worker thread and installs it on a later poll, so the first draw that asks for it
   (menu_hostimg_ready) finds it decoded instead of paying a PNG decode inside a tick. 1 when it is already decoded (or has no file),
   0 when it was queued or is on its way, -1 when four jobs are already waiting (ask again on a later tick). */
extern "C" int menu_hostimg_predecode(int id)
{
    Img *I = img_of(id);
    if (!I) return 1;
    try {
        env();
        {
            std::lock_guard<std::mutex> lk(g_mx);
            if (I->tried) return 1;
        }
        if (!g_wk) g_wk = new Worker();
        Worker *W = g_wk;
        std::lock_guard<std::mutex> lk(W->m);
        for (const Job &q : W->live)
            if (q.id == id && q.dw == 0 && q.dh == 0) return 0;
        if (W->busy >= 4) return -1;
        if (!W->started) {
            try { std::thread(worker_main, W).detach(); }
            catch (...) { return 1; }
            W->started = true;
        }
        W->live.push_back(Job{ id, 0, 0 });
        try { W->in.push_back(Job{ id, 0, 0 }); }
        catch (...) { W->live.pop_back(); return 1; }
        ++W->busy;
        W->cv.notify_one();
        return 0;
    } catch (...) {
        return 1;
    }
}

/* run after093, lane SEAMART1 (one-process plan M3c): the HD UI art in place. hal/mods.cpp's relatch at the title changes which mods ship
   hd_ui/, and every picture above was decoded from the old list. port_menu_hostimg_hd_ui_quiesce waits (at most 2 s) until the worker has no
   job queued or running, so no decode reads the mod list while the relatch rebuilds it; only the game thread queues jobs, and it is inside
   the relatch. port_menu_hostimg_hd_ui_reset then drops every finished job and every decoded and scaled picture (the vectors go to the
   graves, as a replaced picture always does, so a pointer handed out this frame stays valid) and clears the tried flags: the next draw
   decodes again from the new list. Returns the number of pictures dropped. Game thread only. */
extern "C" int port_menu_hostimg_hd_ui_quiesce(void)
{
    if (!g_wk) return 1;
    for (int i = 0; i < 2000; ++i) {
        {
            std::lock_guard<std::mutex> lk(g_wk->m);
            if (g_wk->busy <= 0 && g_wk->in.empty()) return 1;
        }
        Sleep(1);
    }
    return 0;
}

/* FLIPSMOOTH1: the art generation. Every reset below bumps it, so a copy of a scaled picture kept outside this file (the plank turn's
   full-width copies in menu_compose.cpp) is known to be the old art and is built again from the new pictures. */
static std::atomic<unsigned> g_art_gen{ 0 };
extern "C" unsigned menu_hostimg_generation(void) { return g_art_gen.load(); }
extern "C" int port_menu_hostimg_hd_ui_reset(void)
{
    ++g_art_gen;
    if (g_wk) {
        std::lock_guard<std::mutex> lk(g_wk->m);
        g_wk->out.clear();
        g_wk->live.clear();
        g_wk->src.clear();
    }
    int n = 0;
    std::lock_guard<std::mutex> lk(g_mx);
    g_grave_prev.swap(g_grave); g_grave.clear();                     /* what was replaced two resets / installs ago is freed now */
    for (int id = 0; id < MENU_IMG_EXT_END; ++id) {
        Img &I = g_img[id];
        if (I.tried && !I.px.empty()) ++n;
        g_grave.push_back(std::move(I.px));
        g_grave.push_back(std::move(I.scaled));
        g_grave.push_back(std::move(I.scaled1));
        I = Img();
    }
    return n;
}
