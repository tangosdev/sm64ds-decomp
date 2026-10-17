"""Read the live minigame scene through mgwatch's PEEK file.
usage: python tools/trace/mg/mgpeek.py [offset len] ...   (hex; offsets from gMgScene)"""
import os, struct, sys, time
HERE = os.path.dirname(os.path.abspath(__file__))
RUN = os.path.join(HERE, '..', '..', '..', 'build', 'melonds')
PEEK = os.path.join(RUN, 'PEEK')
GMGSCENE = 0x020beb68


def peek_many(reqs):
    out = PEEK + '.out'
    if os.path.exists(out):
        os.remove(out)
    with open(PEEK, 'w') as f:
        for a, n in reqs:
            f.write('%x %x\n' % (a, n))
    for _ in range(200):
        if os.path.exists(out):
            time.sleep(0.05)
            res = [bytes.fromhex(l.split()[1]) if len(l.split()) == 2 else None
                   for l in open(out).read().splitlines()]
            if len(res) == len(reqs):
                return res
        time.sleep(0.03)
    return [None] * len(reqs)


if __name__ == '__main__':
    scene = struct.unpack('<I', peek_many([(GMGSCENE, 4)])[0])[0]
    print('gMgScene = %#x' % scene)
    args = sys.argv[1:] or ['0', '10']
    for off, n in zip(args[::2], args[1::2]):
        r = peek_many([(scene + int(off, 16), int(n, 16))])[0]
        print('+%s: %s' % (off, r.hex() if r else r))
