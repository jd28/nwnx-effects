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
    }

    if (dur_type < 0 || dur_type > 4) {
        eff->SubType &= 0xFFF8u;

    } else {
        eff->SubType = (eff->SubType & 0xFFF8) | dur_type;
    }


    CGameObject *ob = g_pAppManager->ServerExoApp->GetGameObject(obj_id);

    if (ob) {
        CGameObject *effect_creator       = g_pAppManager->ServerExoApp->GetGameObject(eff->CreatorId);
        CNWSCreature *creator_as_creature = ((CNWSCreature * (*)(CGameObject *)) effect_creator->Methods->AsNWSCreature)(effect_creator);

        CNWSCreature *ob_as_creature = ((CNWSCreature * (*)(CGameObject *)) ob->Methods->AsNWSCreature)(ob);

        if (
            ob_as_creature && creator_as_creature && ob_as_creature == creator_as_creature &&
            (int) * ((int *)creator_as_creature + 0xa74) /*cre->cre_item_spell*/) {
            eff->CasterLevel = (int) * ((int *)creator_as_creature + 0xa78); /*cre->cre_item_spell_level*/
        }

        // if (cre && (int) * ((int *)cre + 0xa74) /*cre->cre_item_spell*/) {
        //  eff->SpellId = (int) * ((int *)cre + 0xa78); /*cre->cre_item_spell_level*/
        // }

        CNWSObject *ob_as_object = ((CNWSObject * (*)(CGameObject *)) ob->Methods->AsNWSObject)(ob);

        if (ob_as_object) {
            if (eff->Type == 40/*EFFECT_TRUETYPE_LINK*/) {
                ob_as_object->SetPendingEffectRemoval(1);
            }

            ob_as_object->ApplyEffect(eff, 0, 1);
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
