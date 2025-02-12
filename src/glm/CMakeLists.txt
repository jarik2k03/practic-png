add_library(glm STATIC glm.cxx)
target_sources(glm PUBLIC FILE_SET glm TYPE CXX_MODULES FILES glm.cxx) 

