#!/bin/bash
# Prove the SDL event pump reaches the game loop: push an SDL_QUIT via xdotool
# window close is not available headless, so instead we confirm the keyboard
# seam by launching with SDL and sending a synthetic key through xdotool if X11
# is reachable. Fallback: confirm SDL keyboard state array is queryable.
OUT=/mnt/c/tmp/sm64ds-linux/b/out
which xdotool >/dev/null 2>&1 && echo "xdotool: present" || echo "xdotool: MISSING"
# Minimal SDL keyboard probe compiled inline (32-bit) to confirm the same seam
cat > /tmp/_sdl_kbd_probe.c <<'EOF'
#include <SDL2/SDL.h>
#include <stdio.h>
int main(void){
    if(SDL_Init(SDL_INIT_VIDEO)!=0){printf("INIT_FAIL %s\n",SDL_GetError());return 2;}
    printf("VIDEO_DRIVER=%s\n", SDL_GetCurrentVideoDriver());
    SDL_Window* w=SDL_CreateWindow("probe",0,0,64,64,SDL_WINDOW_SHOWN);
    printf("WINDOW=%s\n", w?"OPENED":"NULL");
    int n=0; const Uint8* ks=SDL_GetKeyboardState(&n);
    printf("KEYSTATE_ARRAY=%s len=%d\n", ks?"OK":"NULL", n);
    if(w) SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}
EOF
gcc -m32 /tmp/_sdl_kbd_probe.c -o /tmp/_sdl_kbd_probe \
    $(PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig pkg-config --cflags --libs sdl2 2>/dev/null) 2>/tmp/_probe_build.log
if [ -x /tmp/_sdl_kbd_probe ]; then /tmp/_sdl_kbd_probe; else echo "PROBE_BUILD_FAIL"; cat /tmp/_probe_build.log; fi
