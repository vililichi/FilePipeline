# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.10)

project ("FilePipeline")

set (SFML_DIR "${CMAKE_SOURCE_DIR}/lib/SFML")
set (CLIENT_DIR "${CMAKE_SOURCE_DIR}/client")
set (SERVER_DIR "${CMAKE_SOURCE_DIR}/server")
set (COMMON_DIR "${CMAKE_SOURCE_DIR}/Common")

# Mettre SFML en Static
add_compile_definitions (SFML_STATIC)
set (SFML_STATIC_LIBRARIES TRUE)
set (SFML_USE_STATIC_LIBS TRUE)
set (BUILD_SHARED_LIBS FALSE)

# Incluez les sous-projets.
add_subdirectory ("${SFML_DIR}")
add_subdirectory ("${CLIENT_DIR}")
add_subdirectory ("${SERVER_DIR}")
