#pragma once

#include "NWNXBase.h"
#include "include/nwn_const.h"
#include "../../api/all.h"
#include "../../include/nx_hook.h"
#include "pluginlink.h"
#include "core/pluginlink.h"

#include <assert.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "hooking.c"

/**
 * Custom effects start here. You can assign any ID as you see fit,
 * as long as it is equal or larger to this.
 *
 * 95 is EFFECT_TRUETYPE_DEFENSIVESTANCE as of this writing (patch 1.69).
 * You do not have to change this unless you are already implementing
 * your own custom effect truetypes, in which case behaviour is undefined.
 */
#define EFFECT_TRUETYPE_CUSTOM 96

#define CUSTOM_EFFECT_SCRIPT_APPLY    "on_ceff_apply"
#define CUSTOM_EFFECT_SCRIPT_TICK     "on_ceff_tick"
#define CUSTOM_EFFECT_SCRIPT_REMOVE   "on_ceff_remove"

enum CustomEffect {
    CUSTOM_EFFECT_APPLY,
    CUSTOM_EFFECT_REMOVE,
    CUSTOM_EFFECT_TICK
};

#define CUSTOM_IP_SCRIPT_APPLY    "on_ip_apply"
#define CUSTOM_IP_SCRIPT_REMOVE   "on_ip_remove"

void HookCustomEffectUpdate();
void HookGetScriptEffectType();
void HookEffectHandlers();
void HookExecuteCommandApplyEffectOnObject();
void HookEffectSetNumIntegers();
void HookItempropHandlers();

#define EFFECT_MIN_INTEGERS        23

#define EFFECT_INT_TICKRATE        20
#define EFFECT_INT_WORLTIMER_DAY   21
#define EFFECT_INT_WORLDTIMER_TIME 22

#define GetEffectTickRate(e)       (e->IntList[EFFECT_INT_TICKRATE])
#define GetEffectLastTickDay(e)    (e->IntList[EFFECT_INT_WORLTIMER_DAY])
#define GetEffectLastTickTime(e)   (e->IntList[EFFECT_INT_WORLDTIMER_TIME])

#define SetEffectTickRate(e,v)     (e->IntList[EFFECT_INT_TICKRATE] = v)
#define SetEffectLastTickDay(e,v)  (e->IntList[EFFECT_INT_WORLTIMER_DAY] = v)
#define SetEffectLastTickTime(e,v) (e->IntList[EFFECT_INT_WORLDTIMER_TIME] = v)

typedef long unsigned int nweffectid;

class CNWNXEffects: public CNWNXBase
{
public:
    CNWNXEffects();

    bool OnCreate(gline *nwnxConfig, const char *LogDir = NULL);
    char *OnRequest(char *gameObject, char *Request, char *Parameters);
    unsigned long OnRequestObject(char *gameObject, char *Request);
    bool in_script = false;

    HANDLE hCustom;
    HANDLE hItemprop;
    int32_t forceApply = 0;

    int CallEffectHandler(CNWSObject* obj, CGameEffect *eff, CustomEffect type);
    bool ItempropEvent(CNWSCreature *obj, CNWSItem *item, CNWItemProperty *ip, bool removal, uint32_t slot);

    /**
     * Built-in effect truetypes that trigger a additional on-apply/remove.
     */
    std::vector<uint16_t> NativeEffectsWithHandlers;

private:
    EffectsCustomEvent effectEvent;
    EffectsItempropEvent itempropEvent;
};
