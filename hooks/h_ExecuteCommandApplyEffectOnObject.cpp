#include "NWNXEffects.h"

extern CNWNXEffects effects;

static int (*CGameEffect__dtor)(CGameEffect*, char);

static int32_t (*Server_ExecuteCommandApplyEffectOnObject)(CNWVirtualMachineCommands *vm_cmds,
        int cmd, int args);

static int32_t ExecuteCommandApplyEffectOnObject_Hook(CNWVirtualMachineCommands *vm_cmds,
        int cmd, int args)
{
    int dur_type;
    CGameEffect *eff;
    long unsigned int obj_id;
    float duration;

    if (!g_pVirtualMachine->StackPopInteger(&dur_type)                    ||
            !g_pVirtualMachine->StackPopEngineStructure(0, (void **)&eff) ||
            !g_pVirtualMachine->StackPopObject(&obj_id)                   ||
            !g_pVirtualMachine->StackPopFloat(&duration)) {
        // printf("ERROR ApplyEffect: StackPop Failed\n");
        return -639;
    }

    if (dur_type == 1) {
        eff->Duration = duration;
        eff->SubType = (eff->SubType & 0xFFF8) | 1;
    }

    if (dur_type < 0 || dur_type > 4) {
        eff->SubType &= 0xFFF8u;

    } else {
        eff->SubType = (eff->SubType & 0xFFF8) | dur_type;
    }

    CGameObject *ob = g_pAppManager->ServerExoApp->GetGameObject(obj_id);

    if (ob) {
        CNWSCreature *creator_as_creature = g_pAppManager->ServerExoApp->GetCreatureByGameObjectID(eff->CreatorId);
        if (creator_as_creature && creator_as_creature->ItemSpell) {
            eff->CasterLevel = creator_as_creature->ItemSpellLevel;
        }

        CNWSObject *ob_as_object = (CNWSObject*)ob;

        if (ob_as_object) {
            if (eff->Type == 40/*EFFECT_TRUETYPE_LINK*/) {
                ob_as_object->SetPendingEffectRemoval(1);
            }

            // If the second param is 0 effect handlers will not apply if the
            // target is dead or dying.  It's necessary to override this for effects
            // applied from ItemProperties.  If the third param is 0, it will be
            // added to the object's effect list even if not successfully applied.
            ob_as_object->ApplyEffect(eff, effects.forceApply, !effects.forceApply);
            // Reset this so NWScript doesn't have to.
            effects.forceApply = 0;
            ob_as_object->SetPendingEffectRemoval(0);

        } else {
            printf("ApplyEffect: Invalid target object. This should never happen.\n");
        }

    } else if (eff) {
        CGameEffect__dtor(eff, 3);
    }

    return 0;
}

void HookExecuteCommandApplyEffectOnObject()
{
    *(DWORD*) &CGameEffect__dtor = 0x0817DFDC;

    *(void **)&Server_ExecuteCommandApplyEffectOnObject = nx_hook_function(
                (void *) 0x0820db94,
                (void *) ExecuteCommandApplyEffectOnObject_Hook,
                5, NX_HOOK_DIRECT | NX_HOOK_RETCODE);
}
