#pragma once

#include "Core/Events/IEvent.h"
#include "Project/ProjectManager.h"

namespace Ember {
namespace Event {

struct ProjectOpen : public IEvent {
  ProjectDirectories directories;
  ProjectOpen(ProjectDirectories directories) : directories(directories) {}
  EventType GetType() const override { return EventType::PROJECT_OPEN; }
  std::string ToString() const override { return "PROJECT_OPEN"; }
};
struct ProjectSave : public IEvent {
  ProjectSave() {}
  EventType GetType() const override { return EventType::PROJECT_SAVE; }
  std::string ToString() const override { return "PROJECT_SAVE"; }
};
struct ProjectCreate : public IEvent {
  ProjectCreate() {}
  EventType GetType() const override { return EventType::PROJECT_CREATE; }
  std::string ToString() const override { return "PROJECT_CREATE"; }
};
struct ProjectClose : public IEvent {
  ProjectClose() {}
  EventType GetType() const override { return EventType::PROJECT_CLOSE; }
  std::string ToString() const override { return "PROJECT_CLOSE"; }
};

} // namespace Event
} // namespace Ember
