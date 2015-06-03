/**
 * Creates a custom effect.
 *
 * +tickInterval+ is a value in seconds how often a script will be called,
 * as long as the effect is still applied and active. A interval of 0
 * specifies that this effect should not tick.
 *
 * All other parameters are opaque and can be used by you as you see fit;
 * they can be retrieved or set with Get/SetCustomEffectInteger(idx) to
 * store state between calls.
 *
 * One of those integers should probably be a differentiator between your
 * custom effect types.
 *
 * Please note that the integer list is managed by nwnx_effects; if you want
 * to mess with it (for example, through nwnx_structs), see the
 * README.md paragraph "Internals" first.
 */
effect EffectCustom(int tickInterval = 0,
					int arg0 = 0, int arg1 = 0,
					int arg2 = 0, int arg3 = 0,
					int arg4 = 0, int arg5 = 0,
					int arg6 = 0. int arg7 = 0);

/**
 * Returns the custom effect's tickrate (>= 0).
 *
 * This will only work inside nwnx_effect callback scripts. Returns -1 on error.
 */
int GetCustomEffectTickRate();

/**
 * Sets the custom effect's tickrate (>= 0).
 *
 * This will only work inside nwnx_effect callback scripts.
 */
void SetCustomEffectTickRate(int value);

/**
 * Gets any of the custom parameters given to EffectCustom (arg0..arg6).
 *
 * This will only work inside nwnx_effect callback scripts.
 *
 * Returns -1 on error (unless your value is -1 ..).
 */
int GetCustomEffectInteger(int index);

/**
 * Sets any of the custom parameters given to EffectCustom (arg0..arg6).
 *
 * This will only work inside nwnx_effect callback scripts.
 */
void SetCustomEffectInteger(int index, int value);

/**
 * Stops the current apply from happening.
 *
 * This will only work inside the nwnx_effect apply callback.
 */
void SetCustomEffectFailed();

/**
 * Gets the effect creator (whatever was OBJECT_SELF when you called EffectCustom()).
 *
 * This will only work inside nwnx_effect callback scripts.
 */
object GetCustomEffectCreator();


int GetCustomEffectTickRate()
{
	SetLocalString(OBJECT_SELF, "NWNX!EFFECTS!GETTICKRATE", " ");
	return StringToInt(GetLocalString(OBJECT_SELF, "NWNX!EFFECTS!GETTICKRATE"));
}

void SetCustomEffectTickRate(int value)
{
	SetLocalString(OBJECT_SELF, "NWNX!EFFECTS!GETTICKRATE", IntToString(value) + " ");
}

int GetCustomEffectInteger(int index)
{
	SetLocalString(OBJECT_SELF, "NWNX!EFFECTS!GETPARAM", IntToString(index) + " ");
	return StringToInt(GetLocalString(OBJECT_SELF, "NWNX!EFFECTS!GETPARAM"));
}

void SetCustomEffectInteger(int index, int value)
{
	SetLocalString(OBJECT_SELF, "NWNX!EFFECTS!SETPARAM",
				   IntToString(index) + " " + IntToString(value));
}

void SetCustomEffectFailed()
{
	SetLocalString(OBJECT_SELF, "NWNX!EFFECTS!SETFAILED", "1");
}

object GetCustomEffectCreator()
{
	return GetLocalObject(OBJECT_SELF, "NWNX!EFFECTS!GETCREATOR");
}

effect EffectCustom(int tickInterval = 0,
					int arg0 = 0, int arg1 = 0,
					int arg2 = 0, int arg3 = 0,
					int arg4 = 0, int arg5 = 0,
					int arg6 = 0, int arg7 = 0)
{
	effect ret = EffectModifyAttacks(0);

	if (tickInterval > 0) SetEffectInteger(ret, 0, tickInterval);

	// SetEffectInteger(ret, 1, 0); // worldTimerHours
	// SetEffectInteger(ret, 2, 0); // worldTimerSEconds
	if (arg0 != 0) SetEffectInteger(ret, 3, arg0);

	if (arg1 != 0) SetEffectInteger(ret, 4, arg1);

	if (arg2 != 0) SetEffectInteger(ret, 5, arg2);

	if (arg3 != 0) SetEffectInteger(ret, 6, arg3);

	if (arg4 != 0) SetEffectInteger(ret, 7, arg4);

	if (arg5 != 0) SetEffectInteger(ret, 8, arg5);

	if (arg6 != 0) SetEffectInteger(ret, 9, arg6);

	if (arg7 != 0) SetEffectInteger(ret, 10, arg7);

	return ret;
}
