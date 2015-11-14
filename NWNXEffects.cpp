#include <sstream>
#include "NWNXEffects.h"

extern PLUGINLINK *pluginLink;

CNWNXEffects::CNWNXEffects()
{
    confKey = strdup("EFFECTS");
}

int CNWNXEffects::CallEffectHandler(CNWSObject* obj, CGameEffect *eff, CustomEffect type)
{
    effectEvent.effect = eff;
    effectEvent.object = obj;
    effectEvent.type   = type;

    if (!NotifyEventHooks(hCustom, (uintptr_t)&effectEvent)) {
        CExoString s;
        if (type == CUSTOM_EFFECT_REMOVE) {
            s = CUSTOM_EFFECT_SCRIPT_REMOVE;
        } else if (type == CUSTOM_EFFECT_APPLY) {
            s = CUSTOM_EFFECT_SCRIPT_APPLY;
        } else if (type == CUSTOM_EFFECT_TICK) {
            s = CUSTOM_EFFECT_SCRIPT_TICK;
        } else {
            effectEvent.effect = NULL;
            return effectEvent.failed;
        }
        g_pVirtualMachine->RunScript(&s, obj->ObjectID, 1);
    }

    effectEvent.effect = NULL;
    return effectEvent.failed;
}

bool CNWNXEffects::ItempropEvent(CNWSCreature *obj, CNWSItem *item, CNWItemProperty *ip, bool removal, uint32_t slot)
{
    itempropEvent.obj = obj;
    itempropEvent.item = item;
    itempropEvent.ip = ip;
    itempropEvent.suppress = false;
    itempropEvent.slot = slot;
    itempropEvent.remove = removal;

    if (!NotifyEventHooks(hItemprop, (uintptr_t)&itempropEvent)) {
        CExoString s;
        if (removal) {
            s = CUSTOM_IP_SCRIPT_REMOVE;
        } else {
            s = CUSTOM_IP_SCRIPT_APPLY;
        }
        g_pVirtualMachine->RunScript(&s, obj->ObjectID, 1);
    }

    itempropEvent.ip = NULL;

    return itempropEvent.suppress;
}

#define M(lit, str)                                                 \
    strncmp((str), "" lit, (sizeof (lit)/sizeof(char)) - 1) == 0

char *CNWNXEffects::OnRequest(char *gameObject, char *Request, char *Parameters)
{
    Log(1, "Request: \"%s\"\nParams: \"%s\"\n", Request, Parameters);

    if (M("LOG", Request)) {
        CNWSObject *obj = (CNWSObject*)gameObject;
        for (int i = 0; i < obj->EffectsLength; ++i) {
            std::stringstream ss;
            ss << "Effect Type: " << obj->Effects[i]->Type << '\n'
               << "Integers (" << obj->Effects[i]->NumIntegers << "): ";
            for(size_t j = 0; j < obj->Effects[i]->NumIntegers; ++j) {
                ss << obj->Effects[i]->IntList[j] << ", ";
            }
            ss << "\n";

            Log(0, ss.str().c_str());
        }
        return NULL;
    }

    // Effects

    if (M("FORCEAPPLY", Request)) {
        forceApply = 1;
        return NULL;
    }
    if (M("SETEFFECTNATIVEHANDLED", Request)) {
        uint32_t ret = atoi(Parameters);

        if (ret > 0)
            NativeEffectsWithHandlers.push_back(ret);

        return NULL;
    }

    if (M("GETTRUETYPE", Request)) {
        int retval = 0;

        if (effectEvent.effect != NULL)
            retval = effectEvent.effect->Type;

        else
            Log(0, "nwnx_effects: called GETTRUETYPE outside of effects handler\n");

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (M("GETINT", Request)) {
        uint32_t offset = atoi(Parameters);
        int retval = -1;

        if (effectEvent.effect != NULL) {
            if (offset < effectEvent.effect->NumIntegers) {
                retval = effectEvent.effect->GetInteger(offset);
            }

        } else {
            Log(0, "nwnx_effects: called GETINT outside of effects handler\n");
        }

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (M("SETINT", Request)) {
        if (effectEvent.effect != NULL) {
            uint32_t offset;
            int32_t value;

            if (2 != sscanf(Parameters, "%d~%d", &offset, &value)) {
                Log(0, "nwnx_effects: usage error; scanf failed for SETINT\n");
                return NULL;
            }

            if (offset < effectEvent.effect->NumIntegers)
                effectEvent.effect->SetInteger(offset, value);

        } else
            Log(0, "nwnx_effects: called SETINT outside of effects handler\n");

        return NULL;
    }

    if (M("SETFAILED", Request)) {
        uint32_t ret = atoi(Parameters);
        effectEvent.failed = ret != 0;
        return NULL;
    }

    // Itemprops
    if (M("GETIPTYPE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->Type;
        } else {
            Log(0, "nwnx_effects: called GETIPTYPE outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("GETIPSUBTYPE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->Subtype;
        } else {
            Log(0, "nwnx_effects: called GETIPSUBTYPE outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("GETIPPARAMTABLE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->Param1;
        } else {
            Log(0, "nwnx_effects: called GETIPPARAMTABLE outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("GETIPPARAMVALUE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->Param1Value;
        } else {
            Log(0, "nwnx_effects: called GETIPPARAMVALUE outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("GETIPCOSTTABLE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->CostTable;
        } else {
            Log(0, "nwnx_effects: called GETIPCOSTTABLE outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("GETIPCOSTVALUE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->CostValue;
        } else {
            Log(0, "nwnx_effects: called GETIPCOSTVALUE outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("GETIPDURTYPE", Request)) {
        int32_t result = -1;
        if (itempropEvent.ip != NULL) {
            result = itempropEvent.ip->DurationType;
        } else {
            Log(0, "nwnx_effects: called GETIPSLOT outside of itemprop handler\n");
        }
        snprintf(Parameters, strlen(Parameters), "%d", result);
    } else if (M("IPBYPASS", Request)) {
        if (itempropEvent.ip != NULL) {
            itempropEvent.suppress = atoi(Parameters);
        } else {
            Log(0, "nwnx_effects: called GETIPSLOT outside of itemprop handler\n");
        }
    }
    return NULL;
}
unsigned long CNWNXEffects::OnRequestObject(char * gameObject, char * Request)
{
    if (M("GETCREATOR", Request)) {
        if (effectEvent.effect != NULL)
            return effectEvent.effect->CreatorId;

        else
            Log(0, "nwnx_effects: called GETCREATOR outside of effects handler\n");
    }
    else if (M("GETIPITEM", Request)) {
        if (itempropEvent.ip != NULL)
            return itempropEvent.item->Object.ObjectID;

        else
            Log(0, "nwnx_effects: called GETCREATOR outside of effects handler\n");
    }

    return OBJECT_INVALID;
}

bool CNWNXEffects::OnCreate(gline * config, const char * LogDir)
{
    char log[128];
    sprintf(log, "%s/nwnx_effects.txt", LogDir);

    if (!CNWNXBase::OnCreate(config, log)) {
        printf("Failed to init Base\n");
        return false;
    }

    if (!pluginLink) {
        printf("Pluginlink not accessible\n");
        return false;
    }

    hCustom  = CreateHookableEvent(EVENT_EFFECTS_CUSTOM);
    hItemprop  = CreateHookableEvent(EVENT_EFFECTS_IP);

    HookGetScriptEffectType();
    HookEffectHandlers();
    HookCustomEffectUpdate();
    HookExecuteCommandApplyEffectOnObject();
    HookEffectSetNumIntegers();
    HookItempropHandlers();

    return true;
}
