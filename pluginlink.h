#pragma once

typedef struct {
    /* The object on which the effect is applied/removed. */
    CNWSObject  *object;
    /* The effect itself. */
    CGameEffect *effect;
    /* Return true here if the effect cant be applied; this deletes it. */
    bool         failed;
    /* 0: Apply, 1: Remove, 2: Tick */
    int32_t      type;
} EffectsCustomEvent;

/**
 * Event: EVENT_EFFECTS_CUSTOM_APPLY
 * Param: EffectsCustomEvent
 */
#define EVENT_EFFECTS_CUSTOM "Effects/Custom"

typedef struct {
    CNWSCreature    *obj;
    CNWSItem        *item;
    CNWItemProperty *ip;
    uint32_t         slot;
    bool             suppress;
    bool             remove;
} EffectsItempropEvent;

/**
 * Event: EVENT_EFFECTS_IP
 * Param: EffectsItempropEvent
 */
#define EVENT_EFFECTS_IP "Effects/IP"
