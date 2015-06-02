#include "NWNXEffects.h"

extern CNWNXEffects effects;

int Hook_OnRemoveModifyNumAttacks(CNWSEffectListHandler *ai, CNWSObject *obj, CGameEffect *eff)
{
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
