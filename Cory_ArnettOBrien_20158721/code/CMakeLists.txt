cmake_minimum_required(VERSION 3.21)

project(RealtimeGraphics LANGUAGES CXX)
project(UsingDoxygen LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Doxygen REQUIRED)
find_package(Eigen REQUIRED)
find_package(SDL REQUIRED)
find_package(glText REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Assimp REQUIRED)
find_package(Bullet REQUIRED)

include_directories(.)
include_directories(${Eigen_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${SDL_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Assimp_INCLUDE_DIRS})
include_directories(${glText_INCLUDE_DIRS})
include_directories(../../3rdParty/glm)
include_directories(${Bullet_INCLUDE_DIRS})

add_subdirectory(RGLib)

set(LIBRARIES
    ${SDL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${Assimp_LIBRARIES}
    RGLib
    optimized
        ${OpenCV_LIBRARIES}
    debug
        ${OpenCV_LIBRARIES_DEBUG}
)

foreach(lib ${Bullet_LIBRARIES_DEBUG})
    list(APPEND LIBRARIES debug ${lib})
endforeach()
foreach(lib ${Bullet_LIBRARIES})
    list(APPEND LIBRARIES optimized ${lib})
endforeach()

function(add_executable_rtg name)
    set(SHADER_LIST)
    if(${ARGC} GREATER 1)
        list(SUBLIST ARGV 1 -1 EXTRA_ARGS)
        foreach(ARG ${EXTRA_ARGS})
            list(APPEND SHADER_LIST ${PROJECT_SOURCE_DIR}/shaders/${ARG})
        endforeach()
    endif()


    add_executable(${name}
        ${name}.cpp
        ${HEADERS}
	${SHADER_LIST})

    source_group(Shaders FILES ${SHADER_LIST})

    target_link_libraries(${name} ${LIBRARIES})

    target_compile_features(${name} PRIVATE cxx_std_20)

    set_target_properties(${name} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${SDL_DLL_DIR};${SDL_TTF_DLL_DIR};${OpenCV_DLL_DIR};${GLEW_DLL_DIR};${Assimp_DLL_DIR};%PATH%")
endfunction()
# function (name shaders.vert shaders.frag)
add_executable_rtg(main lambert.vert lambert.frag VertexLighting.vert FragmentLighting.frag FixedColor.frag FixedColor.vert BlinnPhong.vert BlinnPhong.frag ShadowCubeMap.frag
 ShadowCubeMap.vert ShadowMapped.frag ShadowMapped.vert BillboardParticle.frag BillboardParticle.geom BillboardParticle.vert RainParticle.comp NormalShader.vert NormalShader.frag HDRShader.frag HDRShader.vert Water.frag Water.vert DepthTest.frag DepthTest.vert)

set(SOURCES Main.cpp RGLib shaders)
doxygen_add_docs(doxygen ${SOURCES} ALL)

