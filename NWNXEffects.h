#pragma once

#include "NWNXBase.h"
#include "include/nwn_const.h"
#include "../../api/all.h"
#include "../../include/nx_hook.h"
#include "pluginlink.h"
#include "core/pluginlink.h"

#include "hooking.c"

/**
 * Internal layout of ModifyNumAttacks has required fields at the front.
 * User-accessible props start at X - see README.md "Internals".
 * Don't change this. :)
 */
#define CUSTOM_EFFECT_PROPERTIES_START_AT 4

void HookCustomEffectUpdate();
void HookModifyNumAttacks();

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
