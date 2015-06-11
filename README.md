# What's this?

nwnx_effects allows implementing custom effect types that behave just like
the bioware natives. You get callbacks for OnApply, OnRemove, and you can
specify a interval which calls into a script; much like object heartbeats.

Additionally, nwnx_effects allows specifying the same callbacks for native
effect types, with the caveat that you cannot prevent them from applying.

### Caveat

nwnx_effects depends on nwnx_structs to set up new effects initially. See
the include in nwn/ for details.

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

    const int EFFECT_TRUETYPE_MY_FUNKY_EFFECT = 100;
    effect v = EffectCustom(EFFECT_TRUETYPE_MY_FUNKY_EFFECT);

is used to create a new effect. You need to use IDs larger than the
latest one provided by bioware (EFFECT_TRUETYPE_DEFENSIVESTANCE = 95).

All IDs larger than that are assumed to be custom effects.

You should use local integers, strings, objects and floats (as per nwnx_structs)
to define behaviour or to keep state for your new effect.

Please note that effect integer >= 20 are RESERVED for nwnx_effects usage.

### 2) ApplyEffectToObject

Once you apply your effect to a object, `on_ceff_apply` runs.

`OBJECT_SELF` will be the object you are applying to. You cannot get the raw
`effect` type (since nwscript would always duplicate it on the stack, losing your
modifications) - instead, there are custom helper functions to manage state.

For example, to get your custom parameters, you can use `GetCustomEffectInteger(0)`
to retrieve the first int.
All parameters not specified at creation default to 0.

You can stop a effect from applying by calling `SetCustomEffectFailed()`. This
will only work inside the apply callback.

You can make your effect have a tick rate - that is, a heartbeat that is called
every n seconds, by using `SetCustomEffectTickRate(5);`.

Please keep in mind that this heartbeat script is called for every effect.
While the handler is fairly fast, it it still subject to reality.
If you apply a thousand effects like this with a tickrate of one second, it will
bog down your server.

### 3) Tick tock, tick tock

Since you have given a tickrate (to remind: 5); `on_ceff_tick` will be called
every 5 seconds AFTER the effect is applied - so the first tick will happen
after 5 seconds.

Effects where tickrate is 0 will never trigger this script.

Tip: You can adjust the tickrate at runtime with `Get/SetCustomEffectTickRate()`,
but only inside nwnx_effects handler scripts.

### 3) Remove

When a effect is removed - either through RemoveEffect, resting, DM healing,
or simply expiring, `on_ceff_remove` is called.


## Internals

Effect integer >= 20 are RESERVED for nwnx_effects usage.
