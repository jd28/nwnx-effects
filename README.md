# What's this?

nwnx_effects allows implementing custom effect types that behave just like
the bioware natives. You get callbacks for OnApply, OnRemove, and you can
specify a interval which calls into a script; much like object heartbeats.


### Caveat

Custom effects misuse the native EffectModifyAttacks() constructor. Any scripts
using that effect type will break. You will have to rewrite your scripts BEFORE
implementing nwnx_effects.

### Caveat 2

nwnx_effects depends on nwnx_structs (specifcally, `SetEffectInteger(...)`).


## Setup

You need to create 3 new scripts in your module:

* `on_ceff_apply.nss`
* `on_ceff_remove.nss`
* `on_ceff_tick.nss`

A example is included in nwn/. There is also a special include
in the same directory that can be used to manage your custom effects inside
those scripts.


## Lifetime of a custom effect


### 1) Creation

    int tickRate = 5;
    effect v = EffectCustom(tickRate, 1, 2, 3);

is used to create a new effect.

`tickRate` is a positive integer describing the interval in which the effect
"heartbeat" will trigger. The value is in seconds.

Any following parameters are completely opaque to nwnx_effects and can be put
to use by you as you see fit; however it is good practice to use the first parameter
as a "custom effect type" flag to differentiate between your implemented effects.


### 2) ApplyEffectToObject

Once you apply your effect to a object, `on_ceff_apply` runs.

`OBJECT_SELF` will be the object you are applying to. You cannot get the raw
`effect` type (since nwscript would always duplicate it on the stack, losing your
modifications) - instead, there are custom helper functions to manage state.

For example, to get your custom parameters, you can use `GetCustomEffectInteger(1)`;
in our example this would be 1. All parameters not specified at creation default
to 0.

You can stop a effect from applying by calling `SetCustomEffectFailed()`. This
will only work inside the apply callback.


### 3) Tick tock, tick tock

Since you have given a tickrate (to remind: 5); `on_ceff_tick` will be called
every 5 seconds AFTER the effect is applied - so the first tick will happen
after 5 seconds.

Effects where tickrate is 0 will never trigger this script.

Tip: You can adjust the tickrate at runtime with `Get/SetCustomEffectTickRate()`.


### 3) Remove

When a effect is removed - either through RemoveEffect, resting, DM healing,
or simply expiring, `on_ceff_remove` is called.


## Internals

nwnx_effects uses some effect integers to keep track of needed state. As such,
the first three integer values are **reserved**. Custom parameters as passed in
to `EffectCustom()` start at index 2.

### CGameEffect->IntList

* `0`: tickrate in seconds
* `1`: last tick world timer hours
* `2`: last tick world timer seconds
* `3` through `9`: user-supplied parameters

To expand on the available user-supplied parameters and account for the reserved
values, nwnx_effects raises the default integer list size to 11 (from 8).
