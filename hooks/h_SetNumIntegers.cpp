#include "NWNXEffects.h"

/**
 * By default, effect have 8 integers. The most any native effect ever uses
 * is 20.
 * We ensure that we have at least 23 at all times, since we are using
 * those above 20 for our own nefarious(reserved) purpose.
 */

static void (*SetNumIntegersInitializeToNegativeOne)(CGameEffect*, int);
static void (*SetNumIntegers)(CGameEffect*, int);

static void SetNumIntegersInitializeToNegativeOne_Hook(CGameEffect* e, int count)
{
    return SetNumIntegersInitializeToNegativeOne(
               e, count >= EFFECT_MIN_INTEGERS ? count : EFFECT_MIN_INTEGERS);
}

static void SetNumIntegers_Hook(CGameEffect* e, int count)
{
    return SetNumIntegers(
               e, count >= EFFECT_MIN_INTEGERS ? count : EFFECT_MIN_INTEGERS);
}


void HookEffectSetNumIntegers()
{
    NX_HOOK(SetNumIntegers, 0x0817f54c, SetNumIntegers_Hook, 5);

    NX_HOOK(SetNumIntegersInitializeToNegativeOne, 0x0817f5a0,
            SetNumIntegersInitializeToNegativeOne_Hook, 5);
}

