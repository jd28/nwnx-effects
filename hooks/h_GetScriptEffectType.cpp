#include "NWNXEffects.h"

/**
 * This hooks CGameEffect::GetScriptEffectType to allow custom effects
 * get script IDs.
 *
 * We simply map the TRUETYPE to a script ID of the same value, so
 * aggregating multiple script IDs under the same truetype isn't
 * supported right now.
 */

extern CNWNXEffects effects;

static int (*CGameEffect__GetScriptEffectType)(CGameEffect* eff);

static int CGameEffect__GetScriptEffectType_Hook(CGameEffect* eff)
{
    if (eff->Type >= EFFECT_TRUETYPE_CUSTOM)
        return eff->Type;
    else
        return CGameEffect__GetScriptEffectType(eff);
}

void HookGetScriptEffectType()
{
    NX_HOOK(CGameEffect__GetScriptEffectType, 0x0817EFB8,
            CGameEffect__GetScriptEffectType_Hook, 6);

}
