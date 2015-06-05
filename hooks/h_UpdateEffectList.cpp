#include "NWNXEffects.h"

/**
 * This hooks into CNWSObject::UpdateEffectsList (which is actually called
 * by ::AIUpdate). We're using it to run tick scripts for custom effects and
 * those enabled explicitly.
 */

extern CNWNXEffects effects;

static void TickEffect(CNWSObject* owner, CGameEffect *eff,
                       AttachedEffectData *tickData,
                       unsigned int hr, unsigned int sec)
{
    // No tickdata set or no ticks requested.
    if (!tickData || tickData->tickRate <= 0)
        return;

    printf("tickData: %d\n", tickData->tickRate);

    CWorldTimer *wt = g_pAppManager->
                      ServerExoApp->
                      GetActiveTimer(owner->ObjectID);

    if (tickData->worldTimerBig == 0)
        tickData->worldTimerBig = hr;
    if (tickData->worldTimerLittle == 0)
        tickData->worldTimerLittle = sec;

    long unsigned int out1, out2;
    int ret = wt->SubtractWorldTimes(
                  hr, sec,
                  tickData->worldTimerBig, tickData->worldTimerLittle,
                  &out1, &out2);

    if (!ret && (out1 > 0 || out2 > tickData->tickRate * 1000)) {

        effects.CallEffectHandler(CUSTOM_EFFECT_SCRIPT_TICK, owner, eff);

        tickData->worldTimerBig = hr;
        tickData->worldTimerLittle = sec;
    }
}

static CNWSObject *currentObject;
static unsigned int currentWorldTimer1,
       currentWorldTimer2;

static CNWSObject* (*CNWSObject__UpdateEffectList)(CNWSObject *obj,
        unsigned int a2, unsigned int a3);

static CNWSObject* Hook_CNWSObject__UpdateEffectList(CNWSObject *obj,
        unsigned int a2, unsigned int a3)
{
    currentObject = obj;
    currentWorldTimer1 = a2;
    currentWorldTimer2 = a3;

    return CNWSObject__UpdateEffectList(obj, a2, a3);
}

static CGameEffect** CExoArrayList_Eff__vc_Mid(CExoArrayList<CGameEffect*> *list, int idx)
{
    CGameEffect *eff = list->Array[idx];

    if (eff->Type >= EFFECT_TRUETYPE_CUSTOM) {
        AttachedEffectData *tickData = effects.GetAttachedEffectData(eff->Id);

        if (tickData != NULL && tickData->tickRate > 0)
            TickEffect(currentObject, eff,
                       tickData,
                       currentWorldTimer1, currentWorldTimer2);
    }

    return &list->Array[idx];
}

void HookCustomEffectUpdate()
{
    NX_HOOK(CNWSObject__UpdateEffectList, 0x081CF014,
            Hook_CNWSObject__UpdateEffectList, 5);

    HookCall(0x081CF090, (DWORD) CExoArrayList_Eff__vc_Mid);
}

