"""Compare the wide conductor with its cartridge-source body and declarations.

Only the documented widening casts, local silencer, and exact diagnostic
sites are normalized. Diagnostic helper token hashes pin the reviewed logging
implementation; changing one requires a fresh review of that exception.
"""
import difflib
import hashlib
import pathlib
import re


TOKEN = re.compile(r'/\*.*?\*/|//(?:\\\r?\n|[^\n])*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|[A-Za-z_]\w*|[^\s]', re.S)
HELPER_DIGESTS = {
    'void warn_info_mode_armed_wide(void)':
        'f9fee24e03f69340415b7842db6cacc74c2df0a3a04d9476b27f33a806f6261c',
    'unsigned host_ms(void)':
        'ab54889932004774f3ef56fa9f15414e37d5ec7d20befb139e33110226eb925a',
    'void report_bound_expiry(':
        '9357cdb75dea41314d5e02120508f967121216b448dbe6d5a3eb45325b1be300',
}


SOURCE_OUTSIDE_DIGEST = 'a0e64166c4e1e10301d7ea184697ed48bdafedb1cb023827d4cfa59bc690c2f2'
WIDE_OUTSIDE_DIGEST = 'ce963e1aedc474223bbe5a2f3bcf95405edbb369499701468b68085d62ab43b0'

def without_comments(text):
    return TOKEN.sub(lambda m: ''.join('\n' if c == '\n' else ' ' for c in m.group())
                     if m.group().startswith(('/*', '//')) else m.group(), text)


def function(text, prefix):
    start = text.find(prefix)
    if start < 0 or text.find(prefix, start + len(prefix)) >= 0:
        raise ValueError('missing or repeated function: ' + prefix)
    opened, depth = False, 0
    for match in TOKEN.finditer(text, start):
        token = match.group()
        if token.startswith(('/*', '//')):
            continue
        if token == '{':
            opened = True
            depth += 1
        elif token == '}':
            depth -= 1
            if opened and depth == 0:
                return text[start:match.end()]
    raise ValueError('unterminated function: ' + prefix)


def token_digest(text):
    tokens = [m.group() for m in TOKEN.finditer(text)
              if not m.group().startswith(('/*', '//'))]
    return hashlib.sha256(' '.join(tokens).encode('utf-8')).hexdigest()


def replace_once(lines, before, after):
    hits = [i for i in range(len(lines) - len(before) + 1)
            if lines[i:i + len(before)] == before]
    if len(hits) != 1:
        raise ValueError('diagnostic site changed or moved: ' + before[0])
    i = hits[0]
    return lines[:i] + after + lines[i + len(before):]


def norm(text, wide=False):
    lines = [re.sub(r'\s+', ' ', line).strip()
             for line in without_comments(text).splitlines()]
    lines = [line for line in lines if line]
    if not wide:
        return lines
    # The declaration, increment and surrounding game condition must agree.
    # Removing arbitrary lines containing a diagnostic variable would also
    # erase a changed state assignment or a new call, so no prefix rules apply.
    lines = replace_once(lines, [
        'const unsigned vs7_bound = (unsigned)sp4;',
        'const unsigned vs7_t0 = host_ms();',
        'unsigned long vs7_turns = 0, vs7_with_data = 0;',
        'while ((sp8 == 0) && (sp4 != 0)) {',
        '++vs7_turns;',
    ], ['while ((sp8 == 0) && (sp4 != 0)) {'])
    lines = replace_once(lines, [
        'if (temp_r0_5 != 0) {', '++vs7_with_data;',
        'data_020a0f10 = func_02040704(temp_r0_5);',
    ], ['if (temp_r0_5 != 0) {', 'data_020a0f10 = func_02040704(temp_r0_5);'])
    lines = replace_once(lines, [
        'report_bound_expiry(vs7_bound, vs7_turns, vs7_with_data,',
        'host_ms() - vs7_t0);', 'data_020a0f04 = 0;',
    ], ['data_020a0f04 = 0;'])
    for before, after in (
        (['u16 temp_r1_4;', '(void)temp_r0_7;', 'if (data_02099e1c != 0) {'],
         ['u16 temp_r1_4;', 'if (data_02099e1c != 0) {']),
        (['if (data_020a0ef8 != 0) {', 'warn_info_mode_armed_wide();',
          'if (data_020a0f1c & 0x4000) {'],
         ['if (data_020a0ef8 != 0) {', 'if (data_020a0f1c & 0x4000) {']),
        (['if (data_020a1154[0].unkC & 1) {', 'warn_info_mode_armed_wide();',
          'if (data_020a0f10 != 0) {'],
         ['if (data_020a1154[0].unkC & 1) {', 'if (data_020a0f10 != 0) {']),
        (['if ((var_r6_2 != 0) && (n75 > 1U)) {', 'warn_info_mode_armed_wide();', 'spC = 1;'],
         ['if ((var_r6_2 != 0) && (n75 > 1U)) {', 'spC = 1;']),
    ):
        lines = replace_once(lines, before, after)
    for widened in (
        '} while ((s32) var_r7 < kRecs);',
        'if (var_r2 < kRecs) {', 'if (var_r5_3 < kRecs) {',
        'if (var_r3_2 < kRecs) {', '} while (var_r0 < kRecs);',
        'if (var_r2_3 < kRecs) {', 'if (var_r2_4 < kRecs) {',
    ):
        lines = replace_once(lines, [widened], [widened.replace('kRecs', '4')])
    result = []
    for line in lines:
        line = line.replace('(u8 *)func_0204068c', 'func_0204068c')
        line = line.replace('(void *)&func_0203f644, (void *)&func_0203f604',
                            '&func_0203f644, &func_0203f604')
        line = line.replace('void conductor_wide(void)', 'void func_0203ea5c(void)')
        result.append(line)
    return result


def decls(text):
    out = {}
    for match in re.finditer(r'\bextern\s+(?:"C"\s+)?([^;{}]+);', without_comments(text), re.S):
        declaration = re.sub(r'\s+', ' ', match.group(1)).strip()
        name = re.search(r'(\w+)\s*\(', declaration) or re.search(r'(\w+)\s*(?:\[|$)', declaration)
        if name is None:
            raise ValueError('unsupported extern declaration: ' + declaration)
        out.setdefault(name.group(1), []).append(declaration)
    return out


def outside_digest(text, prefixes):
    # Freeze reviewed includes, typedefs, linkage blocks, declarations and the
    # dispatch wrapper as well as function bodies. Otherwise an inactive copy
    # or a new macro can leave the body comparison unchanged while changing
    # the compiled program. These pins are review boundaries, not ROM proof.
    for prefix in prefixes:
        body = function(text, prefix)
        text = text.replace(body, prefix + ';', 1)
    return token_digest(text)


def check_sources(src, wide):
    if outside_digest(src, ['void func_0203ea5c(void)']) != SOURCE_OUTSIDE_DIGEST:
        raise ValueError('cartridge declarations or preprocessor context changed')
    if outside_digest(wide, list(HELPER_DIGESTS) + ['void conductor_wide(void)']) != WIDE_OUTSIDE_DIGEST:
        raise ValueError('wide declarations, dispatch or preprocessor context changed')
    for prefix, expected in HELPER_DIGESTS.items():
        if token_digest(function(wide, prefix)) != expected:
            raise ValueError('diagnostic helper changed: ' + prefix)
    original = norm(function(src, 'void func_0203ea5c(void)'))
    candidate = norm(function(wide, 'void conductor_wide(void)'), wide=True)
    diff = list(difflib.unified_diff(original, candidate, 'src', 'wide', lineterm=''))
    if diff:
        raise ValueError('DIVERGED:\n' + '\n'.join(diff))
    expected, actual = decls(src), decls(wide)
    for name in sorted(set(expected) | set(actual)):
        if name in ('func_0203ea5c_narrow', 'port_comms_counters_get'):
            continue  # exact extra declarations are pinned by the outside digest
        values = actual.get(name, [])
        if name == 'data_020a1154':
            values = [value.replace('[kRecs]', '[4]') for value in values]
        if expected.get(name) != values:
            raise ValueError('EXTERN DIFF ' + name)
    return {'lines': len(original), 'externs': len(expected),
            'diagnostic_helpers': len(HELPER_DIGESTS)}


def main():
    root = pathlib.Path(__file__).resolve().parents[2]
    try:
        result = check_sources((root / 'src/func_0203ea5c.c').read_text(),
                               (root / 'port/hal/comms_conductor_wide.cpp').read_text())
    except (OSError, ValueError) as exc:
        print(str(exc))
        return 1
    print('TRANSCRIPTION EXACT after normalization: %d lines' % result['lines'])
    print('EXTERNS EXACT: %d symbols' % result['externs'])
    print('DIAGNOSTIC HELPERS PINNED: %d token hashes' % result['diagnostic_helpers'])
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
