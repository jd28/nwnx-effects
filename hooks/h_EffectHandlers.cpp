#include "NWNXEffects.h"

/**
 * This hooks Apply and Remove effect handlers for all effects.
 *
 * We're using it to call back into nwscript for any effects that
 * should have user events - like custom effects, or effects we
 * explicitly enable it for (CNWXEffects::NativeEffectsWithHandlers).
 */

extern CNWNXEffects effects;

static int (*CServerAIMaster__OnEffectApplied)(CServerAIMaster *ai, CNWSObject *obj,
        CGameEffect *eff, int a4);
static int (*CServerAIMaster__OnEffectRemoved)(CServerAIMaster *ai, CNWSObject *obj,
        CGameEffect *eff);

static int CServerAIMaster__OnEffectApplied_Hook(CServerAIMaster *ai, CNWSObject *obj,
        CGameEffect *eff, int a4)

{
    // One of our custom effects: Always call our own handler.
    if (eff->Type >= EFFECT_TRUETYPE_CUSTOM) {
        int ret = effects.CallEffectHandler(obj, eff, CUSTOM_EFFECT_APPLY);
        return ret;
    }

    // Otherwise: a default bioware effect.
    int ret = CServerAIMaster__OnEffectApplied(ai, obj, eff, a4);

    if (ret) {
        return ret;
    }

    // Default effects canno be stopped from applying for stability reasons,
    // but we will still call our custom effect handler if the user wishes us to.
    if (std::find(effects.NativeEffectsWithHandlers.begin(),
                  effects.NativeEffectsWithHandlers.end(), eff->Type) !=
            effects.NativeEffectsWithHandlers.end())
        effects.CallEffectHandler(obj, eff, CUSTOM_EFFECT_APPLY);

    return 0;
}

static int CServerAIMaster__OnEffectRemoved_Hook(CServerAIMaster *ai, CNWSObject *obj,
        CGameEffect *eff)
{
    if (eff->Type >= EFFECT_TRUETYPE_CUSTOM) {
        int ret = effects.CallEffectHandler(obj, eff, CUSTOM_EFFECT_REMOVE);
        return 1;
    }

    // Otherwise: a default bioware effect. See if the user indicated
    // he wants to have his handler tickled.
    if (std::find(effects.NativeEffectsWithHandlers.begin(),
                  effects.NativeEffectsWithHandlers.end(), eff->Type) !=
            effects.NativeEffectsWithHandlers.end())
        effects.CallEffectHandler(obj, eff, CUSTOM_EFFECT_REMOVE);

    int ret = CServerAIMaster__OnEffectRemoved(ai, obj, eff);

    return ret;
}

void HookEffectHandlers()
{
    NX_HOOK(CServerAIMaster__OnEffectApplied, 0x080984E4,
            CServerAIMaster__OnEffectApplied_Hook, 6);

    NX_HOOK(CServerAIMaster__OnEffectRemoved, 0x08098508,
            CServerAIMaster__OnEffectRemoved_Hook, 6);
}
