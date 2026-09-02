"""Prove hal/comms_conductor_wide.cpp's wide copy is src/func_0203ea5c.c.

Normalizes the wide copy back to the ROM shape -- kRecs -> 4, the two C++
casts C does not need, the one-shot info-path warning calls, the unused-local
silencer -- strips comments on both sides, and diffs. Any output but
TRANSCRIPTION EXACT is a divergence to explain or fix.

Run from a worktree root:  python transcription_diff.py
"""
import difflib
import re

src = open('src/func_0203ea5c.c').read()
body_src = src[src.index('void func_0203ea5c(void)'):]

wide = open('port/hal/comms_conductor_wide.cpp').read()
i = wide.index('void conductor_wide(void)')
j = wide.index('}  // namespace', i)
body_w = wide[i:j]

def norm(s, is_wide):
    out = []
    for ln in s.splitlines():
        t = ln.strip()
        if is_wide:
            if t.startswith('warn_info_mode_armed_wide'):
                continue
            if t == '(void)temp_r0_7;':
                continue
        out.append(ln)
    s = '\n'.join(out)
    s = re.sub(r'/\*.*?\*/', '', s, flags=re.S)
    if is_wide:
        s = s.replace('kRecs', '4')
        s = s.replace('(u8 *)func_0204068c', 'func_0204068c')
        s = s.replace('(void *)&func_0203f644, (void *)&func_0203f604',
                      '&func_0203f644, &func_0203f604')
        s = s.replace('void conductor_wide(void)', 'void func_0203ea5c(void)')
    lines = [re.sub(r'\s+', ' ', l).strip() for l in s.splitlines()]
    return [l for l in lines if l]

a = norm(body_src, False)
b = norm(body_w, True)
d = list(difflib.unified_diff(a, b, 'src', 'wide', lineterm=''))
if d:
    print('\n'.join(d))
    raise SystemExit('DIVERGED: %d diff lines' % len(d))
print('TRANSCRIPTION EXACT after normalization: %d lines' % len(a))

# And the extern declarations, verbatim -- a wrong extern type is the one
# corruption the body diff cannot see. The only tolerated difference is the
# record array's extent (Rec[4] -> Rec[kRecs]); the narrow-arm extern
# (func_0203ea5c_narrow's forward decl) is the wide file's own.
def decls(text):
    out = {}
    for m in re.finditer(r'^extern\s+(.+?);\s*$', text, re.M):
        dd = re.sub(r'\s+', ' ', m.group(1)).strip()
        nm = re.search(r'(func_[0-9a-fA-F]+|data_[0-9a-fA-F]+)', dd)
        if nm:
            out.setdefault(nm.group(1), []).append(dd)
    return out

da, db = decls(src), decls(wide)
bad = 0
for k in sorted(set(da) | set(db)):
    if k == 'func_0203ea5c':
        continue
    va = da.get(k)
    vb = [x.replace('[kRecs]', '[4]') for x in (db.get(k) or [])]
    if va != vb:
        print('EXTERN DIFF', k, '| src:', va, '| wide:', vb)
        bad += 1
if bad:
    raise SystemExit('EXTERNS DIVERGED: %d symbols' % bad)
print('EXTERNS EXACT: %d symbols' % len(set(da) | set(db)))
