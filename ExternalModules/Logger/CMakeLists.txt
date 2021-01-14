cmake_minimum_required ( VERSION 3.3 )
project ( Logger )

# Enable c++11 compilation
set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

file ( GLOB LOGGER_SOURCES "*.cpp" "*.h" )
add_library ( Logger STATIC ${LOGGER_SOURCES} )
string ( TOUPPER ${CMAKE_SYSTEM_NAME} UPPERCASE_SYSTEM_NAME )
target_compile_definitions ( Logger PUBLIC LOGGER_PLATFORM_${UPPERCASE_SYSTEM_NAME} )
target_include_directories ( Logger PUBLIC ${CMAKE_CURRENT_LIST_DIR}/.. ) # This is definitely not correct but it is the best solution i've found so far, instead of putting all headers on a separate folder
