"""blindround: one tool for a pre-registered, blind A/B round on the emulator.

  python tools/trace/blindround.py spec      ROUND [--discard]   design.json -> specs, labels, predictions, JUDGE.md
  python tools/trace/blindround.py lint      ROUND               design checks (also run by spec and register)
  python tools/trace/blindround.py dryrun    ROUND               answer every question from the predictions
  python tools/trace/blindround.py pilot     ROUND [--spec S]    mechanics only; prints pass/fail, nothing else
  python tools/trace/blindround.py register  ROUND               write ROUND/REGISTERED.sha256
  python tools/trace/blindround.py run       ROUND [--spec S]    run the events (refuses unless registered)
  python tools/trace/blindround.py selfcheck ROUND               the judge folder is complete and leak-free
  python tools/trace/blindround.py score     ROUND               after every judge/<spec>/my_judgments.json

run and pilot need `ask.py serve --backend ctl`. The other commands are offline.

ROUND is a planner-only directory outside the repo (it holds savestate paths,
screenshots and the key). The planner writes ROUND/design.json:

  {"round": "r16", "seed": "16",
   "specs": [{"name": "s16a", "state": "C:/.../B1.ml", "hook": {"addr": "0x02019390", "module": "arm9"},
              "settle": 4, "screen": "top", "guards": [{"name": "g", "expr": "0x209f250", "size": 1, "want": 0}],
              "locate": null,
              "question": {"text": "...", "choices": ["...", "..."]},
              "how": "Each Qxx has one image, pairs/Qxx.png: ...",
              "conditions": [{"label": "f310=5", "count": 2, "real": true, "expect": "5",
                              "steps": [{"op": "step", "n": 6}, {"op": "shot", "as": "A"},
                                        {"op": "set", "n": 20, "pokes": [{"expr": "0x209f310", "op": "write",
                                                                          "value": 5, "size": 1}]},
                                        {"op": "shot", "as": "B"}]}],
              "prediction": {"hypothesis": "...", "expected": "...", "risks": ["..."]}}]}

Steps: step (n hooked frames), shot (as A or B), set (n frames of pokes). A set
re-applies each poke every frame (a latch or flag the game clears is held); a
poke with "every": "once" is applied on the first frame only, and lint flags it
unless it also carries "once_ok": "<reason>". "hold" is accepted as a synonym of set.
Poke ops: write (value under mask), set, clear, flip (mask), same (sham: rewrite
the value read).

Layout written by spec and run:

  design.json, labels.json            planner only: c-name -> label
  specs/<s>.json, predictions/<s>.md  the registered design (c-names only, scrambled)
  REGISTERED.sha256                   register: hashes of all of the above, JUDGE.md and this tool
  judge/                              the only folder a judge gets: JUDGE.md, <s>/answers.json, <s>/pairs/Qxx.png
  key/<s>/                            key.json, log.jsonl, frames/ (raw A/B captures): sealed, outside judge/
  scores/<s>.json, scores/SUMMARY.md  score: outside judge/

Protections built in:
  - registration gate: run refuses unless REGISTERED.sha256 exists and every spec,
    prediction, labels.json, design.json, JUDGE.md and this tool still hash to it, and
    no spec or prediction is unregistered; spec deletes REGISTERED.sha256, so any
    respec invalidates the registration; score re-checks it and refuses until every
    registered spec has judge/<s>/my_judgments.json;
  - each event's settle gets an offset of 0..2N-1 hooked frames, unique within the
    spec (N = its events), so no two events of a spec capture at the same frame count;
  - every judge PNG is rewritten with only IHDR/PLTE/IDAT/IEND (no text chunks);
  - selfcheck: judge/ holds only allowed files; no path, address, c-name, label or
    planner word in its text; no ancillary PNG chunk; no two judge images identical;
  - pilot reports pass/fail only (guards, pokes landed, hook hit): no images, no values.
"""
import argparse
import datetime
import hashlib
import json
import os
import random
import re
import shutil
import struct
import sys
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
TOOL = os.path.abspath(__file__)

STEP_OPS = ('step', 'shot', 'set', 'hold')
POKE_OPS = ('write', 'set', 'clear', 'flip', 'same')
BAD_WORDS = ['sham', 'control', 'expect', '"real"', 'hypothes', 'predict', 'poke', 'label', 'key.json', 'spec.json',
             'score', 'suite', 'savestate', 'symbol', 'src/', '.cpp', '.h ', 'gen/', 'origin', 'register',
             'condition']
PATH_RX = re.compile(r'[A-Za-z]:[\\/]|\\|\.\./|/tmp|\bUsers\b|scratchpad|sm64ds', re.I)
ADDR_RX = re.compile(r'0x[0-9a-f]{3,}|\b0?2[01][0-9a-f]{5}\b', re.I)
CNAME_RX = re.compile(r'\bc\d+\b')
PNG_KEEP = (b'IHDR', b'PLTE', b'IDAT', b'IEND')
JUDGE_FILES = ('answers.json', 'pairs', 'my_judgments.json')


class Refused(Exception):
    """A gate said no: the message says why. main() prints it and exits 2."""


# ---------------------------------------------------------------- files
def _p(rd, *a):
    return os.path.join(rd, *a)


def _load(path):
    with open(path, encoding='utf-8') as f:
        return json.load(f)


def _dump(path, obj):
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(obj, f, indent=1)
        f.write('\n')


def _sha(path):
    with open(path, 'rb') as f:
        return hashlib.sha256(f.read()).hexdigest()


def _seed(*parts):
    return int(hashlib.sha256(''.join(parts).encode()).hexdigest()[:8], 16)


def spec_names(rd):
    d = _p(rd, 'specs')
    return sorted(f[:-5] for f in os.listdir(d) if f.endswith('.json')) if os.path.isdir(d) else []


def _int(x):
    return int(x, 0) if isinstance(x, str) else int(x)


# ---------------------------------------------------------------- design -> spec
def _norm_steps(steps):
    out = []
    for st in steps:
        st = dict(st)
        if st['op'] == 'hold':
            st['op'] = 'set'
        if st['op'] == 'set':
            ps = []
            for p in st['pokes']:
                p = dict(p)
                p.setdefault('op', 'write')
                p.setdefault('size', 1)
                p.setdefault('mask', 0 if p['op'] == 'same' else (1 << (8 * _int(p['size']))) - 1)
                p.setdefault('every', 'frame')
                ps.append(p)
            st['pokes'] = ps
        out.append(st)
    return out


def build_specs(design):
    """design -> ({name: spec}, {name: {cname: label}}). Conditions get c-names c1..cN,
    scrambled per spec (seeded) unless the spec says "scramble": false."""
    tag = str(design.get('seed', design.get('round', '')))
    specs, labels = {}, {}
    for sd in design['specs']:
        conds = sd['conditions']
        names = ['c%d' % (i + 1) for i in range(len(conds))]
        if sd.get('scramble', True):
            random.Random(_seed('cname', tag, ':', sd['name'])).shuffle(names)
        lab, cs = {}, []
        for cn, c in zip(names, conds):
            lab[cn] = c['label']
            cs.append({'name': cn, 'count': int(c['count']), 'real': bool(c['real']), 'expect': c['expect'],
                       'steps': _norm_steps(c['steps'])})
        cs.sort(key=lambda c: int(c['name'][1:]))
        spec = {k: v for k, v in sd.items() if k not in ('conditions', 'prediction', 'scramble')}
        spec['hook'] = dict(sd['hook'], addr=_int(sd['hook']['addr']))
        spec.setdefault('settle', 4)
        spec.setdefault('screen', 'top')
        spec.setdefault('guards', [])
        spec.setdefault('locate', None)
        spec['seed'] = tag
        spec['conditions'] = cs
        specs[sd['name']] = spec
        labels[sd['name']] = lab
    return specs, labels


def prediction_md(name, spec, labels, pred):
    L = ['# %s prediction (pre-registered before any condition ran)' % name, '',
         '- spec: specs/%s.json, state %s' % (name, os.path.basename(spec['state'])), '',
         '## Hypothesis', '', pred.get('hypothesis', ''), '', '## Expected', '', pred.get('expected', ''), '',
         '| c-name | label | real | n | expected |', '|---|---|---|---|---|']
    for c in spec['conditions']:
        L.append('| %s | %s | %s | %d | %s |' % (c['name'], labels[c['name']], c['real'], c['count'], c['expect']))
    L += ['', '## Risks', ''] + ['- ' + r for r in pred.get('risks') or ['none listed']] + ['']
    return '\n'.join(L)


def judge_md(specs):
    names = sorted(specs)
    n = sum(c['count'] for s in specs.values() for c in s['conditions'])
    rng = names[0] if len(names) == 1 else '`%s` to `%s`' % (names[0], names[-1])
    return '\n'.join([
        '# Judge instructions', '',
        'This folder is everything you need. Do not open anything outside it.', '',
        '%d test%s, %s, %d events in all. Each event is one run of a Nintendo DS game in an' % (
            len(names), '' if len(names) == 1 else 's', rng, n),
        'emulator, started fresh. Each event gave two screen captures, A (before) and B (after).', '',
        '## What to read', '',
        'For each test folder:', '',
        '- `answers.json` holds:',
        '  - `question`: what to decide for every Qxx;',
        '  - `choices`: the allowed answers (copy one exactly);',
        '  - `confidence_choices`: `high`, `medium` or `low`;',
        '  - `how`: what the material is;',
        '  - `answers`: one entry per Qxx, with `answer` and `confidence` both null.',
        '- `pairs/Qxx.png` is one image per Qxx. The left half is A and the right half is B.', '',
        'The events in a test are in a scrambled order, and the captures do not all come from the same frame',
        'count. Judge each Qxx only on its own image. There is no other information about the runs anywhere in',
        'this folder.', '',
        '## Your output', '',
        "For each test, write `my_judgments.json` in its folder: a copy of that test's `answers.json` with every",
        '`answer` and `confidence` filled in. If a Qxx is unreadable or ambiguous, answer as best you can, mark it',
        '`low`, and add a `"note"` field to that entry. A Qxx that is absent from answers.json failed to run.',
        'Ignore it.', '',
        'When every file is written, you are done. Report that they are written; someone else compares them',
        'with the runs afterwards.', ''])


def plan_for(spec):
    """The event order: every condition `count` times, shuffled (seeded per spec)."""
    plan = []
    for c in spec['conditions']:
        plan += [c['name']] * int(c['count'])
    random.Random(_seed('plan', spec.get('seed', ''), ':', spec['name'])).shuffle(plan)
    return plan


def offsets_for(spec, n):
    """n settle offsets in 0..2n-1, all different (seeded per spec)."""
    return random.Random(_seed('offset', spec.get('seed', ''), ':', spec['name'])).sample(range(2 * n), n)


def _ran(rd, name):
    return os.path.exists(_p(rd, 'key', name, 'key.json')) or os.path.exists(_p(rd, 'judge', name, 'answers.json'))


def cmd_spec(a):
    rd = a.round
    design = _load(_p(rd, 'design.json'))
    old = spec_names(rd)
    ran = [n for n in set(old) | {s['name'] for s in design['specs']} if _ran(rd, n)]
    if ran and not a.discard:
        raise Refused('spec: %s already ran; respec with --discard (moves judge/key/scores to discarded_N/)'
                      % ', '.join(sorted(ran)))
    if ran:
        i = 1
        while os.path.exists(_p(rd, 'discarded_%d' % i)):
            i += 1
        dd = _p(rd, 'discarded_%d' % i)
        for sub in ('judge', 'key', 'scores', 'REGISTERED.sha256'):
            if os.path.exists(_p(rd, sub)):
                os.makedirs(dd, exist_ok=True)
                shutil.move(_p(rd, sub), _p(dd, sub))
        print('spec: earlier run moved to %s' % dd)
    reg = _p(rd, 'REGISTERED.sha256')
    if os.path.exists(reg):
        os.remove(reg)
        print('spec: REGISTERED.sha256 removed (a respec invalidates the registration)')
    specs, labels = build_specs(design)
    for d in ('specs', 'predictions'):
        if os.path.isdir(_p(rd, d)):
            shutil.rmtree(_p(rd, d))
    os.makedirs(_p(rd, 'predictions'))
    for n, s in specs.items():
        _dump(_p(rd, 'specs', n + '.json'), s)
        md = prediction_md(n, s, labels[n], next(x for x in design['specs'] if x['name'] == n).get('prediction', {}))
        with open(_p(rd, 'predictions', n + '.md'), 'w', encoding='utf-8', newline='\n') as f:
            f.write(md)
    _dump(_p(rd, 'labels.json'), labels)
    os.makedirs(_p(rd, 'judge'), exist_ok=True)
    with open(_p(rd, 'judge', 'JUDGE.md'), 'w', encoding='utf-8', newline='\n') as f:
        f.write(judge_md(specs))
    print('spec: %d specs, %d events' % (len(specs), sum(len(plan_for(s)) for s in specs.values())))
    flags = lint(rd)
    _print_flags(flags)


# ---------------------------------------------------------------- lint and dryrun
def _flag(out, level, spec, what):
    out.append({'level': level, 'spec': spec, 'flag': what})


def lint_spec(spec, out):
    n = spec['name']
    ch = spec['question']['choices']
    if len(set(ch)) != len(ch):
        _flag(out, 'error', n, 'duplicate choices')
    for c in spec['conditions']:
        if c['expect'] not in ch:
            _flag(out, 'error', n, '%s: expected answer %r is not one of the choices' % (c['name'], c['expect']))
        shots = [st.get('as') for st in c['steps'] if st['op'] == 'shot']
        if sorted(shots) != ['A', 'B']:
            _flag(out, 'error', n, '%s: needs exactly one shot A and one shot B (has %s)' % (c['name'], shots))
        for st in c['steps']:
            if st['op'] not in STEP_OPS:
                _flag(out, 'error', n, '%s: unknown step op %r' % (c['name'], st['op']))
            for p in st.get('pokes', []):
                if p['op'] not in POKE_OPS:
                    _flag(out, 'error', n, '%s: unknown poke op %r' % (c['name'], p['op']))
                if p.get('every') == 'once' and p['op'] != 'same' and not p.get('once_ok'):
                    _flag(out, 'warn', n, '%s: %s %s is poked once; a latch or flag the game clears is lost '
                          '(hold it every frame, or give once_ok)' % (c['name'], p['op'], p['expr']))
                if p['op'] == 'flip' and p.get('every', 'frame') == 'frame' and int(st.get('n', 1)) > 1:
                    _flag(out, 'warn', n, '%s: flip %s every frame toggles it each frame (use set/clear to hold '
                          'a flag, or every: once)' % (c['name'], p['expr']))
    if not any(not c['real'] for c in spec['conditions']):
        _flag(out, 'warn', n, 'no null (real: false) condition')


def lint(rd):
    out = []
    specs = {n: _load(_p(rd, 'specs', n + '.json')) for n in spec_names(rd)}
    for s in specs.values():
        lint_spec(s, out)
    # two specs on one state and screen with overlapping settle windows can capture the same frames
    win = {}
    for s in specs.values():
        n = len(plan_for(s))
        lo = int(s['settle'])
        win.setdefault((os.path.normcase(os.path.abspath(s['state'])), s['screen']), []).append(
            (lo, lo + 2 * n - 1, s['name']))
    for ws in win.values():
        ws.sort()
        for (a0, a1, an), (b0, b1, bn) in zip(ws, ws[1:]):
            if b0 <= a1:
                _flag(out, 'warn', an, 'shares state and screen with %s and its settle window %d..%d overlaps '
                      '%d..%d: give one a later settle' % (bn, a0, a1, b0, b1))
    return out


_STASIS = re.compile(r'\b(froze|frozen|freez\w*|stopped|stops|paused|stuck|stand(s|ing)? still|still)\b')
_NOCHANGE = re.compile(r'\b(no change|nothing changed|unchanged|the same|same as|identical|no difference)\b')
_GENERIC = re.compile(r'\b(changed|change|different|moved|moves)\b')
_EXCL = re.compile(r'\b(else|other|another)\b')


def ambiguous_choices(choices):
    """Pairs of choices one image could fit both of: a heuristic."""
    low = [c.lower() for c in choices]
    out = []
    stasis = [c for c in low if _STASIS.search(c)]
    nochg = [c for c in low if _NOCHANGE.search(c)]
    generic = [c for c in low if _GENERIC.search(c) and not _EXCL.search(c) and not _NOCHANGE.search(c)
               and len(re.findall(r'\w+', c)) <= 4]
    for s in stasis:
        for z in nochg:
            if s != z:
                out.append((s, z, 'a frozen picture is also no change between A and B'))
        for g in generic:
            if s != g:
                out.append((s, g, 'a freeze is also a change against the expected motion'))
    for i, x in enumerate(low):
        for y in low[i + 1:]:
            wx, wy = set(re.findall(r'\w+', x)), set(re.findall(r'\w+', y))
            if wx and wx < wy and not _EXCL.search(y):
                out.append((x, y, 'every word of the first is in the second'))
            elif wy and wy < wx and not _EXCL.search(x):
                out.append((y, x, 'every word of the first is in the second'))
    seen, uniq = set(), []
    for o in out:
        k = tuple(sorted(o[:2]))
        if k not in seen:
            seen.add(k)
            uniq.append(o)
    return uniq


def dryrun_spec(spec):
    """Answer every event from the predictions; report what the choices can't tell apart."""
    from math import comb
    out = []
    n = spec['name']
    ch = spec['question']['choices']
    conds = {c['name']: c for c in spec['conditions']}
    plan = plan_for(spec)
    answers = {'Q%02d' % (i + 1): conds[cn]['expect'] for i, cn in enumerate(plan)}
    null = {c['expect'] for c in spec['conditions'] if not c['real']}
    for c in spec['conditions']:
        if c['real'] and c['expect'] in null:
            _flag(out, 'error', n, '%s (real) predicts %r, the same answer as a null condition: the choices '
                  'cannot tell the effect from no effect' % (c['name'], c['expect']))
    if len({c['expect'] for c in spec['conditions']}) == 1:
        _flag(out, 'error', n, 'every condition predicts %r: a judge answering it every time agrees with all'
              % spec['conditions'][0]['expect'])
    events = len(plan)
    best = max(sum(1 for cn in plan if conds[cn]['expect'] == x) for x in ch)
    if events and best * 2 >= events:
        _flag(out, 'warn', n, 'one constant answer agrees with %d of %d events' % (best, events))
    for x, y, why in ambiguous_choices(ch):
        _flag(out, 'warn', n, 'choices %r and %r could both fit one image (%s)' % (x, y, why))
    p = 1.0 / len(ch)
    pmax = p ** events
    if events and pmax > 0.01:
        _flag(out, 'warn', n, 'even %d/%d agreement has p = %.3g; add events' % (events, events, pmax))
    const_p = sum(comb(events, i) * p ** i * (1 - p) ** (events - i) for i in range(best, events + 1))
    return {'spec': n, 'events': events, 'answers': answers, 'best_constant_agree': best,
            'best_constant_p': const_p, 'all_agree_p': pmax}, out


def cmd_lint(a):
    flags = lint(a.round)
    _print_flags(flags)
    if any(f['level'] == 'error' for f in flags):
        sys.exit(1)


def cmd_dryrun(a):
    rd = a.round
    flags = lint(rd)
    res = []
    for n in spec_names(rd):
        r, f = dryrun_spec(_load(_p(rd, 'specs', n + '.json')))
        res.append(r)
        flags += f
    for r in res:
        print(json.dumps({k: r[k] for k in ('spec', 'events', 'best_constant_agree', 'best_constant_p',
                                             'all_agree_p')}))
    _dump(_p(rd, 'DRYRUN.json'), {'results': res, 'flags': flags})
    _print_flags(flags)
    if any(f['level'] == 'error' for f in flags):
        sys.exit(1)


def _print_flags(flags):
    for f in flags:
        print('%-5s %s: %s' % (f['level'], f['spec'], f['flag']))
    print('flags: %d error, %d warn' % (sum(f['level'] == 'error' for f in flags),
                                         sum(f['level'] == 'warn' for f in flags)))


# ---------------------------------------------------------------- registration
def registered_files(rd):
    fs = ['design.json', 'labels.json', 'judge/JUDGE.md']
    fs += ['specs/%s.json' % n for n in spec_names(rd)]
    pd = _p(rd, 'predictions')
    fs += ['predictions/%s' % f for f in sorted(os.listdir(pd))] if os.path.isdir(pd) else []
    if os.path.exists(_p(rd, 'PILOT.json')):
        fs.append('PILOT.json')
    return fs


def cmd_register(a):
    rd = a.round
    if any(_ran(rd, n) for n in spec_names(rd)):
        raise Refused('register: a spec already ran; respec with --discard first')
    flags = lint(rd)
    for n in spec_names(rd):
        flags += dryrun_spec(_load(_p(rd, 'specs', n + '.json')))[1]
    _print_flags(flags)
    if any(f['level'] == 'error' for f in flags):
        raise Refused('register: fix the design errors first')
    lines = ['%s  %s' % (_sha(_p(rd, f)), f) for f in registered_files(rd)]
    lines.append('%s  tool:blindround.py' % _sha(TOOL))
    lines.append('# registered %s, %d warning flag(s)' % (
        datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ'),
        sum(f['level'] == 'warn' for f in flags)))
    with open(_p(rd, 'REGISTERED.sha256'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')
    print('\n'.join(lines))


def check_registration(rd, what):
    reg = _p(rd, 'REGISTERED.sha256')
    if not os.path.exists(reg):
        raise Refused('%s: no REGISTERED.sha256 (run register first; a respec removes it)' % what)
    want = {}
    with open(reg, encoding='utf-8') as f:
        for line in f:
            if line.strip() and not line.startswith('#'):
                h, p = line.rstrip('\n').split('  ', 1)
                want[p] = h
    bad = []
    for p, h in want.items():
        path = TOOL if p == 'tool:blindround.py' else _p(rd, p)
        if not os.path.exists(path):
            bad.append('%s missing' % p)
        elif _sha(path) != h:
            bad.append('%s changed' % p)
    for p in registered_files(rd):
        if p not in want:
            bad.append('%s not registered' % p)
    if bad:
        raise Refused('%s: registration does not match: %s' % (what, '; '.join(bad)))
    return want


# ---------------------------------------------------------------- PNG
def strip_png(path):
    """Rewrite a PNG with only its critical chunks (IHDR, PLTE, IDAT, IEND). Returns the dropped chunk types."""
    with open(path, 'rb') as f:
        b = f.read()
    if b[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError('%s: not a PNG' % path)
    i, keep, dropped = 8, [b[:8]], []
    while i < len(b):
        n, t = struct.unpack('>I4s', b[i:i + 8])
        whole = b[i:i + 12 + n]
        if t in PNG_KEEP:
            keep.append(whole)
        else:
            dropped.append(t.decode('latin-1'))
        i += 12 + n
    if dropped:
        with open(path, 'wb') as f:
            f.write(b''.join(keep))
    return dropped


def png_chunks(b):
    i, out = 8, []
    while i + 8 <= len(b):
        n, t = struct.unpack('>I4s', b[i:i + 8])
        out.append(t.decode('latin-1'))
        i += 12 + n
    return out


def write_png(path, w, h, rgb):
    """Minimal RGB PNG writer (tests)."""
    raw = b''.join(b'\x00' + rgb[y * w * 3:(y + 1) * w * 3] for y in range(h))

    def chunk(t, d):
        return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) +
                chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b''))


def compose_pair(a_png, b_png, out_png):
    import ask
    ask.compose_pair(a_png, b_png, out_png)


# ---------------------------------------------------------------- emulator
def _sub(x, node):
    if isinstance(x, str) and node is not None:
        return x.replace('NODE', hex(node))
    return x


def start(c, spec, off):
    """Load the state, set the hook, settle + off hooked frames, locate. -> (guards read, bad names, node)."""
    c.clear_bps()
    r = c.load_state_file(path=spec['state'])
    if r.get('verdict') != 'loaded':
        raise RuntimeError('savestate did not load: %s' % r.get('verdict'))
    hook = spec['hook']
    c.set_frame(addr=_int(hook['addr']), module=hook.get('module', 'arm9'), name='hook')
    c.step(n=int(spec.get('settle', 4)) + off)
    node = None
    if spec.get('locate'):
        node = c.locate(locate=spec['locate'])
        if node is None:
            raise RuntimeError('locate found nothing')
    gd = spec.get('guards', [])
    got = c.snapshot(fields=[{'name': x['name'], 'expr': _sub(x['expr'], node), 'size': x.get('size', 1)}
                             for x in gd]) if gd else {}
    bad = [x['name'] for x in gd if got[x['name']] != x['want']]
    return got, bad, node


def _landed(p, r):
    """Did a poke's read-back hold what it wrote?"""
    m, new, old = _int(p['mask']), r['new'], r['old']
    want = {'write': _int(p.get('value', 0)) & m, 'set': m, 'clear': 0, 'flip': (old ^ m) & m,
            'same': old & m}[p['op']]
    return (new & m) == want


def do_poke(c, p, node):
    return c.poke(expr=_sub(p['expr'], node), op=p['op'], mask=_int(p['mask']), value=p.get('value', 0),
                  size=_int(p['size']))


def run_steps(c, cond, node, shot):
    """Run a condition's steps; shot(as) captures. -> (step records, pokes landed on every frame)."""
    recs, landed = [], True
    for st in cond['steps']:
        op = st['op']
        if op == 'step':
            c.step(n=int(st['n']))
        elif op == 'set':
            first = last = None
            for i in range(int(st['n'])):
                rs = []
                for p in st['pokes']:
                    if p.get('every', 'frame') == 'frame' or i == 0:
                        r = do_poke(c, p, node)
                        landed = landed and _landed(p, r)
                        rs.append(r)
                first = first or rs
                last = rs or last
                c.step(n=1)
            recs.append({'set': int(st['n']), 'first': first, 'last': last})
        elif op == 'shot':
            shot(st['as'])
    return recs, landed


def run_event(c, spec, cond, qid, rd, off):
    name = spec['name']
    keyd = _p(rd, 'key', name)
    rec = {'id': qid, 'condition': cond['name'], 'real': cond['real'], 'expected': cond['expect'],
           'settle_offset': off}
    got, bad, node = start(c, spec, off)
    rec['guards'] = got
    rec['node'] = node and '%08x' % node
    if bad:
        raise RuntimeError('guard failed: %s' % bad)
    shots = {}

    def shot(side):
        p = _p(keyd, 'frames', '%s_%s.png' % (qid, side))
        c.shot(path=p, which=spec.get('screen', 'top'), settle_ms=0)
        shots[side] = p
    rec['steps'], rec['pokes_landed'] = run_steps(c, cond, node, shot)
    pair = _p(rd, 'judge', name, 'pairs', qid + '.png')
    compose_pair(shots['A'], shots['B'], pair)
    rec['pair_chunks_dropped'] = strip_png(pair)
    rec['sha'] = {k: _sha(v) for k, v in shots.items()}
    rec['pair_sha'] = _sha(pair)
    rec['status'] = 'ok'
    return rec


def _client():
    import emusession as es
    return es.Client()


def run_spec(c, rd, name):
    spec = _load(_p(rd, 'specs', name + '.json'))
    jd, keyd = _p(rd, 'judge', name), _p(rd, 'key', name)
    for d in (_p(jd, 'pairs'), _p(keyd, 'frames')):
        os.makedirs(d, exist_ok=True)
    plan = plan_for(spec)
    offs = offsets_for(spec, len(plan))
    conds = {x['name']: x for x in spec['conditions']}
    q = spec['question']
    answers = {'spec': name, 'question': q['text'], 'choices': q['choices'],
               'confidence_choices': ['high', 'medium', 'low'], 'how': spec['how'], 'answers': {}}
    key = {'spec': 'specs/%s.json' % name, 'spec_sha256': _sha(_p(rd, 'specs', name + '.json')), 'name': name,
           'choices': q['choices'], 'plan': plan, 'offsets': offs, 'events': {}}
    t0 = time.time()
    with open(_p(keyd, 'log.jsonl'), 'w', encoding='utf-8') as logf:
        for i, cn in enumerate(plan):
            qid = 'Q%02d' % (i + 1)
            try:
                rec = run_event(c, spec, conds[cn], qid, rd, offs[i])
                answers['answers'][qid] = {'answer': None, 'confidence': None}
            except Exception as e:  # noqa: BLE001 -- recorded in the key; the event leaves the judge material
                rec = {'id': qid, 'condition': cn, 'status': 'error', 'error': repr(e)}
                pair = _p(jd, 'pairs', qid + '.png')
                if os.path.exists(pair):
                    os.remove(pair)
            key['events'][qid] = rec
            logf.write(json.dumps(rec) + '\n')
            logf.flush()
            # the planner sees progress only: no condition, no outcome
            print(json.dumps({'spec': name, 'id': qid, 'status': rec['status'], 'n': i + 1, 'of': len(plan),
                              'elapsed_s': round(time.time() - t0)}), flush=True)
    c.clear_bps()
    _dump(_p(keyd, 'key.json'), key)
    _dump(_p(jd, 'answers.json'), answers)


def cmd_run(a, client=None):
    rd = a.round
    check_registration(rd, 'run')
    names = a.spec or spec_names(rd)
    done = [n for n in names if _ran(rd, n)]
    if done:
        raise Refused('run: %s already ran (a rerun needs spec --discard and a new registration)' % ', '.join(done))
    c = client or _client()
    for n in names:
        run_spec(c, rd, n)
    if all(_ran(rd, n) for n in spec_names(rd)):
        ok = selfcheck(rd)
        print('judge folder: %s (%s)' % (_p(rd, 'judge'), 'selfcheck OK' if ok else 'selfcheck FAILED'))


def cmd_pilot(a, client=None):
    """Mechanics only: for each condition, one event without captures. Prints and stores
    pass/fail for guards, hook and pokes; never a value, an image or an outcome."""
    rd = a.round
    c = client or _client()
    res = {}
    for n in a.spec or spec_names(rd):
        spec = _load(_p(rd, 'specs', n + '.json'))
        for cond in spec['conditions']:
            r = {'guards': False, 'hook': False, 'pokes_landed': False}
            try:
                _, bad, node = start(c, spec, 0)
                r['hook'] = True
                r['guards'] = not bad
                if not bad:
                    r['pokes_landed'] = run_steps(c, cond, node, lambda side: None)[1]
            except Exception as e:  # noqa: BLE001
                r['error'] = type(e).__name__
            r['pass'] = r['guards'] and r['hook'] and r['pokes_landed']
            res['%s/%s' % (n, cond['name'])] = r
            print(json.dumps(dict(event='%s/%s' % (n, cond['name']), **r)), flush=True)
    c.clear_bps()
    _dump(_p(rd, 'PILOT.json'), {'pilot': res, 'all_pass': all(r['pass'] for r in res.values())})
    print('pilot: %d/%d pass' % (sum(r['pass'] for r in res.values()), len(res)))
    if not all(r['pass'] for r in res.values()):
        sys.exit(1)


# ---------------------------------------------------------------- selfcheck
def selfcheck(rd, quiet=False):
    J = _p(rd, 'judge')
    labels = _load(_p(rd, 'labels.json'))
    problems, notes = [], []
    names = spec_names(rd)
    for f in os.listdir(J):
        if f != 'JUDGE.md' and f not in names:
            problems.append('stray judge/%s' % f)
    for root, dirs, files in os.walk(J):
        for x in dirs + files:
            if os.path.islink(os.path.join(root, x)):
                problems.append('link in judge/: %s' % x)
    with open(_p(J, 'JUDGE.md'), encoding='utf-8') as f:
        texts = [('JUDGE.md', f.read())]
    pngs = {}
    for n in names:
        d = _p(J, n)
        if not os.path.isdir(d):
            problems.append('%s: not run' % n)
            continue
        key = _load(_p(rd, 'key', n, 'key.json'))
        ok = sorted(q for q, e in key['events'].items() if e['status'] == 'ok')
        with open(_p(d, 'answers.json'), encoding='utf-8') as f:
            raw = f.read()
        ans = json.loads(raw)
        if sorted(ans['answers']) != ok:
            problems.append('%s: answers %s vs ok events %s' % (n, sorted(ans['answers']), ok))
        if any(v != {'answer': None, 'confidence': None} for v in ans['answers'].values()):
            problems.append('%s: answers.json is not blank' % n)
        for f in os.listdir(d):
            if f not in JUDGE_FILES:
                problems.append('stray judge/%s/%s' % (n, f))
        for f in os.listdir(_p(d, 'pairs')):
            if os.path.splitext(f)[0] not in ok or not f.endswith('.png'):
                problems.append('%s: stray pairs/%s' % (n, f))
        for q in ok:
            p = _p(d, 'pairs', q + '.png')
            if not os.path.exists(p):
                problems.append('%s %s: no pair' % (n, q))
                continue
            with open(p, 'rb') as f:
                b = f.read()
            pngs.setdefault(hashlib.sha256(b).hexdigest(), []).append('%s/%s' % (n, q))
            extra = set(png_chunks(b)) - {t.decode() for t in PNG_KEEP}
            if extra:
                problems.append('%s %s: png chunks %s' % (n, q, sorted(extra)))
        dup = {}
        for q in ok:
            for side, h in key['events'][q].get('sha', {}).items():
                dup.setdefault((side, h), []).append(q)
        groups = [v for v in dup.values() if len(v) > 1]
        notes.append('%s: %d ok events, %d byte-identical raw capture group(s) %s' % (n, len(ok), len(groups),
                                                                                     groups))
        texts.append(('%s/answers.json' % n, raw))
        for lab in labels.get(n, {}).values():
            if lab in raw:
                problems.append('%s: label %r in answers.json' % (n, lab))
    for v in pngs.values():
        if len(v) > 1:
            problems.append('identical judge images: %s' % v)
    for nm, t in texts:
        low = t.lower()
        for w in BAD_WORDS:
            if w in low:
                problems.append('%s: word %r' % (nm, w))
        for rx, what in ((PATH_RX, 'path'), (ADDR_RX, 'address'), (CNAME_RX, 'c-name')):
            m = rx.search(t)
            if m:
                problems.append('%s: %s %r' % (nm, what, m.group(0)))
    if not quiet:
        print('\n'.join(notes))
        print('\n'.join(problems) if problems else 'selfcheck: OK (%d texts, %d images)' % (len(texts), len(pngs)))
    return not problems


def cmd_selfcheck(a):
    sys.exit(0 if selfcheck(a.round) else 1)


# ---------------------------------------------------------------- score
def cmd_score(a):
    rd = a.round
    want = check_registration(rd, 'score')
    names = [p[len('specs/'):-len('.json')] for p in want if p.startswith('specs/')]
    missing = [n for n in names if not os.path.exists(_p(rd, 'judge', n, 'my_judgments.json'))]
    if missing:
        raise Refused('score: no my_judgments.json for %s: every registered spec is judged before any key '
                      'is opened' % ', '.join(missing))
    from math import comb
    os.makedirs(_p(rd, 'scores'), exist_ok=True)
    summ = []
    for n in names:
        mj = _p(rd, 'judge', n, 'my_judgments.json')
        j = _load(mj)['answers']
        key = _load(_p(rd, 'key', n, 'key.json'))
        labels = _load(_p(rd, 'labels.json')).get(n, {})
        rows, agree = [], 0
        for qid, e in sorted(key['events'].items()):
            if e['status'] != 'ok':
                continue
            ans = (j.get(qid) or {}).get('answer')
            ok = ans == e['expected']
            agree += ok
            rows.append({'id': qid, 'condition': e['condition'], 'label': labels.get(e['condition']),
                         'real': e['real'], 'expected': e['expected'], 'answer': ans,
                         'confidence': (j.get(qid) or {}).get('confidence'), 'agree': ok})
        k, p = len(rows), 1.0 / len(key['choices'])
        pval = sum(comb(k, i) * p ** i * (1 - p) ** (k - i) for i in range(agree, k + 1))
        res = {'spec': n, 'agree': agree, 'of': k, 'chance_per_item': round(p, 3), 'p_value': pval, 'rows': rows,
               'my_judgments_sha256': _sha(mj), 'registered_sha256': _sha(_p(rd, 'REGISTERED.sha256'))}
        _dump(_p(rd, 'scores', n + '.json'), res)
        summ.append(res)
        print(json.dumps({'spec': n, 'agree': agree, 'of': k, 'p_value': pval}))
    L = ['# Scores', '', 'Generated by blindround.py score from key/*/key.json and judge/*/my_judgments.json.', '',
         '| spec | agree | of | chance/item | p |', '|---|---|---|---|---|']
    for r in summ:
        L.append('| %s | %d | %d | %.3f | %.3g |' % (r['spec'], r['agree'], r['of'], r['chance_per_item'],
                                                      r['p_value']))
    L += ['', '## Per condition', '', '| spec | c-name | label | real | expected | agree | of |',
          '|---|---|---|---|---|---|---|']
    for r in summ:
        per = {}
        for x in r['rows']:
            d = per.setdefault(x['condition'], [x['label'], x['real'], x['expected'], 0, 0])
            d[3] += x['agree']
            d[4] += 1
        for cn in sorted(per, key=lambda s: int(s[1:])):
            lab, real, exp, ag, of = per[cn]
            L.append('| %s | %s | %s | %s | %s | %d | %d |' % (r['spec'], cn, lab, real, exp, ag, of))
    with open(_p(rd, 'scores', 'SUMMARY.md'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(L) + '\n')


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    for name, fn in (('spec', cmd_spec), ('lint', cmd_lint), ('dryrun', cmd_dryrun), ('pilot', cmd_pilot),
                     ('register', cmd_register), ('run', cmd_run), ('selfcheck', cmd_selfcheck),
                     ('score', cmd_score)):
        p = sub.add_parser(name)
        p.add_argument('round')
        if name in ('run', 'pilot'):
            p.add_argument('--spec', action='append')
        if name == 'spec':
            p.add_argument('--discard', action='store_true')
        p.set_defaults(fn=fn)
    a = ap.parse_args(argv)
    try:
        a.fn(a)
    except Refused as e:
        print('REFUSED %s' % e, file=sys.stderr)
        sys.exit(2)


if __name__ == '__main__':
    main()
