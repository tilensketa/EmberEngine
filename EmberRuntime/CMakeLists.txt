project(EmberRuntime)

set(SOURCES
  src/RuntimeMain.cpp
  src/GameLayer.cpp
)

add_executable(EmberRuntime ${SOURCES})

target_link_libraries(EmberRuntime PUBLIC EmberEngine)
target_include_directories(EmberRuntime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
