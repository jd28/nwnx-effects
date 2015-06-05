#include "NWNXEffects.h"

extern PLUGINLINK *pluginLink;

CNWNXEffects::CNWNXEffects()
{
    confKey = strdup("EFFECTS");
}

AttachedEffectData *CNWNXEffects::GetAttachedEffectData(nweffectid id)
{
    auto a = EffectsAttachedEffectData.find(id);
    if (a != EffectsAttachedEffectData.end())
        return a->second;

    return NULL;
}

void CNWNXEffects::CleanupEffect(nweffectid id)
{
    auto a = EffectsAttachedEffectData.find(id);
    if (a != EffectsAttachedEffectData.end()) {
        AttachedEffectData *d = a->second;
        EffectsAttachedEffectData.erase(id);
        delete d;

        printf("cleanup attached data for %d, size: %d\n",
               id, EffectsAttachedEffectData.size());
    }
}

bool CNWNXEffects::CallEffectHandler(const char* handler, CNWSObject* obj, CGameEffect *eff)
{
    currentEffect = eff;
    currentResult = false;

    CExoString s(handler);
    g_pVirtualMachine->RunScript(&s, obj->ObjectID, 1);

    currentEffect = NULL;
    return currentResult > 0;
}

char *CNWNXEffects::OnRequest(char *gameObject, char *Request, char *Parameters)
{
    Log(1, "Request: \"%s\"\nParams: \"%s\"\n", Request, Parameters);

    if (strcmp("SETNATIVEHANDLED", Request) == 0) {
        uint32_t ret = atoi(Parameters);
        if (ret > 0)
            EffectsWithEffectHandlers.push_back(ret);

        return NULL;
    }

    if (strcmp("GETTRUETYPE", Request) == 0) {
        int retval = 0;

        if (currentEffect != NULL)
            retval = currentEffect->Type;

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (strcmp("GETTICKRATE", Request) == 0) {
        int retval = -1;

        if (currentEffect != NULL) {
            retval = 0;
            AttachedEffectData *d = GetAttachedEffectData(currentEffect->Id);
            if (d != NULL)
                retval = d->tickRate;
        }

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (strcmp("SETTICKRATE", Request) == 0) {
        uint32_t value = atoi(Parameters);

        if (currentEffect != NULL && value >= 0) {
            AttachedEffectData* data;
            auto a = EffectsAttachedEffectData.find(currentEffect->Id);
            if (a != EffectsAttachedEffectData.end())
                data = a->second;
            else {
                data = new AttachedEffectData;
                EffectsAttachedEffectData[currentEffect->Id] = data;
            }

            data->tickRate = value;
        }

        return NULL;
    }

    if (strcmp("GETINT", Request) == 0) {
        uint32_t offset = atoi(Parameters) + CUSTOM_EFFECT_PROPERTIES_START_AT;
        int retval = -1;

        if (currentEffect != NULL && offset >= 0 && offset < currentEffect->NumIntegers)
            retval = currentEffect->GetInteger(offset);

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (strcmp("SETINT", Request) == 0) {
        uint32_t offset, value;

        if (2 != sscanf(Parameters, "%d~%d", &offset, &value)) {
            printf("nwnx_effects: usage error; scanf failed for SETINT\n");
            return NULL;
        }

        offset += CUSTOM_EFFECT_PROPERTIES_START_AT;

        if (currentEffect != NULL && offset >= 0 && offset < currentEffect->NumIntegers)
            currentEffect->SetInteger(offset, value);

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
    if (strcmp("GETCREATOR", Request) == 0)
        if (currentEffect != NULL)
            return currentEffect->CreatorId;

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

    hCustomApply  = CreateHookableEvent(EVENT_EFFECTS_CUSTOM_APPLY);
    hCustomRemove = CreateHookableEvent(EVENT_EFFECTS_CUSTOM_REMOVE);

    HookGetScriptEffectType();
    HookEffectHandlers();
    HookCustomEffectUpdate();

    // By default, CGameEffects have 8 integers initialized.
    // We're resetting to 12. For reasons.
    unsigned char *eff_num_ints = (unsigned char*)0x0817dd37;
    nx_hook_enable_write(eff_num_ints, 1);
    memset((void *)eff_num_ints, (uint8_t)12, 1);

    return true;
}
