//cpp
// @symbol _ZN9PushBlock15OnHitByMegaCharER6Player
/* recovered: renamed to Class_Method
 *
 * PushBlock::OnHitByMegaChar(Player&) -- vtable slot 27, ov002 0x020b8d14.
 * Attributed by the vtable: _ZTV9PushBlock carries this address at slot 27 --
 * vtable + 0x6c -- where _ZTV10dBgActor_c carries dActor_c's generic
 * 0x02010124. include/dActor_c.h's own slot 27 declaration supplies the
 * signature, `virtual void OnHitByMegaChar(Player &player)`. Confirmed with
 * tools/mangle.py: _ZN9PushBlock15OnHitByMegaCharER6Player.
 *
 * Kept as an extern "C" free function under the literal mangled name -- the
 * body is the same virtual-dispatch-through-a-shadow-vtable trick the file
 * used before the rename (Player::IncMegaKillCount, then this class's own
 * Kill at slot 31 through an unqualified virtual call), unconverted. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19(); virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27(); virtual void v28(); virtual void v29(); virtual void v30(); virtual void m(); };
extern "C" void _ZN6Player16IncMegaKillCountEv(int);
extern "C" void _ZN9PushBlock15OnHitByMegaCharER6Player(Base *c, int a) { _ZN6Player16IncMegaKillCountEv(a); c->m(); }
