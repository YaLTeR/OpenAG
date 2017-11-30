function(add_sources)
	foreach(SOURCE_FILE ${ARGN})
		set_property(GLOBAL APPEND PROPERTY SOURCE_FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
	endforeach(SOURCE_FILE)
endfunction(add_sources)

macro(msvc_set_static_runtime)
	set(FLAGS_VARIABLES
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO)

	foreach(FLAGS_VARIABLE ${FLAGS_VARIABLES})
		if(${FLAGS_VARIABLE} MATCHES "/MD")
			string(REPLACE "/MD" "/MT" ${FLAGS_VARIABLE} "${${FLAGS_VARIABLE}}")
		endif(${FLAGS_VARIABLE} MATCHES "/MD")
	endforeach(FLAGS_VARIABLE)
endmacro(msvc_set_static_runtime)

macro(msvc_fix_xp_build)
	# http://stackoverflow.com/a/32953859/4214632
	if(CMAKE_GENERATOR_TOOLSET MATCHES "_xp")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zc:threadSafeInit-")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:threadSafeInit-")
	endif(CMAKE_GENERATOR_TOOLSET MATCHES "_xp")
endmacro(msvc_fix_xp_build)

macro(msvc_enable_lto)
	# Add /GL (Whole Program Optimization)
	# Debug and RelWithDebInfo use /INCREMENTAL which is incompatible with /LTCG
	set(CONFIGURATIONS
		MINSIZEREL
		RELEASE)

	set(FLAGS_VARIABLES
		CMAKE_C_FLAGS
		CMAKE_CXX_FLAGS)

	foreach(CONFIGURATION_NAME ${CONFIGURATIONS})
		foreach(FLAGS_VARIABLE ${FLAGS_VARIABLES})
			set(VARIABLE_NAME ${FLAGS_VARIABLE}_${CONFIGURATION_NAME})
			set(${VARIABLE_NAME} "${${VARIABLE_NAME}} /GL")
		endforeach(FLAGS_VARIABLE)

		# Add /LTCG and /LTCG:incremental (Link Time Code Generation)
		set(CMAKE_STATIC_LINKER_FLAGS_${CONFIGURATION_NAME} "${CMAKE_STATIC_LINKER_FLAGS_${CONFIGURATION_NAME}} /LTCG")
		set(CMAKE_SHARED_LINKER_FLAGS_${CONFIGURATION_NAME} "${CMAKE_SHARED_LINKER_FLAGS_${CONFIGURATION_NAME}} /LTCG:incremental")
	endforeach(CONFIGURATION_NAME)
endmacro(msvc_enable_lto)
