include(CMakeSources.cmake)
set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
CREATE_MODULE(shiva::json "${MODULE_SOURCES}" ${MODULE_PATH})
target_link_libraries(json INTERFACE nlohmann_json)
#find_path(JSON_INCLUDE_DIR nlohmann/json.hpp)
#target_include_directories(json INTERFACE ${JSON_INCLUDE_DIR})
AUTO_TARGETS_MODULE_INSTALL(json)
