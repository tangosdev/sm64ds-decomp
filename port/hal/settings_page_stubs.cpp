/* MENU1 / SETPAGEFIX1: the settings page's three seams as no-ops, for a target that links hal/sub_screen.cpp but not the page
   (smoke_player). walk_window and walk_window_hires link the real ones (settings_page.cpp, settings_page_hooks.cpp). */
#include <stdint.h>

extern "C" int settings_page_hook_pad(uint16_t *) { return 0; }
extern "C" int settings_page_hook_touch(unsigned char, unsigned char, unsigned char) { return 0; }
extern "C" int settings_page_swallow_back(void) { return 0; }
extern "C" void menu_pause_hook_pad(uint16_t *, int) {}
extern "C" void menu_pause_rom_pad(uint16_t *) {}
extern "C" int menu_pause_rom_fallback(void) { return 0; }
extern "C" int menu_pause_slide_busy(void) { return 0; }
extern "C" int menu_pause_rom_page_view(void) { return 0; }    /* PAUSESCALE2 */
