#ifndef SM64DS_MATH_FIX12_H
#define SM64DS_MATH_FIX12_H

/* The game's 20.12 fixed-point class template, reconstructed from its own
 * mangled symbols: 5Fix12IiE in e.g. ShadowModel::InitModel and
 * Player::SetAnim demangles to Fix12<int>, and the S3_ substitutions for
 * repeated parameters only fall out of a real template-id -- a plain int
 * parameter mangles as i and can never reproduce those names.
 *
 * include/types.h reserves this name for exactly this reconstruction; the
 * scalar spelling stays Fix12i.
 *
 * Layout is a single T (the raw 20.12 bits): every matched function that
 * passes one does so in a single register, bit-identical to an int.
 * Operators get added here as converted code needs them, not before.
 */

#ifdef __cplusplus

template <typename T>
struct Fix12 {
    T val;
};

typedef char Fix12i_size_must_be_4[sizeof(Fix12<int>) == 4 ? 1 : -1];

#endif /* __cplusplus */

#endif /* SM64DS_MATH_FIX12_H */
