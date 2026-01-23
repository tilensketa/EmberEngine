#pragma once

#include <Ember.h>

#include <memory>

namespace Ember {

class GameLayer : public ILayer {
public:
  GameLayer(ProjectManager &projectManager, std::shared_ptr<Window> window);
  ~GameLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  ProjectManager &mProjectManager;
  std::unique_ptr<RenderTarget> mRenderTarget;

  std::unique_ptr<Shader> mScreenShader;
  std::unique_ptr<VAO> mScreenVao;
  std::shared_ptr<Window> mWindow;
};
} // namespace Ember
