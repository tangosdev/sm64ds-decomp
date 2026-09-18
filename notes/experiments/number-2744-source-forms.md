# Number source-form experiments, PR #2744

Producer evidence for `NUMBER-2744-05`, measured on 2026-09-18. This is a bounded
set of compiler experiments, not a proof that simpler source cannot match.
Independent Source review and current-main composition remain required.

Input: `5add4b136b53f98b1fa040adec1db9b9cb8dc43f`. Comparison snapshot: `4e844d388f0d37e688ebf9899a7d532dff9d7146`.
The final control is the source and header in this commit, after the three
contract corrections. Source SHA-256: `763981f98c3dfc2c1b61ee4ce9f3a36db3dd28190761955aa98f60bd305077b5`;
header SHA-256: `92df30c6d1acb1928b9ae932a1191cf834ff82007eb5d50a74d2abe4ee2db0e7` (UTF-8 files with LF line endings).
Compiler: `tools/mwccarm/2004/b56/mwccarm.exe`; flags:
`-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`.
The recipe uses `swarm.CPP_FLAGS`, which additionally passes `-w illpragmas`.
Independent review of `39de013c631077cbc8023bfff46fdfae013fba3a` reproduced all
six complete objects with and without that warning suppression and found them identical.

## Adopted ordinary forms

The source uses `mVertSpeed += mVertAccel`, `mPosY += mVertSpeed` and `++mState`.
It replaces the local NumberPos shadow with real Vector3, uses `Vector3 mStartPos`
at 0x13c, reads `pos.z` normally, and passes `&mModel.mat4x3` directly. Both volatile
reads and every integer-pointer scheduling cast are gone. Render uses the existing
`PowerStar::unk_43c` member, and SpawnNumber uses the real popup delay member.
All eight Number functions retain their 1340 bytes and exact relocation targets;
the 152-byte SpawnNumber caller also verifies. Initial isolated and combined
probe controls are retained locally under `build/number-probes/` in `results.json`,
`second-results.json`, `third-results.json` and `fourth-results.json`.

Vector3 emits `_ZN7Vector3D1Ev`, an existing four-byte weak destructor. It is
explicitly licensed as `deadstrip-duplicate` at arm9:0x020072c0; its emitted body
`1eff2fe1` verifies with zero blind/differing relocation slots. The ROM build
enforces byte equality before discarding the duplicate. This is neither a new
Number function nor newly matched credit. The eight-function enrollment is unchanged.

## Measured remaining alternatives

The table is from a fresh pass against the cleaned final source and header.
Every non-control variant leaves the other seven Number functions verified.
`NO-SYM` is the checker result when the emitted function is shorter than the
requested ROM span; each such candidate compiled and its size is recorded.

| Case | Exact change | Target | Emitted | Result |
| --- | --- | ---: | ---: | --- |
| final_control | This commit's source | 1340 total | 1340 total | 8/8 VERIFIED, zero blind/diff slots |
| final_particle_scalar | Vector3-view Y argument to `mPosY` | Behavior 372 | 368 | NO-SYM |
| final_owner_scalars | `op->x/y/z` to `other->mPosX/Y/Z`, remove view | Behavior 372 | 368 | NO-SYM |
| final_position_reference | Name the actor Vector3 reference at function entry; use `position.y` for the particle | Behavior 372 | 376 | WRONG |
| final_returned_files | Save LoadFile results and use them for Prepare/SetFile | InitResources 444 | 436 | NO-SYM |
| final_SetFile_member | Existing member SetFile with local `Fix12<int> speed = {0}` | InitResources 444 | 468 | WRONG |

The last probe is not a proposed contract fix: the existing shared member still
says u32 for the final argument, while its actual implementation says u16.
The retained Number scalar bridge agrees with the definition. A shared-header
repair needs separately reserved consumers. Particle::System currently has no
NewSimple member declaration; the local bridge now correctly returns void*.
No member-call impossibility is claimed for that untested shared reconstruction.

The two position views remain a source compromise because dActor_c still exposes
three scalar members. SharedFilePtr+4 reads remain because its header exposes no
loaded-file field. PowerStar's halfword at 0x496 remains inside unrecovered padding;
the named 0x43c field is already used. These are the remaining reconstruction
findings in this partial repair. [Issue #2769](https://github.com/tangosdev/sm64ds-decomp/issues/2769)
records the bounded continuation, with `codex-integrator-0918` as next owner.
The proposed deferral still requires independent acceptance; the issue alone
does not establish it.

## Reproduction

In a wired checkout of this commit, save the following block as
`build/number-probes/reproduce.py` and run
`python build/number-probes/reproduce.py`. It only writes build artifacts.
It records exact source/object hashes, function sizes and strict relocated-byte
verdicts in `build/number-probes/final-probes.json`. No source/header changes are
needed. The initial control must verify all eight functions before interpreting
the alternatives.

```python
import hashlib, json, pathlib, sys
R=pathlib.Path.cwd(); O=R/'build/number-probes'; sys.path.insert(0,str(R/'tools'))
import match, swarm, objisolate, linkcheck, reloc_audit
s=(R/'src/game/actors/d_a_obj_number.cpp').read_text(encoding='utf-8')
manifest=json.loads((R/'config/tu_manifest.d/ov002/daObjNumber_c.json').read_text(encoding='utf-8'))
index=reloc_audit.build_name_index(); rows=[]
variants={
 'final_control': s,
 'final_particle_scalar': s.replace('reinterpret_cast<const Vector3 *>(&mPosX)->y', 'mPosY'),
 'final_owner_scalars': s.replace('            const Vector3 *op = reinterpret_cast<const Vector3 *>(&other->mPosX);\n', '').replace('op->x','other->mPosX').replace('op->y','other->mPosY').replace('op->z','other->mPosZ'),
 'final_position_reference': s.replace('    Vector3 pos;', '    const Vector3 &position = *reinterpret_cast<const Vector3 *>(&mPosX);\n    Vector3 pos;').replace('reinterpret_cast<const Vector3 *>(&mPosX)->y', 'position.y'),
}
t=s
for anim,model in [('0210da08','0210da28'),('0210d9e8','0210d9a8')]:
 t=t.replace('TextureSequence::LoadFile(data_ov002_'+anim+');','BTP_File *anim = static_cast<BTP_File *>(TextureSequence::LoadFile(data_ov002_'+anim+'));')
 t=t.replace('void *m = Model::LoadFile(data_ov002_'+model+');','BMD_File *m = static_cast<BMD_File *>(Model::LoadFile(data_ov002_'+model+'));')
 t=t.replace('**(BMD_File **)((char *)&data_ov002_'+model+' + 4)','*m').replace('**(BTP_File **)((char *)&data_ov002_'+anim+' + 4)','*anim').replace('*(BTP_File **)((char *)&data_ov002_'+anim+' + 4)','anim')
variants['final_returned_files']=t
t=s
for anim in ['0210da08','0210d9e8']:
 old='''        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence,
            *(BTP_File **)((char *)&data_ov002_'''+anim+''' + 4),
            0x40000000, 0, 0);'''
 new='''        Fix12<int> speed = {0};
        mTextureSequence.SetFile(**(BTP_File **)((char *)&data_ov002_'''+anim+''' + 4),
                                 0x40000000, speed, 0);'''
 assert old in t
 t=t.replace(old,new)
variants['final_SetFile_member']=t
for name,text in variants.items():
 p=O/(name+'.cpp'); p.write_text(text,encoding='utf-8')
 obj=match.compile_c(p,'2004/b56',swarm.CPP_FLAGS); assert obj
 (O/(name+'.o')).write_bytes(obj)
 row=dict(case=name,source_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),object_sha256=hashlib.sha256(obj).hexdigest(),functions=[])
 for f in manifest['functions']:
  sym=f['symbol']; code,_=match.extract_func(obj,sym); isolated,_=objisolate.derive(obj,sym)
  res=linkcheck.linkcheck(sym,int(f['address'],16),int(f['size'],16),'ov002',index,obj=isolated,sym=sym)
  row['functions'].append(dict(symbol=sym,size=len(code),target_size=int(f['size'],16),strict=res))
 row['matched']=sum(f['strict']['verdict']=='VERIFIED' and f['strict']['blind']==0 for f in row['functions'])
 rows.append(row)
 print(name,row['matched'],[(f['symbol'],f['size'],f['strict']['verdict']) for f in row['functions'] if f['strict']['verdict']!='VERIFIED'],flush=True)
(O/'final-probes.json').write_text(json.dumps(dict(compiler='2004/b56',flags=swarm.CPP_FLAGS,cases=rows),indent=2)+'\n',encoding='utf-8')
```

## Historical evidence

The pre-promotion [Number manifest](https://github.com/tangosdev/sm64ds-decomp/blob/4e844d388f0d37e688ebf9899a7d532dff9d7146/config/tu_manifest.d/ov002/Number.json)
is the immutable snapshot for the original legacy paths, old boundary claim and
failed 7/8 partial-isolation experiment. It does not identify the original
measurement commit, so the repaired manifest records that value as unknown.
The old observations remain unchanged under `historical_evidence`; they are not
current proof. Current TU verification is separate.

The restored Number entries in `notes/data/tu-merge-candidates.json`,
`notes/data/c-cpp-classification.tsv`, `notes/plan-tu-merge-queue.md`,
`symbols/actor_renames_report.txt` and `notes/cpp-conversion-enemies.md` describe
their original snapshots. In particular the old D0 `.c` census row is historical,
not a claim about the immediate pre-promotion `.cpp` predecessor. Current mapping:
old Number is RTTI-backed daObjNumber_c, now one promoted source with eight
functions (six class method entries including D1/D0, one utility and one factory).
No converted counter or symbol attribution is changed by this repair.

Fresh tu_map reports seven class-labelled entries followed by the adjacent
anonymous factory, with medium boundary confidence. Manual factory membership
and fresh byte proof justify the eight-function manifest; the old high-confidence
eight-function heuristic is preserved only as history.
