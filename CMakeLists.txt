include_directories(.)

add_module(effects plugin-effects
	../../api/all ../../api/nwnsyms.S

	NWNXEffects
    hooks/h_OnApplyModifyNumAttacks
    hooks/h_OnRemoveModifyNumAttacks
)

set_target_properties(effects PROPERTIES COMPILE_FLAGS
	"-std=c++0x  -Wno-deprecated -Wno-return-type")
