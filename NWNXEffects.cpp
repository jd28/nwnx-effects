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
char *CNWNXEffects::OnRequest(char *gameObject, char *Request, char *Parameters)
{
	Log(1, "Request: \"%s\"\nParams: \"%s\"\n", Request, Parameters);

	if (strcmp("SETEFFECTNATIVEHANDLED", Request) == 0) {
		uint32_t ret = atoi(Parameters);

		if (ret > 0)
			NativeEffectsWithHandlers.push_back(ret);

		return NULL;
	}

	if (strcmp("GETTRUETYPE", Request) == 0) {
		int retval = 0;

		if (currentEffect != NULL)
			retval = currentEffect->Type;

		else
			printf("nwnx_effects: called GETTRUETYPE outside of effects handler\n");

		char *ret;
		asprintf(&ret, "%d", retval);
		return ret;
	}

	if (strcmp("GETINT", Request) == 0) {
		uint32_t offset = atoi(Parameters);
		int retval = -1;

		if (currentEffect != NULL) {
			if (offset >= 0 && offset < currentEffect->NumIntegers) {
				retval = currentEffect->GetInteger(offset);
			}

		} else
			printf("nwnx_effects: called GETINT outside of effects handler\n");

		char *ret;
		asprintf(&ret, "%d", retval);
		return ret;
	}

	if (strcmp("SETINT", Request) == 0) {
		if (currentEffect != NULL) {
			uint32_t offset, value;

			if (2 != sscanf(Parameters, "%d~%d", &offset, &value)) {
				printf("nwnx_effects: usage error; scanf failed for SETINT\n");
				return NULL;
			}

			if (currentEffect != NULL && offset >= 0 && offset < currentEffect->NumIntegers)
				currentEffect->SetInteger(offset, value);

		} else
			printf("nwnx_effects: called SETINT outside of effects handler\n");

		return NULL;
	}

	if (strcmp("SETFAILED", Request) == 0) {
		uint32_t ret = atoi(Parameters);
		currentResult = ret != 0;
		return NULL;
	}

	return NULL;
}

unsigned long CNWNXEffects::OnRequestObject(char *gameObject, char *Request)
{
	if (strcmp("GETCREATOR", Request) == 0) {
		if (currentEffect != NULL)
			return currentEffect->CreatorId;

		else
			printf("nwnx_effects: called GETCREATOR outside of effects handler\n");
	}

	return NULL;
}

bool CNWNXEffects::OnCreate(gline *config, const char *LogDir)
{
	char log[128];
	sprintf(log, "%s/nwnx_effects.txt", LogDir);

	if (!CNWNXBase::OnCreate(config, log))
		return false;

	if (!pluginLink) {
		Log(0, "Pluginlink not accessible\n");
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
