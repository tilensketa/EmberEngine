#pragma once

#include "Core/Application.h"

extern Ember::Application *Ember::CreateApplication();

int main() {
  Ember::Application *app = Ember::CreateApplication();
  app->Run();
  app->Stop();
  delete app;
}
