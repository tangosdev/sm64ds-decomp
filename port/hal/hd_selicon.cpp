/* hd_selicon.cpp -- see hd_selicon.h (run after093, lane HDSELICON1): the star select's icon art out of the enabled mods. */
#include "hd_selicon.h"
#include "mods.h"
#include "../third_party/stb/stb_image.h"

#include <cstdio>
#include <cstring>
#include <string>

extern "C" {
int port_scene_live_id(void);           /* hal/scene_boot.cpp: 4 = the star select */
int GetOwnerLanguage(void);             /* the ROM's language byte: 0 English, 2 French, 3 German, 4 Italian, 5 Spanish */
int SublevelToLevel(int);
extern signed char data_02092110;       /* the level the star select is for */
}

namespace {

const int kStarSelScene = 4;

struct Slot {
    std::string path;              /* the file the art came from ("" = none) */
    HdSelArt art;
    bool tried;
};
Slot g_slot[2 + 5];                /* star, coin, medal en fr de it es */
bool g_live;
int g_gen;

const char *const kLang[5] = { "en", "fr", "de", "it", "es" };

int lang_index(void)
{
    switch (GetOwnerLanguage()) {
    case 2: return 1;
    case 3: return 2;
    case 4: return 3;
    case 5: return 4;
    default: return 0;             /* InitResources' own default arm: the English files */
    }
}

const HdSelArt *get(int slot, const char *rel)
{
    Slot &s = g_slot[slot];
    if (s.tried) return s.art.rgba ? &s.art : 0;
    s.tried = true;
    char path[1400];
    if (!port_mods_ui_file(rel, path, sizeof path)) {
        if (s.art.rgba) { stbi_image_free((void *)s.art.rgba); s.art = HdSelArt(); s.path.clear(); }
        return 0;
    }
    if (s.art.rgba && s.path == path) return &s.art;   /* the same file as last time: keep the decoded picture */
    if (s.art.rgba) { stbi_image_free((void *)s.art.rgba); s.art = HdSelArt(); s.path.clear(); }
    int w = 0, h = 0, comp = 0;
    unsigned char *p = stbi_load(path, &w, &h, &comp, 4);
    if (!p || w < 8 || h < 8) {
        if (p) stbi_image_free(p);
        std::fprintf(stderr, "[hdselicon] %s from %s: not a picture this game can read, ignored\n", rel, path);
        return 0;
    }
    s.art.w = w; s.art.h = h; s.art.rgba = p; s.art.gen = ++g_gen;
    s.path = path;
    std::fprintf(stderr, "[hdselicon] %s %dx%d from %s\n", rel, w, h, path);
    return &s.art;
}

}  // namespace

const HdSelArt *hd_selicon_art(int id)
{
    if (port_scene_live_id() != kStarSelScene) {
        if (g_live)
            for (int i = 0; i < 7; ++i) g_slot[i].tried = false;   /* asked again at the next star select; pictures kept by path */
        g_live = false;
        return 0;
    }
    g_live = true;
    static const HdSelArt kLiveMark = HdSelArt();
    switch (id) {
    case -1: return &kLiveMark;
    case HDSEL_STAR: return get(0, "starselect/star_counter.png");
    case HDSEL_COIN: return get(1, "starselect/coin_score.png");
    case HDSEL_MEDAL: {
        /* InitResources draws the medal on courses 1 to 15 only (SublevelToLevel < 0xF); the Bowser and secret stages load the koopa
           background into BG2 instead (src/_ZN12dScStarSel_c13InitResourcesEv.cpp :157-195) */
        if (SublevelToLevel(data_02092110) >= 0xF) return 0;
        const int li = lang_index();
        char rel[64];
        std::snprintf(rel, sizeof rel, "starselect/course_medal_%s.png", kLang[li]);
        return get(2 + li, rel);
    }
    default: return 0;
    }
}

/* the compositor's hook (hal/message_compositor.cpp), set before main like hal/hd_meter.cpp's */
extern "C" const HdSelArt *(*port_hd_selicon_hook)(int id);
namespace {
struct HookSet { HookSet() { port_hd_selicon_hook = hd_selicon_art; } } g_hook_set;
}
