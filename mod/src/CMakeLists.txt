target_precompile_headers(${MOD_TARGET}
    PRIVATE
        Pch.h
)
target_sources(${MOD_TARGET}
    PRIVATE
        version.rc
        DllMain.cpp
        Mod.cpp
        MusicAttenuationService.cpp
        OptionsBootstrap_${TARGET_SDK}.cpp
    PRIVATE FILE_SET HEADERS FILES
        Pch.h
        utilities/Helpers.h
        MusicAttenuationService.h
        OptionsBootstrap.h
)
