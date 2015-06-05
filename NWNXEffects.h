#pragma once

#include "NWNXBase.h"
#include "include/nwn_const.h"
#include "../../api/all.h"
#include "../../include/nx_hook.h"
#include "pluginlink.h"
#include "core/pluginlink.h"
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

/**
 * Internal layout of ModifyNumAttacks has required fields at the front.
 * User-accessible props start at X - see README.md "Internals".
 * Don't change this. :)
 */
#define CUSTOM_EFFECT_PROPERTIES_START_AT 0

void HookCustomEffectUpdate();
void HookGetScriptEffectType();
void HookEffectHandlers();
void HookExecuteCommandApplyEffectOnObject();

/**
 * A struct that describes attached data to effects. It's used to carry
 * metadata around instead of using native Integers, so we don't clobber
 * existing ints in native CGameEffects.
 *
 * Lifetime is restricted to the Apply/Remove cycle, under the assumption that
 * all effects will be removed at some point.
 */
struct AttachedEffectData {
	// Effect tick rate in seconds.
    uint32_t tickRate;

    // Used internally to keep track of when the last tick happened.
    uint32_t worldTimerBig;
    uint32_t worldTimerLittle;
};

typedef uint32_t nweffectid;

class CNWNXEffects: public CNWNXBase
{
public:
    CNWNXEffects();

    bool OnCreate(gline *nwnxConfig, const char *LogDir = NULL);
    char *OnRequest(char *gameObject, char *Request, char *Parameters);
    unsigned long OnRequestObject(char *gameObject, char *Request);

    HANDLE hCustomApply, hCustomRemove;

    bool CallEffectHandler(const char* handler, CNWSObject* obj, CGameEffect *eff);

    /**
     * This removes any local state about the given effect.
     */
    void CleanupEffect(nweffectid id);

    /**
     * Returns the AttachedEffectData associated with the given effect, or NULL if
     * none has been assigned.
     */
    AttachedEffectData *GetAttachedEffectData(nweffectid id);

    /**
     * Built-in effect truetypes that trigger a additional on-apply/remove.
     */
    std::vector<uint32_t> EffectsWithEffectHandlers;

private:
    CGameEffect *currentEffect;
    bool currentResult;

    std::unordered_map<nweffectid, AttachedEffectData*> EffectsAttachedEffectData;
};
