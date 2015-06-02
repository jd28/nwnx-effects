int GetCustomEffectInteger(int index);

void SetCustomEffectFailed();

int GetCustomEffectInteger(int index)
{
    SetLocalString(GetModule(), "NWNX!EFFECTS!GETCUSTOMEFFECTPARAM",
                   IntToString(index) + " ");
    return StringToInt(GetLocalString(GetModule("NWNX!EFFECTS!GETCUSTOMEFFECTPARAM")));
}

void SetCustomEffectFailed()
{
    SetLocalString(GetModule(), "NWNX!EFFECTS!SETCUSTOMEFFECTFAILED", "1");
}
