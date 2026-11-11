/* hal/lobby_page_stubs.cpp: the lobby page for the smoke player (run after093, lane LOBBYUI1), which links the online menu's logic side
   (hal/menu_online.cpp asks for the page) but not the page: a request is dropped. See hal/lobby_page.h. */
#include "lobby_page.h"

extern "C" void lobby_page_request_host(void) {}
extern "C" void lobby_page_request_vs(void) {}
extern "C" void lobby_page_request_join(const char *) {}
extern "C" int lobby_page_pending(void) { return 0; }
extern "C" void lobby_page_clear_request(void) {}
extern "C" void lobby_page_open_now(void) {}
extern "C" int lobby_page_open(void) { return 0; }
extern "C" unsigned lobby_page_closed_count(void) { return 0; }
extern "C" void lobby_page_cancel(void) {}
extern "C" int lobby_page_env_poll(void) { return 0; }
extern "C" int lobby_page_frame(const struct SpInput *, unsigned) { return 0; }
extern "C" void lobby_page_draw(uint32_t *, int, int, int) {}
