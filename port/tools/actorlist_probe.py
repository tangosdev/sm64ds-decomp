#!/usr/bin/env python
"""Walk the ROM's own actor list inside a WEDGED port process and say what is in it.

src/func_02043f4c.c is the list walk BigBrickBlock::Behavior spins in, and its
shape is the whole reason this works from outside: the list head is the DS
global data_0209b468, each node is embedded at actor+0x50 with `next` at +0x54
and a back pointer to the actor at +0x58, and the actor's id is the u16 at
actor+0x0c. The port hosts DS globals at their own DS addresses, so those are
literal addresses in the process and ReadProcessMemory is enough -- no debugger,
no symbols, and nothing that has to stop the spin to answer.

Usage: actorlist_probe.py <exe> <VAR=val ...> [--wait S] [--find-id 0xb]
"""

import ctypes
import ctypes.wintypes as w
import os
import subprocess
import sys
import time

HEAD = 0x017A8AE0          # data_0209b468, the list head, hosted (walk_window.map)
NODE_OFF = 0x50            # the node embedded in every actor
NEXT_OFF = NODE_OFF + 4
OWNER_OFF = NODE_OFF + 8
ID_OFF = 0x0C              # dActor_c::actorID, u16

k32 = ctypes.WinDLL("kernel32", use_last_error=True)
k32.OpenProcess.restype = w.HANDLE
k32.ReadProcessMemory.argtypes = [w.HANDLE, w.LPCVOID, w.LPVOID,
                                  ctypes.c_size_t,
                                  ctypes.POINTER(ctypes.c_size_t)]


def rd(h, addr, n):
    buf = (ctypes.c_char * n)()
    got = ctypes.c_size_t(0)
    if not k32.ReadProcessMemory(h, ctypes.c_void_p(addr), buf, n,
                                 ctypes.byref(got)):
        return None
    return bytes(buf[:got.value])


def u32(h, a):
    b = rd(h, a, 4)
    return None if b is None or len(b) < 4 else int.from_bytes(b, "little")


def u16(h, a):
    b = rd(h, a, 2)
    return None if b is None or len(b) < 2 else int.from_bytes(b, "little")


def u8(h, a):
    b = rd(h, a, 1)
    return None if b is None or len(b) < 1 else b[0]


def main():
    exe = sys.argv[1]
    rest = sys.argv[2:]
    wait = 35.0
    want = []
    setenv = {}
    i = 0
    while i < len(rest):
        a = rest[i]
        if a == "--wait":
            wait = float(rest[i + 1]); i += 2
        elif a == "--find-id":
            want.append(int(rest[i + 1], 0)); i += 2
        else:
            k, _, v = a.partition("=")
            setenv[k] = v
            i += 1

    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    env.update(setenv)
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    si.wShowWindow = 7
    exedir = os.path.dirname(os.path.abspath(exe))
    p = subprocess.Popen([exe], cwd=exedir, env=env,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                         startupinfo=si, creationflags=0x08000000)
    t0 = time.time()
    while time.time() - t0 < wait:
        if p.poll() is not None:
            print("EXITED rc=%s after %.1fs -- nothing to probe"
                  % (p.returncode, time.time() - t0))
            return 0
        time.sleep(0.5)

    h = k32.OpenProcess(0x0410, False, p.pid)  # VM_READ | QUERY_INFORMATION
    if not h:
        print("OpenProcess failed: %d" % ctypes.get_last_error())
        p.kill()
        return 1

    head = u32(h, HEAD)
    print("data_0209b468 = 0x%08x" % (head or 0))
    ids = {}
    order = []
    seen = set()
    n = head
    steps = 0
    cyclic = False
    while n:
        if n in seen:
            cyclic = True
            print("LIST IS CIRCULAR at node 0x%08x after %d nodes" % (n, steps))
            break
        seen.add(n)
        owner = u32(h, n + 8)
        aid = u16(h, owner + ID_OFF) if owner else None
        if aid is None:
            print("unreadable node 0x%08x owner 0x%08x" % (n, owner or 0))
            break
        ids[aid] = ids.get(aid, 0) + 1
        order.append((n, owner, aid))
        n = u32(h, n + 4)
        steps += 1
        if steps > 4000:
            print("stopped after 4000 nodes")
            break

    print("%d actors, %d distinct ids, circular=%s"
          % (steps, len(ids), cyclic))
    print("ids present: %s" % " ".join("0x%x(x%d)" % (k, v)
                                       for k, v in sorted(ids.items())))
    for wid in want:
        rows = [(nd, ow) for nd, ow, a in order if a == wid]
        print("id 0x%x: %d instance(s)" % (wid, len(rows)))
        for nd, ow in rows:
            p1 = u32(h, ow + 0x08)
            print("   actor 0x%08x  param1(+0x08)=0x%08x  mSwitchType(+0x33c)=%d "
                  "mEventBit(+0x34e)=0x%02x unk_34f(+0x34f)=%d  "
                  "(param1>>3)&0xf=%d (param1>>4)&0xf=%d"
                  % (ow, p1, u32(h, ow + 0x33C) or 0, u8(h, ow + 0x34E),
                     u8(h, ow + 0x34F), (p1 >> 3) & 0xf, (p1 >> 4) & 0xf))
    # THE WHOLE TAIL OF EVERY WANTED ACTOR, because a field that reads zero is
    # either a write that never happened or a write that landed somewhere else,
    # and only the bytes tell the two apart. StarSwitch::InitResources writes a
    # recognisable signature -- mDrawScaleX/Y/Z = 0x1000 three words running --
    # so scanning for it says whether the body ran and at what offset it lands.
    sig = (0x1000).to_bytes(4, "little") * 3
    for wid in want:
        for nd, ow in [(n_, o_) for n_, o_, a in order if a == wid]:
            blob = rd(h, ow, 0x400) or b""
            print("   actor 0x%08x vptr=0x%08x" % (ow, u32(h, ow) or 0))
            hits = [k for k in range(0, max(0, len(blob) - 12), 4)
                    if blob[k:k + 12] == sig]
            print("   0x1000 triple (mDrawScaleX/Y/Z) at: %s"
                  % (", ".join("+0x%x" % k for k in hits) or "NOWHERE"))
            for off in range(0x310, 0x360, 16):
                print("     +0x%03x  %s" % (off, blob[off:off + 16].hex(" ")))

    # Every BigBrickBlock-family actor and the event id it is hunting for.
    for fam in (0x0F, 0x10, 0x11, 0x12, 0x13, 0x2E):
        rows = [(nd, ow) for nd, ow, a in order if a == fam]
        for nd, ow in rows:
            p1 = u32(h, ow + 0x08)
            print("BigBrickBlock actorID 0x%02x at 0x%08x: param1(+0x08)=0x%08x "
                  "mEventID(+0x320)=0x%02x mSwitch(+0x324)=0x%08x"
                  % (fam, ow, p1, u8(h, ow + 0x320), u32(h, ow + 0x324)))
    p.kill()
    return 0


if __name__ == "__main__":
    sys.exit(main())
