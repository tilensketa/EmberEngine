#include "ProjectPanel.h"
#include "imgui_internal.h"

namespace Ember {

void ProjectPanel::Update() {
  ImGui::Begin("Project");

  handleNavigationBar();
  handleFileViewer();

  ImGui::End();

  if (mRequestTextureImportPopup) {
    ImGui::OpenPopup("Texture Import");
    mRequestTextureImportPopup = false;
  }
  if (mRequestModelImportPopup) {
    ImGui::OpenPopup("Model Import");
    mRequestModelImportPopup = false;
  }
  if (mRequestMaterialCreatePopup) {
    ImGui::OpenPopup("Material Create");
    mRequestMaterialCreatePopup = false;
  }
  if (mRequestFolderCreatePopup) {
    ImGui::OpenPopup("Folder Create");
    mRequestFolderCreatePopup = false;
  }

  showMaterialCreateWindow();
  showFolderCreateWindow();

  showTextureLoadWindow();
  showModelLoadWindow();
}

void ProjectPanel::OnEvent(Ember::Event::IEvent &event) {}

void ProjectPanel::handleNavigationBar() {
  float buttonHeight =
      ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
  ImGui::BeginChild("NavigationBar", ImVec2(0, buttonHeight));
  auto &selectedFolder = mCtx.editorState.temporary.selectedFolder;

  auto &projectManager = mCtx.projectManager;
  auto projectPath = projectManager.GetProjectDirectories().root;
  auto path = std::filesystem::relative(selectedFolder, projectPath);
  auto newPath = projectPath;
  ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
  if (ImGui::Button(projectManager.GetProjectDirectories().projectName.c_str()))
    selectedFolder = projectPath;
  ImGui::SameLine();
  ImGui::Text("/");
  ImGui::SameLine();
  for (const auto p : path) {
    if (p == ".") continue;
    newPath /= p;
    if (ImGui::Button(p.c_str())) {
      selectedFolder = newPath;
    }
    ImGui::SameLine();
    ImGui::Text("/");
    ImGui::SameLine();
  }
  ImGui::PopStyleColor();
  ImGui::EndChild();
}
void ProjectPanel::handleFileViewer() {
  ImGui::BeginChild("FileView");
  handleAddMenu();

  const float iconSize = 100.0f;
  const float padding = 16.0f;
  const float cellWidth = iconSize + padding;

  float panelWidth = ImGui::GetContentRegionAvail().x;
  int itemsPerRow = (int)(panelWidth / cellWidth);
  if (itemsPerRow < 1) itemsPerRow = 1;

  int itemIndex = 0;

  for (const auto &entry : std::filesystem::directory_iterator(
           mCtx.editorState.temporary.selectedFolder)) {
    const FileType fileType = FileTypeUtils::FromPath(entry.path());
    if (fileType == FileType::AssetMeta) continue;
    ImGui::PushID(entry.path().c_str());

    const auto filename = entry.path().filename().string();
    bool isFolder = entry.is_directory();

    AssetHandle displayIconHandle =
        EditorAssetLoader::GetIcon(EditorIcon::FileUnknown);
    std::shared_ptr<Asset::Texture> texture = nullptr;
    bool isEditorAsset = true;

    if (isFolder) {
      displayIconHandle = EditorAssetLoader::GetIcon(EditorIcon::Folder);
    } else {
      EditorIcon icon = fileTypeToEditorIcon(fileType);
      displayIconHandle = EditorAssetLoader::GetIcon(icon);
    }
    if (isEditorAsset)
      texture =
          mCtx.assetManager.GetAsset<Asset::Texture>(displayIconHandle.guid);
    else
      texture =
          mCtx.assetRegistry.GetAsset<Asset::Texture>(displayIconHandle.guid);

    ImGui::BeginGroup();

    // Icon
    ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
    if (ImGui::ImageButton("##icon", (ImTextureID)(intptr_t)texture->gpu.id,
                           ImVec2(iconSize, iconSize))) {
      if (isFolder) {
        mCtx.editorState.temporary.selectedFolder = entry;
      } else {
        mPendingImportPath = std::filesystem::relative(
            entry.path(), mCtx.projectManager.GetProjectDirectories().assets);
        if (fileType == FileType::ModelSource) {
          mRequestModelImportPopup = true;
        } else if (fileType == FileType::TextureSource) {
          mRequestTextureImportPopup = true;
        } else if (fileType == FileType::Material) {
          auto handleOpt =
              mCtx.assetRegistry.GetAssetHandleByPath(mPendingImportPath);
          if (handleOpt)
            mCtx.editorState.temporary.selectedMaterial = handleOpt->guid;
        } else if (fileType == FileType::Scene) {
          // TODO fix this, make it load scene meta
          YAML::Node node = YAML::LoadFile(entry.path());
          GUID sceneGuid = node["guid"].as<GUID>();
          mCtx.projectManager.GetSceneManager().ChangeScene(sceneGuid);
          mCtx.editorState.temporary.selectedEntityHandle = std::nullopt;
        }
      }
    }
    ImGui::PopStyleColor();

    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + iconSize);
    ImGui::Text("%s", filename.c_str());
    ImGui::PopTextWrapPos();

    ImGui::EndGroup();

    ImGui::PopID();

    // Layout flow
    itemIndex++;
    if (itemIndex % itemsPerRow != 0) ImGui::SameLine();
  }

  ImGui::EndChild();
}

void ProjectPanel::handleAddMenu() {
  if (ImGui::BeginPopupContextWindow("ProjectAddPopup",
                                     ImGuiPopupFlags_MouseButtonRight)) {
    ImGui::Text("ADD");
    ImGui::Separator();

    if (ImGui::MenuItem("Material")) {
      mRequestMaterialCreatePopup = true;
    } else if (ImGui::MenuItem("Folder")) {
      mRequestFolderCreatePopup = true;
    }

    ImGui::EndPopup();
  }
}

EditorIcon ProjectPanel::fileTypeToEditorIcon(const FileType &type) {
  switch (type) {
  case FileType::ModelSource: return EditorIcon::FileModel;
  case FileType::TextureSource: return EditorIcon::FileTexture;
  case FileType::Material: return EditorIcon::FileMaterial;
  case FileType::Scene: return EditorIcon::FileScene;
  case FileType::Project: return EditorIcon::FileProject;
  default: return EditorIcon::FileUnknown;
  }
}

void ProjectPanel::showMaterialCreateWindow() {
  if (!ImGui::BeginPopupModal("Material Create", nullptr,
                              ImGuiWindowFlags_AlwaysAutoResize))
    return;

  const auto &selectedFolder = mCtx.editorState.temporary.selectedFolder;
  ImGui::Text("Path: %s", selectedFolder.string().c_str());

  static char buffer[64];
  ImGui::InputText("Material name", buffer, sizeof(buffer));
  std::string name = std::string(buffer);

  if (ImGui::Button("Add Material")) {
    auto handleOpt = mCtx.projectManager.GetAssetImporter().CreateMaterial(
        selectedFolder / name);
    if (!handleOpt) {
      ImGui::CloseCurrentPopup();
      return;
    }

    auto metaOpt = mCtx.projectManager.GetAssetManager().GetAssetMetaByGuid(
        handleOpt->guid);
    if (!metaOpt) {
      ImGui::CloseCurrentPopup();
      return;
    }

    mCtx.projectManager.GetAssetManager().LoadMaterial(*metaOpt);
    mCtx.editorState.temporary.selectedMaterial = handleOpt->guid;
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel")) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

void ProjectPanel::showTextureLoadWindow() {
  if (ImGui::BeginPopupModal("Texture Import", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Path: %s", mPendingImportPath.c_str());
    auto textureTypeNames =
        Utils::EnumTraits<TextureLoader::ImportSettings::TextureType>::Names;
    auto filterTypeNames =
        Utils::EnumTraits<TextureLoader::ImportSettings::FilterType>::Names;
    auto wrapTypeNames =
        Utils::EnumTraits<TextureLoader::ImportSettings::WrapType>::Names;

    static int currentTextureType =
        static_cast<int>(TextureLoader::ImportSettings::TextureType::Albedo);
    static int currentFilterType =
        static_cast<int>(TextureLoader::ImportSettings::FilterType::Linear);
    static int currentWrapType =
        static_cast<int>(TextureLoader::ImportSettings::WrapType::ClampToEdge);
    static bool generateMipmaps = true;

    ImGui::Combo("Texture Type", &currentTextureType, textureTypeNames.data(),
                 static_cast<int>(textureTypeNames.size()));
    ImGui::Combo("Filter Type", &currentFilterType, filterTypeNames.data(),
                 static_cast<int>(filterTypeNames.size()));
    ImGui::Combo("Wrap Type", &currentWrapType, wrapTypeNames.data(),
                 static_cast<int>(wrapTypeNames.size()));
    ImGui::Checkbox("Generate Mipmaps", &generateMipmaps);

    if (ImGui::Button("Import")) {
      auto importSettings = TextureLoader::ImportSettings();
      importSettings.textureType =
          static_cast<TextureLoader::ImportSettings::TextureType>(
              currentTextureType);
      importSettings.filterType =
          static_cast<TextureLoader::ImportSettings::FilterType>(
              currentFilterType);
      importSettings.wrapType =
          static_cast<TextureLoader::ImportSettings::WrapType>(currentWrapType);
      importSettings.generateMipmaps = generateMipmaps;

      auto handleOpt = mCtx.projectManager.GetAssetImporter().ImportTexture(
          mPendingImportPath, importSettings);
      if (!handleOpt) {
        ImGui::CloseCurrentPopup();
        return;
      }
      auto metaOpt = mCtx.projectManager.GetAssetManager().GetAssetMetaByGuid(
          handleOpt->guid);
      if (!metaOpt) {
        ImGui::CloseCurrentPopup();
        return;
      }
      mCtx.projectManager.GetAssetManager().LoadTexture(*metaOpt);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}
void ProjectPanel::showModelLoadWindow() {
  if (ImGui::BeginPopupModal("Model Import", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Path: %s", mPendingImportPath.c_str());
    if (ImGui::Button("Import")) {
      auto handleOpt = mCtx.projectManager.GetAssetImporter().ImportModel(
          mPendingImportPath, ModelLoader::ImportSettings());
      if (!handleOpt) {
        ImGui::CloseCurrentPopup();
        return;
      }
      auto metaOpt = mCtx.projectManager.GetAssetManager().GetAssetMetaByGuid(
          handleOpt->guid);
      if (!metaOpt) {
        ImGui::CloseCurrentPopup();
        return;
      }
      mCtx.projectManager.GetAssetManager().LoadModel(*metaOpt);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void ProjectPanel::showFolderCreateWindow() {
  if (!ImGui::BeginPopupModal("Folder Create", nullptr,
                              ImGuiWindowFlags_AlwaysAutoResize))
    return;

  const auto &selectedFolder = mCtx.editorState.temporary.selectedFolder;

  static char buffer[64];
  ImGui::InputText("Material name", buffer, sizeof(buffer));
  std::string name = std::string(buffer);

  if (ImGui::Button("Create")) {
    std::filesystem::create_directory(selectedFolder / name);
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel")) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

} // namespace Ember
