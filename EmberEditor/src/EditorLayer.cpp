#include "EditorLayer.h"

#include "EditorPanels/Panels.h"

#include "ImGuiFontRegistry.h"

#define FONTS "../../EmberEditor/res/fonts/"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <portable-file-dialogs.h>

namespace Ember {
EditorLayer::EditorLayer(ProjectManager &projectManager,
                         std::shared_ptr<Window> window,
                         AssetRegistry &assetRegistry)
    : mProjectManager(projectManager), mAssetRegistry(assetRegistry) {
  mEditorState = std::make_unique<EditorState>();
  mEditorState->temporary.selectedFolder =
      mProjectManager.GetProjectDirectories().root;
  mEditorState->temporary.camTransform.position = glm::vec3(10,0,0);
  mEditorState->temporary.camCamera.viewMode = Component::Camera::ViewMode::Debug;
  mAssetManager = std::make_unique<AssetManager>("");
  EditorAssetLoader::LoadEditorAssets(*mAssetManager);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window->GetRawPtr(), true);
  ImGui_ImplOpenGL3_Init("#version 460");

  ImFont *main = io.Fonts->AddFontFromFileTTF(
      FONTS "Noto_Sans/static/NotoSans-Light.ttf", 16.0f);
  ImFontConfig cfg;
  cfg.MergeMode = true;
  cfg.PixelSnapH = true;
  static const ImWchar codicon_ranges[] = {0xE000, 0xF8FF, 0};
  ImGuiFontRegistry::Get().pCodicons = io.Fonts->AddFontFromFileTTF(
      FONTS "codicon.ttf", 16.0f, &cfg, codicon_ranges);
  ImGui::PushFont(ImGuiFontRegistry::Get().pCodicons, 0.0f);

  configureCustomTheme();

  mEditorContext = std::make_unique<EditorContext>(
      mProjectManager, *mEditorState, *mAssetManager, mAssetRegistry);

  mEditorPanels = {std::make_shared<SceneViewPanel>(*mEditorContext),
                   std::make_shared<GameViewPanel>(*mEditorContext),
                   std::make_shared<HierarchyPanel>(*mEditorContext),
                   std::make_shared<InspectorPanel>(*mEditorContext),
                   std::make_shared<ProjectPanel>(*mEditorContext),
                   std::make_shared<EditorPanel>(*mEditorContext),
                   std::make_shared<AssetRegistryPanel>(*mEditorContext),
                   std::make_shared<MaterialEditorPanel>(*mEditorContext),
                   std::make_shared<TopBarPanel>(*mEditorContext)};
}

EditorLayer::~EditorLayer() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void EditorLayer::OnAttach() {}
void EditorLayer::OnDetach() {}
void EditorLayer::OnUpdate() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport();
  handleMainMenu();
  if (mRequestSceneCreatePopup) {
    ImGui::OpenPopup("Scene Create");
    mRequestSceneCreatePopup = false;
  }

  showSceneCreateWindow();

  ImGui::ShowDemoWindow();

  for (auto editorPanel : mEditorPanels) {
    editorPanel->Update();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void EditorLayer::OnEvent(Event::IEvent &event) {
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<Event::ProjectOpen>(
      Event::EventType::PROJECT_OPEN, [this](Event::ProjectOpen &e) {
        auto &temp = mEditorState->temporary;
        temp.selectedFolder = e.directories.root;
        temp.selectedEntityHandle.reset();
        temp.selectedMaterial.reset();
        return true;
      });

  for (auto &panel : mEditorPanels) {
    panel->OnEvent(event);
    if (event.handled) {
      break;
    }
  }
}

void EditorLayer::handleMainMenu() {
  auto &folder = mEditorState->temporary.selectedFolder;
  ImGui::BeginMainMenuBar();
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New")) {
      auto dir = pfd::select_folder("Select project folder", folder).result();
      if (!dir.empty()) {
        auto folderPath = std::filesystem::path(dir);
        mProjectManager.Create(folderPath);
      }
    }
    if (ImGui::MenuItem("Open")) {
      auto f =
          pfd::open_file("Choose file to read", folder,
                         {"Ember Files (.ember)", "*.ember", "All Files", "*"});
      if (!f.result().empty()) {
        auto path = std::filesystem::path(f.result()[0]);
        auto folderPath = path.parent_path();
        mProjectManager.Open(folderPath);
      }
    }
    if (ImGui::MenuItem("Save")) mProjectManager.Save();
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Assets")) {
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Edit")) {
    if (ImGui::MenuItem("Add Scene")) {
      mRequestSceneCreatePopup = true;
    }
    ImGui::EndMenu();
  }
  ImGui::EndMainMenuBar();
}

void EditorLayer::showSceneCreateWindow() {
  if (ImGui::BeginPopupModal("Scene Create", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {

    static char buffer[64];
    ImGui::InputText("Scene name", buffer, sizeof(buffer));
    std::string name = std::string(buffer);

    if (ImGui::Button("Create")) {
      mProjectManager.GetSceneManager().CreateScene(name);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void EditorLayer::configureCustomTheme() {
  ImGuiStyle *style = &ImGui::GetStyle();
  ImVec4 *colors = style->Colors;

  colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.94f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.52f, 0.54f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(0.28f, 0.29f, 0.30f, 0.60f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.30f, 0.32f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.32f, 0.34f, 0.36f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.54f, 0.64f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.34f, 0.44f, 0.35f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.46f, 0.56f, 0.66f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

  // Style adjustments
  style->WindowPadding = ImVec2(8.00f, 8.00f);
  style->FramePadding = ImVec2(5.00f, 2.00f);
  style->CellPadding = ImVec2(6.00f, 6.00f);
  style->ItemSpacing = ImVec2(6.00f, 6.00f);
  style->ItemInnerSpacing = ImVec2(6.00f, 6.00f);
  style->TouchExtraPadding = ImVec2(0.00f, 0.00f);
  style->IndentSpacing = 25;
  style->ScrollbarSize = 11;
  style->GrabMinSize = 10;
  style->WindowBorderSize = 1;
  style->ChildBorderSize = 1;
  style->PopupBorderSize = 1;
  style->FrameBorderSize = 1;
  style->TabBorderSize = 1;
  style->WindowRounding = 7;
  style->ChildRounding = 4;
  style->FrameRounding = 3;
  style->PopupRounding = 4;
  style->ScrollbarRounding = 9;
  style->GrabRounding = 3;
  style->LogSliderDeadzone = 4;
  style->TabRounding = 4;
}

} // namespace Ember
