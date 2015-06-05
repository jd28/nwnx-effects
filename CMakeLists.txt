include_directories(.)

add_module(effects plugin-effects
	../../api/all ../../api/nwnsyms.S

	NWNXEffects
	hooks/h_EffectHandlers
	hooks/h_GetScriptEffectType
    hooks/h_UpdateEffectList
    hooks/h_ExecuteCommandApplyEffectOnObject
)

set_target_properties(effects PROPERTIES COMPILE_FLAGS
	"-std=c++0x  -Wno-deprecated -Wno-return-type")
