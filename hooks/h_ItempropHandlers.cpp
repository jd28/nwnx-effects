#include "NWNXEffects.h"

extern CNWNXEffects effects;

// Note that the return value of these functions is ignored.

int (*CServerAIMaster__OnItemPropertyApplied_orig)(CServerAIMaster*, CNWSItem*, CNWItemProperty*, CNWSCreature*, uint32_t, int);
int (*CServerAIMaster__OnItemPropertyRemoved_orig)(CServerAIMaster*, CNWSItem*, CNWItemProperty*, CNWSCreature*, uint32_t);

int CServerAIMaster__OnItemPropertyApplied_Hook(CServerAIMaster *ai, CNWSItem *item, CNWItemProperty *ip, CNWSCreature *cre, uint32_t slot, int b)
{
    if (cre == NULL || item == NULL || ip == NULL) {
        return 0;
    }

    bool suppress = false;

    if (!effects.in_script) {
        suppress = effects.ItempropEvent(cre, item, ip, false, slot);
    }
    if (suppress) { return 0; }

    CServerAIMaster__OnItemPropertyApplied_orig(ai, item, ip, cre, slot, b);

    return 0;
}

// Item property removal cannot be supressed.
int CServerAIMaster__OnItemPropertyRemoved_Hook(CServerAIMaster *ai, CNWSItem *item, CNWItemProperty *ip, CNWSCreature *cre, uint32_t slot)
{
    if (cre == NULL || item == NULL || ip == NULL) {
        return 0;
    }

    int result = CServerAIMaster__OnItemPropertyRemoved_orig(ai, item, ip, cre, slot);

    if (!effects.in_script) {
        effects.ItempropEvent(cre, item, ip, true, slot);
    }

    return 0;
}

void HookItempropHandlers()
{
    NX_HOOK(CServerAIMaster__OnItemPropertyApplied_orig, 0x08098528,
            CServerAIMaster__OnItemPropertyApplied_Hook, 6);

    NX_HOOK(CServerAIMaster__OnItemPropertyRemoved_orig, 0x08098550,
            CServerAIMaster__OnItemPropertyRemoved_Hook, 6);
}
