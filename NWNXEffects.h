#pragma once

#include "NWNXBase.h"
#include "include/nwn_const.h"
#include "../../api/all.h"
#include "../../include/nx_hook.h"
#include "pluginlink.h"
#include "core/pluginlink.h"

#include "hooking.c"

int Hook_OnApplyModifyNumAttacks(CNWSEffectListHandler *ai, CNWSObject *obj, CGameEffect *eff, int a);
int Hook_OnRemoveModifyNumAttacks(CNWSEffectListHandler *ai, CNWSObject *obj, CGameEffect *eff);

void HookCustomEffectUpdate();

class CNWNXEffects: public CNWNXBase
{
public:
    CNWNXEffects();

    bool OnCreate(gline *nwnxConfig, const char *LogDir = NULL);
    char *OnRequest(char *gameObject, char *Request, char *Parameters);
    unsigned long OnRequestObject(char *gameObject, char *Request);

    HANDLE hCustomApply, hCustomRemove;

    CGameEffect *currentEffect;
    bool currentResult;
};
