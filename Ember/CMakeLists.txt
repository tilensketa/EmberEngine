project(EmberEngine)

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/spdlog)
add_subdirectory(dependencies/entt)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/stb)
add_subdirectory(dependencies/assimp)
add_subdirectory(dependencies/yaml-cpp)
add_subdirectory(dependencies/reactphysics3d)

set(SOURCES
  src/Core/Application.cpp
  src/Core/Logger.cpp
  src/Core/Window.cpp
  src/Core/Input.cpp

  src/Core/Layers/LayerStack.cpp
  src/Core/Layers/RenderLayer.cpp
  src/Core/Layers/PhysicsLayer.cpp

  src/Project/ProjectManager.cpp

  src/Project/Assets/AssetManager.cpp
  src/Project/Assets/AssetRegistry.cpp
  src/Project/Assets/EngineAssetLoader.cpp
  src/Project/Assets/ProjectAssetLoader.cpp
  src/Project/Assets/AssetImporter.cpp
  src/Project/Assets/AssetDatabase.cpp

  src/Project/Assets/CPU/Loaders/TextureLoader.cpp
  src/Project/Assets/CPU/Loaders/ModelLoader.cpp
  src/Project/Assets/CPU/Loaders/ModelBinaryLoader.cpp

  src/Project/Scene/SceneManager.cpp
  src/Project/Scene/Scene.cpp

  src/Renderer/Renderer.cpp
  src/Renderer/RenderTarget.cpp
  src/Renderer/Shader.cpp
)

find_package(OpenGL REQUIRED)

add_library(EmberEngine STATIC ${SOURCES})
target_link_libraries(EmberEngine PUBLIC glfw glad OpenGL::GL spdlog EnTT glm stb assimp yaml-cpp reactphysics3d)
target_include_directories(EmberEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
