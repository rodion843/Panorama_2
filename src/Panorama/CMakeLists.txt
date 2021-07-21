
set(CMAKE_PREFIX_PATH "C:/opencv/build")
find_package(OpenCV 4.5.2 REQUIRED)

add_executable(Panorama Panorama.cpp)

add_library(PylonLib STATIC IMPORTED)
set_target_properties(PylonLib PROPERTIES 
        LINKER_LANGUAGE CXX
        IMPORTED_LOCATION "C:/Program Files/Basler/pylon 6/Development/lib/x64/GCBase_MD_VC141_v3_1_Basler_pylon.lib"
        IMPORTED_GLOBAL TRUE
        )


target_include_directories(Panorama PUBLIC "C:/Program Files/Basler/pylon 6/Development/include")
target_link_directories(Panorama PUBLIC "C:/Program Files/Basler/pylon 6/Development/lib/x64")

#set_target_properties(PylonLib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries( Panorama
        ${OpenCV_LIBS}
        "C:/Program\ Files/Basler/pylon\ 6/Development/lib/x64/GCBase_MD_VC141_v3_1_Basler_pylon.lib"
        )


target_compile_features(Panorama PRIVATE cxx_std_20)
