# CMakeList.txt : projet CMake pour AVCs, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("client")

set (SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set (COMMON_DIR"${CMAKE_SOURCE_DIR}/Common")

# Ajoutez une source à l'exécutable de ce projet.
include_directories ("${SRC_DIR}")
include_directories ("${COMMON_DIR}")

file (GLOB G_SRC "${SRC_DIR}/*.cpp")
file (GLOB G_HDR "${SRC_DIR}/*.h")

file (GLOB G_COMMON_SRC "${COMMON_DIR}/*/*.cpp")
file (GLOB G_COMMON_HDR "${COMMON_DIR}/*/*.h")

# add_executable
add_executable ("${PROJECT_NAME}" "${G_COMMON_SRC}" "${G_COMMON_HDR}" "${G_SRC}" "${G_HDR}")
target_link_libraries ("${PROJECT_NAME}" sfml-network sfml-system)

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
