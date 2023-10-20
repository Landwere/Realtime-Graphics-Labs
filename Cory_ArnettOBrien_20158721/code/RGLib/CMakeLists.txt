add_library(RGLib 
modelLoader.cpp
modelLoader.hpp
shader.h
shader.cpp
ShaderProgram.cpp
ShaderProgram.hpp
ShaderUtils.h
ShaderUtils.cpp
Camera.h
Camera.cpp
ModelRenderer.cpp
ModelRenderer.h
Texture.cpp
Texture.h)

target_compile_features(RGLib PRIVATE cxx_std_20)
