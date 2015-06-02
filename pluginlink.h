#pragma once

/**
 * Event: EVENT_EFFECTS_CUSTOM_APPLY
 * Param: EffectsCustomApplyEvent
 */
#define EVENT_EFFECTS_CUSTOM_APPLY "Effects/Custom/Apply"

typedef struct {
	/* The object on which the effect is applied/removed. */
    CNWSObject  *object;
    /* The effect itself. */
    CGameEffect *effect;
    /* Return true here if the effect cant be applied; this deletes it. */
    bool         failed;
} EffectsCustomApplyEvent;

/**
 * Event: EVENT_EFFECTS_CUSTOM_REMOVE
 * Param: EffectsCustomRemoveEvent
 */
#define EVENT_EFFECTS_CUSTOM_REMOVE "Effects/Custom/Remove"

typedef struct {
	/* The object on which the effect is applied/removed. */
    CNWSObject  *object;
    /* The effect itself. */
    CGameEffect *effect;
} EffectsCustomRemoveEvent;
