#pragma once

#include "engine/ecs/entity.hpp"
#include <cstddef>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ecs {
class World {
public:
  Entity createEntity();
  void destroyEntity(Entity e);

  template <class T, class... Args> T &add(Entity e, Args &&...args);
  template <class T> bool has(Entity e) const noexcept;
  template <class T> T &get(Entity e) noexcept;
  template <class T> const T &get(Entity e) const noexcept;
  template <class T> void remove(Entity e) noexcept;

  template <class T0, class... Ts, class Fn> void each(Fn &&fn);
  template <class T0, class... Ts, class Fn> void each(Fn &&fn) const;

  template <class T> void markOneFrame();
  void clearOneFrame();

private:
  using EntityHandle = ecs::EntityHandle;
  std::vector<std::uint32_t> generations_{};
  std::vector<EntityHandle> free_{};

  struct IStorage {
    virtual ~IStorage() = default;
    virtual void erase(EntityHandle id) = 0;
    virtual void clear() = 0;
  };

  template <class T> struct Storage : IStorage {
    std::vector<T> data{};
    std::vector<EntityHandle> dense{};
    std::vector<std::size_t> sparse{};

    void grow(std::size_t n) {
      if (sparse.size() < n)
        sparse.resize(n, npos);
    }

    void erase(EntityHandle id) override {
      if (id >= sparse.size())
        return;
      std::size_t idx = sparse[id];
      if (idx == npos)
        return;
      std::size_t last = data.size() - 1;
      sparse[id] = npos;
      if (idx != last) {
        data[idx] = std::move(data.back());
        EntityHandle moved = dense.back();
        dense[idx] = moved;
        sparse[moved] = idx;
      }
      data.pop_back();
      dense.pop_back();
    }

    void clear() override {
      for (EntityHandle id : dense) {
        if (id < sparse.size())
          sparse[id] = npos;
      }
      data.clear();
      dense.clear();
    }
  };

  template <class T> Storage<T> *storage();
  template <class T> const Storage<T> *storage() const;

  static constexpr std::size_t npos = static_cast<std::size_t>(-1);
  std::unordered_map<std::size_t, std::unique_ptr<IStorage>> storages_{};
  std::unordered_set<std::size_t> one_frame_{};
};

// --- template implementations ---

template <class T> typename World::Storage<T> *World::storage() {
  auto key = typeid(T).hash_code();
  auto it = storages_.find(key);
  if (it == storages_.end()) {
    auto ptr = std::make_unique<Storage<T>>();
    auto *raw = ptr.get();
    storages_.emplace(key, std::move(ptr));
    return raw;
  }
  return static_cast<Storage<T> *>(it->second.get());
}

template <class T> const typename World::Storage<T> *World::storage() const {
  auto key = typeid(T).hash_code();
  auto it = storages_.find(key);
  if (it == storages_.end()) {
    return nullptr;
  }
  return static_cast<const Storage<T> *>(it->second.get());
}

template <class T, class... Args> T &World::add(Entity e, Args &&...args) {
  auto *s = storage<T>();
  s->grow(generations_.size());
  auto id = e.id;
  std::size_t idx = s->sparse[id];
  if (idx == npos) {
    idx = s->data.size();
    s->data.emplace_back(std::forward<Args>(args)...);
    s->dense.push_back(id);
    s->sparse[id] = idx;
  } else {
    s->data[idx] = T(std::forward<Args>(args)...);
  }
  return s->data[idx];
}

template <class T> bool World::has(Entity e) const noexcept {
  auto *s = storage<T>();
  if (!s)
    return false;
  return e.id < s->sparse.size() && s->sparse[e.id] != npos;
}

template <class T> T &World::get(Entity e) noexcept {
  auto *s = storage<T>();
  return s->data[s->sparse[e.id]];
}

template <class T> const T &World::get(Entity e) const noexcept {
  auto const *s = storage<T>();
  return s->data[s->sparse[e.id]];
}

template <class T> void World::remove(Entity e) noexcept {
  auto *s = storage<T>();
  s->erase(e.id);
}

template <class T0, class... Ts, class Fn> void World::each(Fn &&fn) {
  auto *primary = storage<T0>();
  for (std::size_t i = 0; i < primary->dense.size(); ++i) {
    EntityHandle id = primary->dense[i];
    Entity ent{id, generations_[id]};
    if ((has<Ts>(ent) && ...)) {
      fn(ent, primary->data[i], get<Ts>(ent)...);
    }
  }
}

template <class T0, class... Ts, class Fn> void World::each(Fn &&fn) const {
  auto const *primary = storage<T0>();
  if (!primary)
    return;
  for (std::size_t i = 0; i < primary->dense.size(); ++i) {
    EntityHandle id = primary->dense[i];
    Entity ent{id, generations_[id]};
    if ((has<Ts>(ent) && ...)) {
      fn(ent, primary->data[i], get<Ts>(ent)...);
    }
  }
}

template <class T> void World::markOneFrame() { one_frame_.insert(typeid(T).hash_code()); }

} // namespace ecs
