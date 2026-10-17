"""Pass-3 runtime watcher for the minigame names (notes/minigames/runtime-checks.md).

Attaches to the melonDS GDB stub, arms one breakpoint per check symbol, and on
each hit logs r0-r3/lr. A hit counts only when the 8 bytes at pc match the
symbol's overlay image (ov005 and ov006 share 0x020bfec0). Each breakpoint is
cleared after MAX_HITS hits so per-frame functions do not stall the game.
Create build/melonds/STOP to detach cleanly. Write "addr len" lines (both hex) to
build/melonds/PEEK to read memory: the answer lands in PEEK.out. Write "addr hexbytes"
lines to build/melonds/POKE to patch RAM (for example `bx lr` over a draw call to see
what it draws): the old and new bytes land in POKE.out.

usage: python tools/trace/mg/mgwatch.py [--unlock] [--out build/melonds/pass3.jsonl]
"""
import argparse, json, os, re, select, socket, sys, time
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'tools', 'trace'))
RUN = os.path.join(ROOT, 'build', 'melonds')  # melonDS, the ROM copy and run files (gitignored)
from rsp import RspClient  # noqa: E402

MODULES = {  # module: (symbols.txt, image, load address)
    'ov004': ('config/arm9/overlays/ov004/symbols.txt', 'extracted/dsd/arm9_overlays/ov004.bin', 0x020ad660),
    'ov005': ('config/arm9/overlays/ov005/symbols.txt', 'extracted/dsd/arm9_overlays/ov005.bin', 0x020bfec0),
    'ov006': ('config/arm9/overlays/ov006/symbols.txt', 'extracted/dsd/arm9_overlays/ov006.bin', 0x020bfec0),
}
# (check, symbol regex): every symbols.txt name that matches is armed.
CHECKS = [
    ('shared state',      r'^Mg_SetSharedState$'),
    ('banner',            r'^Mg_ShowBanner$'),
    ('banner override',   r'^Mg_SetNextBanner$'),
    ('jingle',            r'^Mg_PlayJingle$'),
    ('start round',       r'^Mg_CallStartRound$'),
    ('new record',        r'^_ZN\d+dScM\w+_c11OnNewRecordEv$'),
    ('deal done',         r'^_ZN\d+dScM\w+_c10OnDealDoneEv$'),
    ('presenter anim',    r'^Presenter_Play(Anim[1247]|Miss|Match|Lose|Cheer)$'),
    ('credits',           r'^Mg_(Lose|Gain|Add|Sub)Credits$'),
    ('bet',               r'^MgCoins_(PlaceBet|AddBetCoin)$'),
    ('new badge',         r'^MiniGm_(IsNew|ClearNew)$'),
    ('alt variant',       r'^_ZN\d+dScM\w+_c12IsAltVariantEv$'),
    ('wiggler pass',      r'^Wiggler_Draw(Pass[AB]|Target|Field)$'),
    ('trampoline item',   r'^TrmpItem_SetMotion\d$'),
    ('pachinko flyer/bg', r'^Pachinko(Flyer_State0|Bg_Start|Fx_Spawn)$'),
    ('teresa sprites',    r'^_ZN13dScMgTeresa_c\d+(Exclaim_Spawn|FoundMark_Spawn|VanishPuff_Spawn|RisingIcon_Spawn|FoundNumber_Spawn|Reveal_State0)Ei$'),
]
MAX_HITS = 400
# per-frame pollers: a few hits are enough
LOW_CAP = {'MiniGm_IsNew': 20, 'Wiggler_DrawPassA': 20, 'Wiggler_DrawPassB': 20, 'Wiggler_DrawTarget': 20, 'Wiggler_DrawField': 20, 'PachinkoFlyer_State0': 20, '_ZN11dScMgBase_c12IsAltVariantEv': 20}


def load_targets():
    out = []
    for mod, (symf, imgf, base) in MODULES.items():
        img = open(os.path.join(ROOT, imgf), 'rb').read()
        for line in open(os.path.join(ROOT, symf), encoding='utf-8'):
            m = re.match(r'(\S+) kind:function\S* addr:0x([0-9a-f]+)', line)
            if not m:
                continue
            name, addr = m.group(1), int(m.group(2), 16)
            for check, rx in CHECKS:
                if re.search(rx, name):
                    off = addr - base
                    out.append({'check': check, 'name': name, 'module': mod, 'addr': addr,
                                'canary': img[off:off + 8].hex()})
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', default=os.path.join(RUN, 'pass3.jsonl'))
    ap.add_argument('--port', type=int, default=3333)
    ap.add_argument('--unlock', action='store_true',
                    help='on the first menu hit, patch func_02013638 (the minigame unlock-bit '
                         'read) to return 1 so every minigame shows as unlocked; RAM only')
    a = ap.parse_args()
    stop = os.path.join(RUN, 'STOP')
    peek = os.path.join(RUN, 'PEEK')
    poke = os.path.join(RUN, 'POKE')
    if os.path.exists(stop):
        os.remove(stop)
    targets = load_targets()
    by_addr = {}
    for t in targets:
        by_addr.setdefault(t['addr'], []).append(t)
    print('[*] %d symbols at %d addresses' % (len(targets), len(by_addr)), flush=True)
    missing = [c for c, rx in CHECKS if not any(t['check'] == c for t in targets)]
    if missing:
        print('[!] no symbol found for: ' + ', '.join(missing), flush=True)
    hits = {}
    cli = RspClient(port=a.port).connect()
    try:
        cli.read_mem(0x02000000, 4)  # probe: a wedged stub accepts but never answers
        for addr in by_addr:
            cli.set_breakpoint(addr)
        cli.cont()
        cli.sock.settimeout(2.0)
        print('[*] armed and running; play now. touch build/melonds/STOP to finish.', flush=True)
        UNLOCK_AT = 0x02013638
        UNLOCK_ORIG = bytes.fromhex('0c109fe50120a0e3')   # ldr r1,=0x0209caa0 ; mov r2,#1
        UNLOCK_NEW = '0100a0e31eff2fe1'                  # mov r0,#1 ; bx lr
        unlocked = not a.unlock
        with open(a.out, 'a', encoding='utf-8') as fo:
            while True:
                if os.path.exists(stop):
                    break
                pending = bool(cli._buf) or bool(select.select([cli.sock], [], [], 0)[0])
                if os.path.exists(poke) and not pending:
                    # POKE holds "addr hexbytes" lines (RAM only); answers in POKE.out.
                    reqs = [l.split() for l in open(poke) if l.strip()]
                    os.remove(poke)
                    cli.interrupt()
                    cli.sock.settimeout(5.0)
                    cli.wait_for_stop()
                    while cli._buf or select.select([cli.sock], [], [], 0.02)[0]:
                        cli.wait_for_stop()
                    with open(poke + '.out', 'w') as po:
                        for r_ in reqs:
                            try:
                                a_, data = int(r_[0], 16), r_[1]
                                before = cli.read_mem(a_, len(data) // 2).hex()
                                res = cli.send_packet('M%x,%x:%s' % (a_, len(data) // 2, data))
                                after = cli.read_mem(a_, len(data) // 2).hex()
                                po.write('%s %s was %s now %s\n' % (r_[0], res, before, after))
                            except Exception as e:
                                po.write('%s error %s\n' % (' '.join(r_), e))
                    cli.sock.settimeout(2.0)
                    cli.cont()
                    continue
                if os.path.exists(peek) and not pending:
                    # PEEK holds "addr len" lines; halt, read, answer in PEEK.out, resume.
                    # Skipped while a stop packet is waiting: that stop is handled
                    # first, or the interrupt's reply and it would cross.
                    reqs = [l.split() for l in open(peek) if l.strip()]
                    os.remove(peek)
                    cli.interrupt()
                    cli.sock.settimeout(5.0)
                    cli.wait_for_stop()
                    # A breakpoint that fired just before the interrupt leaves a
                    # second stop reply behind; drain it so replies stay paired.
                    while cli._buf or select.select([cli.sock], [], [], 0.02)[0]:
                        cli.wait_for_stop()
                    with open(peek + '.out', 'w') as po:
                        for r_ in reqs:
                            try:  # both words are hex, with or without 0x
                                po.write('%s %s\n' % (r_[0], cli.read_mem(int(r_[0], 16), int(r_[1], 16)).hex()))
                            except Exception as e:  # a bad request must not end the session
                                po.write('%s error %s\n' % (' '.join(r_), e))
                    cli.sock.settimeout(2.0)
                    cli.cont()
                    continue
                # Poll briefly so STOP and PEEK are seen within 50 ms; only read
                # a stop packet once bytes are waiting, so none is cut in half.
                if not cli._buf and not select.select([cli.sock], [], [], 0.05)[0]:
                    continue
                try:
                    cli.wait_for_stop()
                except socket.timeout:
                    continue
                regs = cli.read_registers()
                pc = regs['pc']
                live = None
                if pc in by_addr:
                    seen = cli.read_mem(pc, 8).hex()
                    live = next((t for t in by_addr[pc] if t['canary'] == seen), None)
                if live and not unlocked and live['module'] == 'ov005':
                    cur = cli.read_mem(UNLOCK_AT, 8)
                    if cur == UNLOCK_ORIG:
                        r = cli.send_packet('M%x,8:%s' % (UNLOCK_AT, UNLOCK_NEW))
                        ok = cli.read_mem(UNLOCK_AT, 8).hex() == UNLOCK_NEW
                        print('[*] unlock patch %s (%s); leave and re-enter the minigame menu' % (
                            'applied' if ok else 'FAILED', r), flush=True)
                    else:
                        print('[!] unlock patch skipped: bytes at %#x are %s' % (UNLOCK_AT, cur.hex()), flush=True)
                    unlocked = True
                if live:
                    n = hits.get(live['name'], 0) + 1
                    hits[live['name']] = n
                    rec = {'t': round(time.time(), 3), 'check': live['check'], 'name': live['name'],
                           'module': live['module'], 'n': n,
                           'r0': regs['r0'], 'r1': regs['r1'], 'r2': regs['r2'], 'r3': regs['r3'],
                           'lr': regs['lr']}
                    fo.write(json.dumps(rec) + '\n'); fo.flush()
                    print('%-18s %-40s r0=%#x r1=%#x r2=%#x lr=%#010x' % (
                        live['check'], live['name'][:40], rec['r0'], rec['r1'], rec['r2'], rec['lr']), flush=True)
                    if n >= LOW_CAP.get(live['name'], MAX_HITS):
                        cli.clear_breakpoint(pc)
                        print('[*] %s reached %d hits; cleared' % (live['name'], n), flush=True)
                cli.cont()
    finally:
        cli.detach()
        cli.close()
        print('[*] detached. hits: ' + json.dumps(hits), flush=True)


if __name__ == '__main__':
    main()
