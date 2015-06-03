#include "NWNXEffects.h"

extern PLUGINLINK *pluginLink;

CNWNXEffects::CNWNXEffects()
{
    confKey = strdup("EFFECTS");
}

char *CNWNXEffects::OnRequest(char *gameObject, char *Request, char *Parameters)
{
    Log(1, "Request: \"%s\"\nParams: \"%s\"\n", Request, Parameters);

    if (strcmp("GETTICKRATE", Request) == 0) {
        int retval = -1;

        if (currentEffect != NULL)
            retval = currentEffect->IntList[0];

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (strcmp("SETTICKRATE", Request) == 0) {
        uint32_t value = atoi(Parameters);

        if (currentEffect != NULL && value >= 0)
            currentEffect->IntList[0] = value;

        return NULL;
    }

    if (strcmp("GETPARAM", Request) == 0) {
        uint32_t offset = atoi(Parameters) + 3; /* custom params start at idx 3 */
        int retval = -1;

        if (currentEffect != NULL && offset >= 0 && offset < currentEffect->NumIntegers)
            retval = currentEffect->IntList[offset];

        char *ret;
        asprintf(&ret, "%d", retval);
        return ret;
    }

    if (strcmp("SETPARAM", Request) == 0) {
        uint32_t offset, value;

        if (2 != scanf(Parameters, "%d %d", &offset, &value)) {
            printf("nwnx_effects: usage error; scanf failed for SETPARAM\n");
            return NULL;
        }

        offset += 3; /* custom params start at idx 3 */

        if (currentEffect != NULL && offset >= 0 && offset < currentEffect->NumIntegers)
            currentEffect->IntList[offset] = value;

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

    nx_hook_function((void*)0x0817D22C,
                     (void*)Hook_OnApplyModifyNumAttacks, 5, NX_HOOK_DIRECT);

    nx_hook_function((void*)0x08177D00,
                     (void*)Hook_OnRemoveModifyNumAttacks, 5, NX_HOOK_DIRECT);

    HookCustomEffectUpdate();

    // By default, CGameEffects have 8 integers initialized.
    // We're resetting to 10. For reasons.
    unsigned char *eff_num_ints = (unsigned char*)0x0817dd37;
    nx_hook_enable_write(eff_num_ints, 1);
    memset((void *)eff_num_ints, (uint8_t)11, 1);

    return true;
}
