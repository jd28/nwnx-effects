#include "NWNXEffects.h"

extern CNWNXEffects effects;

static int (*OnApplyModifyNumAttacks)(CNWSEffectListHandler *ai,
                                      CNWSObject *obj, CGameEffect *eff, int a);
static int (*OnRemoveModifyNumAttacks)(CNWSEffectListHandler *ai,
                                       CNWSObject *obj, CGameEffect *eff);

static int Hook_OnApplyModifyNumAttacks(CNWSEffectListHandler *ai,
                                        CNWSObject *obj, CGameEffect *eff, int a)
{
    if (eff->GetInteger(0) != 0)
        return OnApplyModifyNumAttacks(ai, obj, eff, a);

    EffectsCustomApplyEvent ev = {
        obj, eff, false
    };

    // Log(3, "Custom Effect Event: Object: %x, Effect: %d, Removal: %d\n",
    //     obj->ObjectID, eff->IntList[1], removal);

    if (NotifyEventHooks(effects.hCustomApply, (uintptr_t) &ev))
        return ev.failed;

    effects.currentEffect = eff;
    effects.currentResult = false;

    CoreRunScriptService sv = {
        obj->ObjectID,
        "on_ceff_apply",
        0
    };
    CallService(SERVICE_CORE_RUNSCRIPT, (uintptr_t) &sv);

    effects.currentEffect = NULL;

    return effects.currentResult;
}

static int Hook_OnRemoveModifyNumAttacks(CNWSEffectListHandler *ai,
        CNWSObject *obj, CGameEffect *eff)
{
    if (eff->GetInteger(0) != 0)
        return OnRemoveModifyNumAttacks(ai, obj, eff);

    EffectsCustomRemoveEvent ev = {
        obj, eff
    };

    // Log(3, "Custom Effect Event: Object: %x, Effect: %d, Removal: %d\n",
    //     obj->ObjectID, eff->IntList[1], removal);

    if (NotifyEventHooks(effects.hCustomRemove, (uintptr_t) &ev))
        return 1;

    effects.currentEffect = eff;

    CoreRunScriptService sv = {
        obj->ObjectID,
        "on_ceff_remove",
        0
    };
    CallService(SERVICE_CORE_RUNSCRIPT, (uintptr_t) &sv);

    effects.currentEffect = NULL;

    return 1;
}

void HookModifyNumAttacks()
{
    NX_HOOK(OnApplyModifyNumAttacks, 0x0817D22C,
            Hook_OnApplyModifyNumAttacks, 5);

    NX_HOOK(OnRemoveModifyNumAttacks, 0x08177D00,
            Hook_OnRemoveModifyNumAttacks, 5);
}
