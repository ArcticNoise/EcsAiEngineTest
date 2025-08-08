#include "engine/ecs/world.hpp"

namespace ecs {
Entity World::createEntity() {
  EntityHandle id{};
  if (!free_.empty()) {
    id = free_.back();
    free_.pop_back();
  } else {
    id = static_cast<EntityHandle>(generations_.size());
    generations_.push_back(0);
  }
  return Entity{id, generations_[id]};
}

void World::destroyEntity(Entity e) {
  if (e.id >= generations_.size() || generations_[e.id] != e.gen)
    return;
  for (auto &[_, storage] : storages_) {
    storage->erase(e.id);
  }
  ++generations_[e.id];
  free_.push_back(e.id);
}

void World::clearOneFrame() {
  for (auto h : one_frame_) {
    auto it = storages_.find(h);
    if (it != storages_.end()) {
      it->second->clear();
    }
  }
}
} // namespace ecs
