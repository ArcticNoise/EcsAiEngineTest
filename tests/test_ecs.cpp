#include "engine/ecs/world.hpp"
#include <catch2/catch_test_macros.hpp>

struct PositionComponent {
  int value;
};

struct VelocityComponent {
  int value;
};

struct EventComponent {
  int value;
};

TEST_CASE("entity lifecycle") {
  ecs::World world;
  auto e = world.createEntity();
  world.add<PositionComponent>(e, PositionComponent{1});
  REQUIRE(world.has<PositionComponent>(e));
  auto &p = world.get<PositionComponent>(e);
  REQUIRE(p.value == 1);
  world.destroyEntity(e);
  REQUIRE_FALSE(world.has<PositionComponent>({e.id, e.gen}));
}

TEST_CASE("iteration over components") {
  ecs::World world;
  auto e1 = world.createEntity();
  auto e2 = world.createEntity();
  world.add<PositionComponent>(e1, PositionComponent{1});
  world.add<PositionComponent>(e2, PositionComponent{2});
  world.add<VelocityComponent>(e2, VelocityComponent{3});
  int sum = 0;
  world.each<PositionComponent>([&](ecs::Entity /*ent*/, PositionComponent &p) { sum += p.value; });
  REQUIRE(sum == 3);
}

TEST_CASE("one-frame components are cleared") {
  ecs::World world;
  world.markOneFrame<EventComponent>();
  auto e = world.createEntity();
  world.add<EventComponent>(e, EventComponent{5});
  REQUIRE(world.has<EventComponent>(e));
  world.clearOneFrame();
  REQUIRE_FALSE(world.has<EventComponent>(e));
}

TEST_CASE("remove component") {
  ecs::World world;
  auto e = world.createEntity();
  world.add<PositionComponent>(e, PositionComponent{1});
  world.remove<PositionComponent>(e);
  REQUIRE_FALSE(world.has<PositionComponent>(e));
}

TEST_CASE("const iteration") {
  ecs::World world;
  auto e = world.createEntity();
  world.add<PositionComponent>(e, PositionComponent{4});
  const ecs::World &cworld = world;
  int sum = 0;
  cworld.each<PositionComponent>(
      [&](ecs::Entity /*ent*/, const PositionComponent &p) { sum += p.value; });
  REQUIRE(sum == 4);
}
