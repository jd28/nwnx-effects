include_directories(.)

add_module(effects plugin-effects
    NWNXEffects
    pluginlink.h
    hooks/h_SetNumIntegers
    hooks/h_EffectHandlers
    hooks/h_GetScriptEffectType
    hooks/h_UpdateEffectList
    hooks/h_ExecuteCommandApplyEffectOnObject
    hooks/h_ItempropHandlers
)

set_target_properties(effects PROPERTIES COMPILE_FLAGS
    "-std=c++0x  -Wno-deprecated -Wno-return-type")
