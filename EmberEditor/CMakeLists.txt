project(EmberEditor)

add_subdirectory(dependencies/imgui)
add_subdirectory(dependencies/ImGuizmo)
add_subdirectory(dependencies/portable-file-dialogs)

set(SOURCES
  src/EditorMain.cpp
  src/EditorLayer.cpp
  src/EditorAssetLoader.cpp

  src/EditorPanels/InspectorPanel/InspectorPanel.cpp
  src/EditorPanels/HierarchyPanel/HierarchyPanel.cpp
  src/EditorPanels/ProjectPanel/ProjectPanel.cpp
  src/EditorPanels/SceneViewPanel/SceneViewPanel.cpp
  src/EditorPanels/EditorPanel/EditorPanel.cpp
  src/EditorPanels/AssetRegistryPanel/AssetRegistryPanel.cpp
  src/EditorPanels/MaterialEditorPanel/MaterialEditorPanel.cpp
  src/EditorPanels/GameViewPanel/GameViewPanel.cpp
)

add_executable(EmberEditor ${SOURCES})

target_link_libraries(EmberEditor PUBLIC EmberEngine imgui ImGuizmo portable_file_dialogs)
target_include_directories(EmberEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
