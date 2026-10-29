/* menu_online_stubs.cpp -- the online menu's draw side for the smoke player (run after093, lane NAME1): the smoke target does not link ui_text, so the
   build side answers "nothing to add" and every plank is its solo self. See hal/menu_online_draw.cpp for the real one. */
#include "menu_online.h"
#include <string.h>

void menu_online_plank_state(const OnlFacts *, int k, int *kind, int *w1000)
{
    *w1000 = 1000;
    *kind = k == 0 ? ONL_K_ONLINE : k == 1 ? ONL_K_ADV : ONL_K_REC;
}
int menu_online_plank_phase(const OnlFacts *, int, int *a, int *half, int *from, int *to) { *a = 0; *half = 8; *from = *to = 0; return 0; }
int menu_online_add_plank(MenuLayout *, int, int, int, int, int, int, int, int, int, int, int, int, int, int) { return -1; }
int menu_online_plank_w(int, int fh) { return fh; }
int menu_online_plank_art_ready(int) { return 0; }
void menu_online_add_modal(MenuLayout *, const OnlFacts *) {}
void menu_online_add_tint(MenuLayout *, const OnlFacts *) {}
int menu_online_tint_permille(const OnlFacts *) { return 0; }
void menu_online_add_help(MenuLayout *, int, int, int) {}
void menu_online_add_arrow(MenuLayout *, int, int, int, int, int) {}

/* PROFILE1: the name and colour popup (hal/menu_online_profile.cpp) is a walk_window feature; the smoke player never opens it */
void profile_open(int) {}
int profile_active(void) { return 0; }
int profile_name_needed(void) { return 0; }
const char *profile_saved_name(void) { return ""; }
void profile_facts(ProfFacts *f) { memset(f, 0, sizeof *f); f->pending = -1; }
void profile_tick(void) {}
unsigned profile_pad(unsigned, unsigned) { return 0; }
int profile_touch(int, int, int) { return 0; }
int profile_back(void) { return 0; }
int profile_backspace(void) { return 0; }
void menu_online_profile_modal(MenuLayout *, const OnlFacts *) {}
void menu_online_add_name_plank(MenuLayout *, int, int, int) {}
