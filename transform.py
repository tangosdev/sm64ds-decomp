import re, json

SRC='src/game/actors/d_a_demo.cpp'
t=open(SRC,encoding='utf-8').read()

KEEP_FREE={'func_ov002_020f2630','func_ov002_020f26d4','func_ov002_020f7d74'}

def body_end(t, open_pos):
    depth=0; i=open_pos-1
    while i<len(t):
        c=t[i]
        if c=='{': depth+=1
        elif c=='}':
            depth-=1
            if depth==0: return i
        i+=1
    return len(t)

def parse_params(p):
    p=p.strip()
    if p in ('','void'): return []
    return [x.strip() for x in p.split(',')]

def mcode(ty):
    ty=ty.strip().replace('const ','')
    # drop trailing parameter name
    ty=re.sub(r'\s*\(?\*?\s*\w+$','',ty).strip() if ' ' in ty else ty
    ptr=ty.count('*')
    core=ty.replace('*','').strip()
    base={'int':'i','unsigned int':'j','long':'l','unsigned long':'m','short':'s','s16':'s','unsigned short':'t','u16':'t','char':'c','signed char':'a','unsigned char':'h','u8':'h','bool':'b','float':'f','double':'d','void':'v','s32':'i','u32':'j','Fix12i':'i'}
    c=base.get(core)
    if c is None:
        if core=='StaffRoll::Star': c='N8daDemo_c9StaffRoll4StarE'
        elif core=='Vector3' or core=='struct Vector3': c='7Vector3'
        elif core=='SharedFilePtr': c='13SharedFilePtr'
        elif core=='OamAttr': c='7OamAttr'
        else: raise Exception('unmangled type '+ty)
    return 'P'*ptr+c

# ---------- locate free defs ----------
defs=[]
for m in re.finditer(r'\n(extern "C" )?(int|void|short|u8|s16|u16|u32|long)\s+(func_ov002_\w+)\s*\(([^)]*)\)\s*\{', t):
    pre=t[max(0,m.start(2)-5):m.start(2)]
    if '::' in pre: continue
    defs.append((m.start(),m.end(),m.group(2),m.group(3),m.group(4).strip(),m.end()))
defs.sort()

info={}
for start,hend,ret,name,params,oend in defs:
    if name in KEEP_FREE: continue
    end=body_end(t,oend)
    plist=parse_params(params)
    recv=plist[0] if plist else None
    rest=plist[1:]
    rname=recv.split()[-1].replace('*','').strip() if recv else None
    rtype=recv[:recv.rfind(rname)].strip() if recv else ''
    body=t[oend:end]
    recv_mod=bool(rname) and bool(re.search(r'\b'+re.escape(rname)+r'\s*(\+\+|--|\+=|-=|=[^=])', body))
    suffix='Ev' if not rest else 'E'+''.join(mcode(x) for x in rest)
    info[name]=dict(start=start,end=end,ret=ret,rest=rest,rname=rname,rtype=rtype,
                    body=body,recv_mod=recv_mod,
                    zname=f'_ZN8daDemo_c9StaffRoll19{name}{suffix}')

conv=set(info)

# ---------- call-site transformer ----------
def split_args(s):
    args=[]; depth=0; cur=''
    for ch in s:
        if ch in '([{': depth+=1
        elif ch in ')]}': depth-=1
        if ch==',' and depth==0: args.append(cur); cur=''
        else: cur+=ch
    args.append(cur)
    return args

def strip_cast(e):
    e=e.strip()
    m=re.match(r'^\(\s*\w[\w: ]*\*\s*\)\s*(\w[\w\-:>\*\(\) ]*)$', e, re.S)
    return m.group(1).strip() if m else e

restcount={n:len(d['rest']) for n,d in info.items()}

def transform_calls(body, rname, walked=False):
    out=[]; i=0
    pat=re.compile(r'\b(func_ov002_\w+)\s*\(')
    while True:
        m=pat.search(body,i)
        if not m: out.append(body[i:]); break
        name=m.group(1)
        # skip ::-qualified (defs/member calls)
        if body[max(0,m.start()-2):m.start()].endswith('::'):
            out.append(body[i:m.end()]); i=m.end(); continue
        j=m.end(); depth=1
        while j<len(body) and depth:
            if body[j]=='(': depth+=1
            elif body[j]==')': depth-=1
            j+=1
        if name not in conv:
            out.append(body[i:j]); i=j; continue
        args=split_args(body[m.end():j-1])
        # a free-form call passes receiver + rest args; anything else is already member-shaped
        if len(args)!=restcount[name]+1 or (len(args)==1 and not args[0].strip()):
            out.append(body[i:j]); i=j; continue
        rexpr=args[0].strip()
        rest=','.join(a.strip() for a in args[1:])
        core=strip_cast(rexpr)
        if core==rname and walked:
            newcall=f'((StaffRoll*){rname})->{name}({rest})' if rest else f'((StaffRoll*){rname})->{name}()'
        elif core==rname or core=='this':
            newcall=f'{name}({rest})' if rest else f'{name}()'
        elif core=='this->mStaffRoll':
            newcall=f'this->mStaffRoll->{name}({rest})' if rest else f'this->mStaffRoll->{name}()'
        else:
            newcall=f'({rexpr})->{name}({rest})' if rest else f'({rexpr})->{name}()'
        out.append(body[i:m.start()]); out.append(newcall); i=j
    return ''.join(out)

# ---------- rewrite each def ----------
pieces=[]
for name,d in info.items():
    body=d['body']; rname=d['rname']; rtype=d['rtype']
    nb=transform_calls(body, rname, d['recv_mod'])          # 1. calls first (match raw rname)
    if rtype.startswith('C2dd4'):
        nb=re.sub(r'\b'+re.escape(rname)+r'\s*->\s*active', 'this->credit[0].active', nb)
        nb=re.sub(r'\b'+re.escape(rname)+r'\s*->\s*step', 'this->credit[0].step', nb)
        nb=re.sub(r'\b'+re.escape(rname)+r'\b', 'this', nb)
    elif rtype.startswith('C37'):
        nb=re.sub(r'\b'+re.escape(rname)+r'\s*->\s*active', 'this->window[0].active', nb)
        nb=re.sub(r'\b'+re.escape(rname)+r'\s*->\s*kind', 'this->window[0].kind', nb)
        nb=re.sub(r'\b'+re.escape(rname)+r'\s*->\s*step', 'this->window[0].step', nb)
        nb=re.sub(r'\b'+re.escape(rname)+r'\b', 'this', nb)
    elif 'Star' in rtype:
        if d['recv_mod']:
            nb = re.sub(r'^(\s*)', r'\1StaffRoll::Star *'+rname+r' = stars;\n    ', nb, count=1)
        else:
            nb=re.sub(r'\b'+re.escape(rname)+r'\b', 'stars', nb)
    elif rname:
        if d['recv_mod']:
            # receiver is walked (p += n, e++): keep it as a local alias of this/stars
            init='stars' if 'Star' in rtype else 'this' if rtype=='void *' else f'({rtype})this'
            nb = re.sub(r'^(\s*)', r'\1'+f'{rtype} {rname} = {init};\n    ', nb, count=1)
        else:
            # casts of the receiver var -> this / (T)this
            nb=re.sub(r'\(\(\s*StaffRoll\s*\*\s*\)\s*'+re.escape(rname)+r'\)', 'this', nb)
            nb=re.sub(r'\(\s*(?:C|C37|C2dd4|StaffRoll)\s*\*\s*\)\s*'+re.escape(rname)+r'\b', 'this', nb)
            nb=re.sub(r'\(\s*(char|int|u8|unsigned char|void)\s*\*\s*\)\s*'+re.escape(rname)+r'\b', r'(\1*)this', nb)
            # bare uses -> typed this
            rep='this' if rtype in ('StaffRoll*','StaffRoll *','void*','void *','C*','C *','C37*','C37 *','C2dd4*','C2dd4 *') else f'({rtype})this'
            nb=re.sub(r'\b'+re.escape(rname)+r'\b', rep, nb)
    restsig=', '.join(d['rest'])
    newdef=f'{d["ret"]} StaffRoll::{name}({restsig}) {{'+nb+'}'
    pieces.append((d['start'],d['end'],newdef,name))

# ---------- build spans on the ORIGINAL text ----------
spans=[]
unstripped=[]
for start,end,newtext,name in pieces:
    dline=start+1                              # def's own line (start is the '\n' before it)
    span_begin=dline
    span_end=end+1                             # just past the body's closing '}'
    # scan upward over preservable lines (comments, @symbol, single extern decls)
    # looking for an 'extern "C" {' opener; stop at anything else.
    pls=dline; found=False
    for _ in range(12):
        prev=t.rfind('\n',0,pls-1)+1
        if prev==pls: break                    # BOF
        eol=t.index('\n',prev)
        pline=t[prev:eol+1]
        if re.match(r'\s*extern "C" \{\s*(/\*[^\n]*\*/)?\s*$', pline): pls=prev; found=True; break
        if re.match(r'\s*(//[^\n]*|/\*[^\n]*\*/)?\s*$', pline): pls=prev; continue
        if re.match(r'\s*extern\s+(?!"C")[^;{]*;\s*$', pline): pls=prev; continue
        break
    if found:
        # preserve mid lines (@symbol, extern decls) between opener and def
        mid=t[t.index('\n',pls)+1:dline]
        # the wrapper's '}': skip whitespace and #undef/#define lines after the body
        j=end+1
        while True:
            while j<len(t) and t[j] in ' \t\n': j+=1
            if j<len(t) and t[j]=='#':
                j=t.index('\n',j)+1; continue
            break
        if j<len(t) and t[j]=='}':
            span_begin=pls
            newtext=mid+newtext+t[end+1:j]      # keep #undef etc. between body and closer
            span_end=j+1
            if t[span_end:span_end+1]=='\n': span_end+=1
        else:
            unstripped.append(name)
    spans.append((span_begin,span_end,newtext))
if unstripped: print('WARNING extern "C" { kept:',unstripped)

# sanity: spans must not overlap
spans.sort()
for i in range(1,len(spans)):
    assert spans[i][0]>=spans[i-1][1], f'overlap at {spans[i][0]}'

out=[]; pos=0
for b,e,nt in spans:
    out.append(t[pos:b]); out.append(nt); pos=e
out.append(t[pos:])
t=''.join(out)

# ---------- extern decls of converted functions -> delete (before call pass!) ----------
t=re.sub(r'\nextern (?:int|void|short) (func_ov002_\w+)\([^)]*\);', lambda m: '' if m.group(1) in conv else m.group(0), t)

# ---------- global call-site pass (member defs & other bodies now see member names) ----------
t=transform_calls(t, None)

# ---------- W records: (int)func -> (int)_ZN... ----------
for name,d in info.items():
    t=t.replace(f'(int){name}', f'(int){d["zname"]}')

# ---------- @symbol comments ----------
for name,d in info.items():
    t=t.replace(f'// @symbol {name}', f'// @symbol {d["zname"]}')

# ---------- shadow structs + typedefs ----------
t=t.replace('''/* shadow struct 'C' */
struct C;

/* shadow typedef 'void' */
typedef void (C::*PMF)(int);

/* pointer-to-member taking no args (dispatch tables at 02110f34/02110f9c) */
typedef void (C::*PMF0)();''',
'''typedef void (StaffRoll::*PMF)(int);

/* pointer-to-member taking no args (dispatch tables at 02110f34/02110f9c) */
typedef void (StaffRoll::*PMF0)();''')
t=re.sub(r'/\* dispatch receiver of func_ov002_020f37a0.*?\*/\s*struct C37 \{.*?\};\s*typedef void \(C37::\*PMF37\)\(int\);\s*struct Entry37 \{ PMF37 pmf; \};',
         'typedef void (StaffRoll::*PMF37)(int);\nstruct Entry37 { PMF37 pmf; };', t, flags=re.S)
t=re.sub(r'/\* dispatch receiver of func_ov002_020f2dd4.*?\*/\s*struct C2dd4 \{.*?\};\s*typedef void \(C2dd4::\*PMF2dd4\)\(int\);\s*struct Entry2dd4 \{ PMF2dd4 pmf; \};',
         'typedef void (StaffRoll::*PMF2dd4)(int);\nstruct Entry2dd4 { PMF2dd4 pmf; };', t, flags=re.S)

# ---------- _ZN extern decls for the W records (descriptor addresses) ----------
# insert before the W record block
zn=sorted(set(d['zname'] for d in info.values()))
# build decl: extern ret zname(StaffRoll*, rest...)
decl_lines=[]
def type_only(p):
    # 'unsigned char *p' -> 'unsigned char *', 'int idx' -> 'int'
    p=p.strip()
    m=re.match(r'^(.*[\*\s])([A-Za-z_]\w*)$',p)
    return (m.group(1).rstrip() if m else p)
for name,d in sorted(info.items(), key=lambda kv: kv[1]['zname']):
    ps=', '.join(['StaffRoll *']+[type_only(x) for x in d['rest']])
    decl_lines.append(f'extern {d["ret"]} {d["zname"]}({ps}); /* local extern: descriptor address */')
# place them just before the first W record definition
anchor='W data_ov002_'
i=t.find(anchor)
assert i>0
# find a comment line to attach: insert right before the first 'W data_' line start
ls=t.rfind('\n',0,i)+1
t=t[:ls]+'\n'.join(decl_lines)+'\n'+t[ls:]

open('/tmp/stage1.cpp','w',encoding='utf-8').write(t)
print('rewrote',len(pieces),'defs; zn externs:',len(decl_lines))
open('/tmp/znames.txt','w').write('\n'.join(f'{n} {d["zname"]}' for n,d in sorted(info.items())))

# ---------- header decls ----------
hdr=open('include/daDemo_c.h',encoding='utf-8').read()
anchor='        /* Begin credit line n (1..0x14) in the first credit slot. */\n        void StartCredit(int);'
if anchor in hdr and 'func_ov002_020f5d34' not in hdr:
    decls=[]
    for name,d in sorted(info.items(), key=lambda kv: kv[1]['start']):
        restsig=', '.join(d['rest'])
        decls.append(f'        {d["ret"]} {name}({restsig});')
    hdr=hdr.replace(anchor, anchor+'\n'+'\n'.join(decls),1)
    open('include/daDemo_c.h','w',encoding='utf-8').write(hdr)
    print('header decls added:',len(decls))
else:
    print('header already updated')

# ---------- symbols.txt ----------
sy=open('config/arm9/overlays/ov002/symbols.txt',encoding='utf-8').read()
for name,d in info.items():
    sy=re.sub(r'^'+re.escape(name)+r'( kind:)', d['zname']+r'\1', sy, flags=re.M)
open('config/arm9/overlays/ov002/symbols.txt','w',encoding='utf-8').write(sy)
print('symbols.txt updated')

# ---------- manifest ----------
mp='config/tu_manifest.d/ov002/daDemo_c.json'
mj=open(mp,encoding='utf-8').read()
for name,d in info.items():
    mj=mj.replace(f'"symbol": "{name}"', f'"symbol": "{d["zname"]}"')
open(mp,'w',encoding='utf-8').write(mj)
print('manifest updated')
