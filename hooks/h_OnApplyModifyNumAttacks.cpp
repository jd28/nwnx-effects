#include "NWNXEffects.h"

extern CNWNXEffects effects;

int Hook_OnApplyModifyNumAttacks(CNWSEffectListHandler *ai, CNWSObject *obj, CGameEffect *eff, int a)
{
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
