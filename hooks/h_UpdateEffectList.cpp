#include "NWNXEffects.h"

extern CNWNXEffects effects;

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
    CGameEffect *e = list->Array[idx];

    if (e->Type == EFFECT_TRUETYPE_MODIFYNUMATTACKS && e->GetInteger(0) == 0) {
        CWorldTimer *wt = g_pAppManager->
                          ServerExoApp->
                          GetActiveTimer(currentObject->ObjectID);

        unsigned int desiredTickInterval = e->GetInteger(1);

        if (desiredTickInterval > 0) {
            int lastWorldTimer1 = e->GetInteger(2);
            if (lastWorldTimer1 == 0) {
                lastWorldTimer1 = currentWorldTimer1;
                e->SetInteger(2, currentWorldTimer1);
            }
            int lastWorldTimer2 = e->GetInteger(3);
            if (lastWorldTimer2 == 0) {
                lastWorldTimer2 = currentWorldTimer2;
                e->SetInteger(3, currentWorldTimer2);
            }

            long unsigned int out1, out2;
            int ret = wt->SubtractWorldTimes(currentWorldTimer1, currentWorldTimer2,
                                             lastWorldTimer1, lastWorldTimer2, &out1, &out2);

            if (!ret && (out1 > 0 || out2 > desiredTickInterval * 1000)) {
                effects.currentEffect = e;

                CoreRunScriptService sv = {
                    currentObject->ObjectID,
                    "on_ceff_tick",
                    0
                };
                CallService(SERVICE_CORE_RUNSCRIPT, (uintptr_t) &sv);

                effects.currentEffect = NULL;

                e->SetInteger(2, currentWorldTimer1);
                e->SetInteger(3, currentWorldTimer2);
            }
        }
    }

    return &list->Array[idx];
}

void HookCustomEffectUpdate()
{
    NX_HOOK(CNWSObject__UpdateEffectList, 0x081CF014,
            Hook_CNWSObject__UpdateEffectList, 5);

    HookCall(0x081CF090, (DWORD) CExoArrayList_Eff__vc_Mid);
}

