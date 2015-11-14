#include "NWNXEffects.h"

/**
 * This hooks into CNWSObject::UpdateEffectsList (which is actually called
 * by ::AIUpdate). We're using it to run tick scripts for custom effects and
 * those enabled explicitly.
 */

extern CNWNXEffects effects;

static void TickEffect(CNWSObject* owner, CGameEffect *eff,
                       unsigned int day, unsigned int time)
{
    int32_t tickRate = GetEffectTickRate(eff);

    // No tickdata set or no ticks requested.
    if (tickRate <= 0)
        return;

    CWorldTimer *wt = g_pAppManager->
                      ServerExoApp->
                      GetActiveTimer(owner->ObjectID);

    if (GetEffectLastTickDay(eff) == 0)
        SetEffectLastTickDay(eff, day);
    if (GetEffectLastTickTime(eff) == 0)
        SetEffectLastTickTime(eff, time);

    long unsigned int out1, out2;
    int ret = wt->SubtractWorldTimes(
                  day, time,
                  GetEffectLastTickDay(eff), GetEffectLastTickTime(eff),
                  &out1, &out2);

    if (!ret && (out1 > 0 || out2 > (uint32_t)(tickRate * 1000))) {

        effects.CallEffectHandler(owner, eff, CUSTOM_EFFECT_TICK);

        SetEffectLastTickDay(eff, day);
        SetEffectLastTickTime(eff, time);
    }
}

static CNWSObject *currentObject;
static unsigned int currentWorldTimerDay,
       currentWorldTimerTime;

static CNWSObject* (*CNWSObject__UpdateEffectList)(CNWSObject *obj,
        unsigned int a2, unsigned int a3);

static CNWSObject* Hook_CNWSObject__UpdateEffectList(CNWSObject *obj,
        unsigned int a2, unsigned int a3)
{
    currentObject = obj;
    currentWorldTimerDay = a2;
    currentWorldTimerTime = a3;

    return CNWSObject__UpdateEffectList(obj, a2, a3);
}

static CGameEffect** CExoArrayList_Eff__vc_Mid(CExoArrayList<CGameEffect*> *list, int idx)
{
    CGameEffect *eff = list->Array[idx];

    if (GetEffectTickRate(eff) > 0)
        TickEffect(currentObject, eff,
                   currentWorldTimerDay, currentWorldTimerTime);

    return &list->Array[idx];
}

void HookCustomEffectUpdate()
{
    NX_HOOK(CNWSObject__UpdateEffectList, 0x081CF014,
            Hook_CNWSObject__UpdateEffectList, 5);

    HookCall(0x081CF090, (DWORD) CExoArrayList_Eff__vc_Mid);
}

