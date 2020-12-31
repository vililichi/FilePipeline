# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall)

project ("FilePipeline")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
	set(CMAKE_CXX_FLAGS "-std=c++17")
	add_compile_options (-stdlib=libc++)
	add_link_options (-stdlib=libc++)
	add_link_options (-lc++abi)
endif()

set (SFML_DIR "${CMAKE_SOURCE_DIR}/lib/SFML")
set (DOWN_DIR "${CMAKE_SOURCE_DIR}/downloader")
set (UP_DIR "${CMAKE_SOURCE_DIR}/uploader")
set (COMMON_DIR "${CMAKE_SOURCE_DIR}/Common")
set (COMBINED_DIR "${CMAKE_SOURCE_DIR}/combined")

# Mettre SFML en Static
add_compile_definitions (SFML_STATIC)
set (SFML_STATIC_LIBRARIES TRUE)
set (SFML_USE_STATIC_LIBS TRUE)
set (BUILD_SHARED_LIBS FALSE)

# Incluez les sous-projets.
add_subdirectory ("${SFML_DIR}")
add_subdirectory ("${DOWN_DIR}")
add_subdirectory ("${UP_DIR}")
add_subdirectory ("${COMBINED_DIR}")
