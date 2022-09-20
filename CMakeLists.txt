cmake_minimum_required(VERSION 3.0)

project("microcoap_ed" C)



set(TARGET_GROUP production CACHE STRING "Group to build")

add_subdirectory(src)

if(TARGET_GROUP STREQUAL production)

elseif(TARGET_GROUP STREQUAL test)
  include(CTest)
  add_subdirectory(external)
  add_subdirectory(test)
else()
  message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()