#!/usr/bin/env python3
"""Split a candidate source list into the TUs that can be compiled ONCE and the
TUs that must still be compiled per target.

WHY THIS EXISTS.  Three targets -- walk_window, walk_window_hires and
smoke_player -- each compile the whole matched slice.  Their command lines are
byte-identical except for ONE define apiece: NTR_WIDE_RT on walk_window,
NTR_HIRES on walk_window_hires, neither on smoke_player.  That made the build
compile ~10,200 sources three times to produce three identical objects for all
but a few dozen of them: 32,716 compile edges over 10,276 distinct sources.

A TU can only differ between those targets if a DIVERGENT MACRO -- one that is
defined for some of the targets and not the others -- is visible somewhere in
its include closure.  So the rule is exact and needs no compiler: build each
TU's transitive include closure over the build's own -I set, and ask whether any
file in it so much as MENTIONS a divergent macro name.  Mentioning is
deliberately coarser than using: a comment about NTR_HIRES is enough to keep a
TU per-target, which is the safe direction.

The divergent macro list is not hard-coded here.  CMakeLists.txt computes it as
the symmetric difference of the three targets' compile definitions and passes it
in, so a lane that adds a fourth define to one target and not the others gets
the correct answer without touching this file.

Output is a CMake fragment:

    set(PORT_TIER_SHARED "...;...")   # compile once
    set(PORT_TIER_PERTGT "...;...")   # compile per target

Usage:
    tierscan.py --sources LIST --out FRAGMENT
                [--incdir DIR]... [--macro NAME]... [--cache FILE]
"""

import argparse
import hashlib
import json
import os
import re
import sys

INCLUDE_RE = re.compile(rb'^[ \t]*#[ \t]*include[ \t]*[<"]([^>"\r\n]+)[>"]', re.M)


def _norm(p):
    return os.path.normcase(os.path.abspath(p))


class Scanner(object):
    def __init__(self, incdirs, macros):
        self.incdirs = [os.path.abspath(d) for d in incdirs]
        # word-boundary match so NTR_HIRES does not hit NTR_HIRES2's owner only
        # by accident -- and so a divergent NTR_HIRES still matches inside
        # "#elif defined(NTR_HIRES)".  A prefix relationship between two macro
        # names is handled by the alternation being sorted longest-first.
        names = sorted(set(macros), key=len, reverse=True)
        self.macro_re = (re.compile(rb'\b(' + b'|'.join(
            re.escape(n.encode()) for n in names) + rb')\b') if names else None)
        self._scan = {}      # normalised path -> (includes, mentions_macro)
        self._resolve = {}   # (name, fromdir) -> normalised path or None
        self._reach = {}     # normalised path -> bool

    def scan(self, path):
        key = _norm(path)
        hit = self._scan.get(key)
        if hit is not None:
            return hit
        try:
            with open(path, 'rb') as fh:
                data = fh.read()
        except OSError:
            hit = ((), False)
            self._scan[key] = hit
            return hit
        incs = tuple(m.group(1).decode('utf-8', 'replace')
                     for m in INCLUDE_RE.finditer(data))
        mentions = bool(self.macro_re and self.macro_re.search(data))
        hit = (incs, mentions)
        self._scan[key] = hit
        return hit

    def resolve(self, name, fromdir):
        key = (name, fromdir)
        if key in self._resolve:
            return self._resolve[key]
        found = None
        for base in [fromdir] + self.incdirs:
            cand = os.path.normpath(os.path.join(base, name))
            if os.path.isfile(cand):
                found = cand
                break
        self._resolve[key] = found
        return found

    def divergent(self, path):
        """True if this TU's include closure mentions any divergent macro."""
        stack = [path]
        seen = set()
        while stack:
            cur = stack.pop()
            key = _norm(cur)
            if key in seen:
                continue
            seen.add(key)
            cached = self._reach.get(key)
            if cached is True:
                return True
            incs, mentions = self.scan(cur)
            if mentions:
                self._reach[key] = True
                return True
            here = os.path.dirname(os.path.abspath(cur))
            for name in incs:
                nxt = self.resolve(name, here)
                if nxt is not None:
                    stack.append(nxt)
        for key in seen:
            self._reach.setdefault(key, False)
        return False


def signature(paths):
    """Cheap identity of the input tree: path + size + mtime_ns per file."""
    h = hashlib.sha256()
    for p in paths:
        try:
            st = os.stat(p)
            h.update(('%s|%d|%d\n' % (p, st.st_size, st.st_mtime_ns)).encode())
        except OSError:
            h.update(('%s|missing\n' % p).encode())
    return h.hexdigest()


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument('--sources', required=True,
                    help='file with one candidate source path per line')
    ap.add_argument('--out', required=True, help='CMake fragment to write')
    ap.add_argument('--incdir', action='append', default=[])
    ap.add_argument('--macro', action='append', default=[],
                    help='a macro that is defined for some targets and not '
                         'others; any TU whose closure mentions it stays '
                         'per-target')
    ap.add_argument('--cache', default=None)
    args = ap.parse_args(argv)

    with open(args.sources, encoding='utf-8') as fh:
        srcs = [ln.strip() for ln in fh if ln.strip()]

    scanner = Scanner(args.incdir, args.macro)

    # The cache key covers everything that can change the answer: this script,
    # the candidate list, the include dirs, the macro list, and the identity of
    # every file the scan could read.  The last is the expensive part to state
    # exactly, so it is approximated from the top: the candidate sources plus
    # every header under the include dirs.  A header edit anywhere in the -I set
    # busts the cache, which over-invalidates and never under-invalidates.
    key = None
    if args.cache:
        watch = list(srcs)
        for d in args.incdir:
            for root, dirs, files in os.walk(d):
                dirs[:] = [x for x in dirs if x not in ('.git', '__pycache__')]
                for f in files:
                    if f.endswith(('.h', '.hpp', '.hh', '.inc', '.inl')):
                        watch.append(os.path.join(root, f))
        h = hashlib.sha256()
        h.update(signature([os.path.abspath(__file__)]).encode())
        h.update(('|'.join(sorted(args.macro))).encode())
        h.update(('|'.join(sorted(args.incdir))).encode())
        h.update(signature(sorted(watch)).encode())
        key = h.hexdigest()
        try:
            with open(args.cache, encoding='utf-8') as fh:
                blob = json.load(fh)
            if blob.get('key') == key and os.path.isfile(args.out):
                sys.stderr.write('tierscan: cache hit (%d shared, %d per-target)\n'
                                 % (blob['shared'], blob['pertgt']))
                return 0
        except (OSError, ValueError, KeyError):
            pass

    shared, pertgt = [], []
    for s in srcs:
        (pertgt if scanner.divergent(s) else shared).append(s)

    text = ('# generated by port/tools/tierscan.py -- do not edit\n'
            'set(PORT_TIER_SHARED "%s")\n'
            'set(PORT_TIER_PERTGT "%s")\n'
            % (';'.join(shared), ';'.join(pertgt)))
    tmp = args.out + '.tmp'
    with open(tmp, 'w', encoding='utf-8', newline='\n') as fh:
        fh.write(text)
    os.replace(tmp, args.out)

    if key:
        with open(args.cache, 'w', encoding='utf-8', newline='\n') as fh:
            json.dump({'key': key, 'shared': len(shared),
                       'pertgt': len(pertgt)}, fh)
    sys.stderr.write('tierscan: %d shared, %d per-target (%s)\n'
                     % (len(shared), len(pertgt), ' '.join(args.macro) or 'no divergent macros'))
    for p in pertgt:
        sys.stderr.write('tierscan:   per-target %s\n' % p)
    return 0


if __name__ == '__main__':
    sys.exit(main())
