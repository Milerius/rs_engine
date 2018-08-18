//
// Created by roman Sztergbaum on 30/05/2018.
//

#include <gtest/gtest.h>
#include <entt/signal/dispatcher.hpp>
#include <shiva/reflection/reflection.hpp>
#include <shiva/meta/list.hpp>
#include <shiva/meta/tuple_for_each.hpp>
#include <shiva/ecs/ecs.hpp>
#include <shiva/world/world.hpp>
#include "plugins/system_plugged_bar.hpp"

template <typename system_type>
class FakeSystem
{
public:
    template <typename T>
    using is_same_system = std::is_same<system_type, T>;

    FakeSystem()
    {
        using is_same_list = shiva::meta::list::Transform<shiva::ecs::details::valid_systems_list, is_same_system>;
        static_assert(shiva::ecs::details::is_valid_system_v(is_same_list{}));
    }
};

class fixture_system : public ::shiva::world, public ::testing::Test
{
protected:
    fixture_system() : shiva::world(shiva::fs::path("systems_test"))
    {
    }

    void SetUp() override
    {
        spdlog::set_pattern("[%n][%r][pid: %P][%^%l%$]: %v");
        dispatcher_.trigger<shiva::event::start_game>();
        entity_registry_.create();
    }

    void TearDown() override
    {
        spdlog::drop_all();
    }
};

class test_system : public shiva::ecs::post_update_system<test_system>
{
public:
    reflect_class(test_system)

    test_system(shiva::entt::dispatcher &dispatcher, shiva::entt::entity_registry &registry,
                const float &fixed_delta_time) noexcept :
        system(dispatcher, registry, fixed_delta_time)
    {
    }

    void update() noexcept override
    {
    }
};

class another_test_system : public shiva::ecs::pre_update_system<another_test_system>
{
public:
    reflect_class(another_test_system)

    another_test_system(shiva::entt::dispatcher &dispatcher, shiva::entt::entity_registry &registry,
                        const float &fixed_delta_time) noexcept :
        system(dispatcher, registry, fixed_delta_time)
    {
    }

    void update() noexcept override
    {
    }
};

class third_test_system : public shiva::ecs::logic_update_system<third_test_system>
{
public:
    reflect_class(third_test_system)

    third_test_system(shiva::entt::dispatcher &dispatcher,
                      shiva::entt::entity_registry &registry,
                      const float &fixed_delta_time) noexcept:
        system(dispatcher, registry, fixed_delta_time)
    {
    }

    void update() noexcept override
    {
    }
};

class fourth_test_system : public shiva::ecs::pre_update_system<fourth_test_system>
{

public:
    reflect_class(fourth_test_system)

    fourth_test_system(shiva::entt::dispatcher &dispatcher,
                       shiva::entt::entity_registry &registry,
                       const float &fixed_delta_time) :
        system(dispatcher, registry, fixed_delta_time)
    {
    }

    void update() noexcept override
    {
    }
};

TEST(ecs_testing, constructor)
{
    entt::Dispatcher dispatcher{};
    shiva::entt::entity_registry registry{};
    shiva::helpers::plugins_registry<shiva::ecs::system_manager::pluginapi_create_t> plugins(shiva::fs::path("systems"),
                                                                                             "shiva-system");
    shiva::ecs::system_manager manager(dispatcher, registry, plugins);
    spdlog::drop_all();
    ASSERT_EQ(1, 1);
}

TEST(ecs_testing, type_traits)
{
    [[maybe_unused]] FakeSystem<shiva::ecs::system_logic_update> s;
}

TEST_F(fixture_system, add_simple_system)
{
    auto system = system_manager_.create_system<test_system>();
    ASSERT_EQ(system_manager_.nb_systems(), 1u);
    ASSERT_TRUE(system_manager_.has_system<test_system>());
    system.update();
}

TEST_F(fixture_system, load_multiple_systems)
{
    [[maybe_unused]] auto[test_sys_nc, another_test_sys_nc] = system_manager_.load_systems<test_system, another_test_system>();
    ASSERT_EQ(system_manager_.nb_systems(), 2u);
    bool result = system_manager_.has_systems<test_system, another_test_system>();
    ASSERT_TRUE(result);
}

TEST_F(fixture_system, has_system)
{
    auto system = system_manager_.create_system<test_system>();
    ASSERT_TRUE(system_manager_.has_system<test_system>());
    ASSERT_FALSE(system_manager_.has_system<another_test_system>());
}

TEST_F(fixture_system, has_systems)
{
    system_manager_.load_systems<test_system, another_test_system>();
    ASSERT_TRUE(system_manager_.has_systems<test_system>());
    ASSERT_FALSE(system_manager_.has_systems<third_test_system>());
    bool result = system_manager_.has_systems<test_system, another_test_system>();
    ASSERT_TRUE(result);
    result = system_manager_.has_systems<test_system, another_test_system, third_test_system>();
    ASSERT_FALSE(result);
}

TEST_F(fixture_system, get_simple_system)
{
    system_manager_.create_system<another_test_system>();

    system_manager_.get_system<another_test_system>();
    [[maybe_unused]] const auto &sys = system_manager_.get_system<another_test_system>();

    system_manager_.get_system<test_system>();
    system_manager_.get_system<fourth_test_system>();
}

TEST_F(fixture_system, const_get_simple_system)
{
    system_manager_.load_systems<test_system, another_test_system>();
    const shiva::ecs::system_manager &mgr = system_manager_;
    mgr.get_system<another_test_system>();
    [[maybe_unused]] const auto &sys = mgr.get_system<another_test_system>();
    mgr.get_system<third_test_system>();
    mgr.get_system<fourth_test_system>();
}

TEST_F(fixture_system, get_multiple_systems)
{
    system_manager_.create_system<test_system>();
    system_manager_.create_system<another_test_system>();
    [[maybe_unused]] const auto&[test_sys, another_test_sys] = system_manager_.get_systems<test_system, another_test_system>();
    [[maybe_unused]] auto[test_sys_nc, another_test_sys_nc] = system_manager_.get_systems<test_system, another_test_system>();
}

TEST_F(fixture_system, enable_single_system)
{
    auto &system = system_manager_.create_system<test_system>();
    system.disable();
    ASSERT_FALSE(system.is_enabled());
    system_manager_.enable_system<test_system>();
    ASSERT_TRUE(system.is_enabled());
}

TEST_F(fixture_system, enable_multiple_systems)
{
    auto systems = system_manager_.load_systems<test_system, another_test_system>();
    shiva::meta::tuple_for_each(systems, [](auto &sys) {
        sys.disable();
        ASSERT_FALSE(sys.is_enabled());
    });

    bool res = this->system_manager_.enable_systems<test_system, another_test_system>();
    ASSERT_TRUE(res);
}

TEST_F(fixture_system, disable_single_system)
{
    auto &system = system_manager_.create_system<test_system>();

    ASSERT_TRUE(system.is_enabled());
    ASSERT_EQ(system_manager_.update(), 1u);
    ASSERT_TRUE(system_manager_.disable_system<test_system>());
    ASSERT_EQ(system_manager_.update(), 0u);
    ASSERT_FALSE(system.is_enabled());
}

TEST_F(fixture_system, disable_multiple_systems)
{
    auto systems = system_manager_.load_systems<test_system, another_test_system>();

    shiva::meta::tuple_for_each(systems, [](auto &sys) {
        ASSERT_TRUE(sys.is_enabled());
    });

    ASSERT_EQ(system_manager_.update(), 2u);
    bool res = system_manager_.disable_systems<test_system, another_test_system>();
    ASSERT_TRUE(res);
    ASSERT_EQ(system_manager_.update(), 0u);
}

TEST_F(fixture_system, size)
{
    system_manager_.load_systems<test_system, another_test_system>();
    ASSERT_EQ(system_manager_.nb_systems(), 2u);
}

TEST_F(fixture_system, size_per_system_type)
{
    system_manager_.load_systems<test_system>();
    ASSERT_NE(1, system_manager_.nb_systems(shiva::ecs::pre_update));
    ASSERT_EQ(1, system_manager_.nb_systems(shiva::ecs::post_update));
}

#ifndef _WIN32

#if !defined(__EMSCRIPTEN__)

TEST_F(fixture_system, load_plugins_from_non_existent_directory)
{
    shiva::fs::copy("systems_test", "save", shiva::fs::copy_options::recursive);
    shiva::fs::remove_all("systems_test");
    ASSERT_FALSE(system_manager_.load_plugins());
    shiva::fs::copy("save", "systems_test", shiva::fs::copy_options::recursive);
    shiva::fs::remove_all("save");
}

#endif

TEST_F(fixture_system, fake_plugin)
{
    std::ofstream outfile("systems_test/libshiva-system-test.so");
    ASSERT_FALSE(system_manager_.load_plugins());
    shiva::fs::remove("systems_test/libshiva-system-test.so");
}

TEST_F(fixture_system, plugin_not_regular_file)
{
    std::ofstream outfile("systems_test/bidule.txt");
    shiva::fs::create_symlink("systems_test/bidule.txt", "systems_test/symlink");
    ASSERT_TRUE(system_manager_.load_plugins());
    shiva::fs::remove("systems_test/bidule.txt");
    shiva::fs::remove("systems_test/symlink");
}

TEST_F(fixture_system, plugin_regular_file)
{
    std::ofstream outfile("systems_test/regular_file.txt");
    ASSERT_TRUE(system_manager_.load_plugins());
    shiva::fs::remove("systems_test/regular_file.txt");
}

#endif

TEST_F(fixture_system, plugins)
{
    ASSERT_TRUE(system_manager_.load_plugins());
    ASSERT_EQ(system_manager_.nb_systems(), 1u);
}

TEST_F(fixture_system, remove_system)
{
    system_manager_.load_systems<test_system, another_test_system>();
    ASSERT_EQ(system_manager_.nb_systems(), 2u);
    ASSERT_TRUE(system_manager_.mark_system<test_system>());
    ASSERT_TRUE(system_manager_.get_system<test_system>().is_marked());
    ASSERT_EQ(system_manager_.update(), 2u);
    ASSERT_EQ(system_manager_.nb_systems(), 1u);
    ASSERT_EQ(system_manager_.update(), 1u);
}

TEST_F(fixture_system, remove_multiple_systems)
{
    system_manager_.load_systems<test_system, another_test_system>();
    ASSERT_EQ(system_manager_.nb_systems(), 2u);
    bool res = system_manager_.mark_systems<test_system, another_test_system>();
    ASSERT_TRUE(res);
    ASSERT_EQ(system_manager_.update(), 2u);
    ASSERT_EQ(system_manager_.nb_systems(), 0u);
    ASSERT_EQ(system_manager_.update(), 0u);
}

TEST_F(fixture_system, remove_unexistent_system)
{
    ASSERT_FALSE(system_manager_.mark_system<third_test_system>());
}

TEST_F(fixture_system, enable_unexistent_system)
{
    ASSERT_FALSE(system_manager_.enable_system<third_test_system>());
}

TEST_F(fixture_system, disable_unexistent_system)
{
    ASSERT_FALSE(system_manager_.disable_system<third_test_system>());
}

TEST_F(fixture_system, remove_plugged_system)
{
    ASSERT_TRUE(system_manager_.load_plugins());
    ASSERT_EQ(system_manager_.nb_systems(), 1u);
    system_manager_.mark_system<shiva::testing::plugins::bar_system>();
    ASSERT_EQ(system_manager_.update(), 1u);
    ASSERT_EQ(system_manager_.update(), 0u);
    ASSERT_EQ(system_manager_.nb_systems(), 0u);
}

TEST_F(fixture_system, logic_system)
{
    system_manager_.load_systems<third_test_system>();
    size_t accumulator = 0;
    for (size_t idx = 0; idx < 60000; ++idx) {
        accumulator += system_manager_.update();
    }
    ASSERT_GE(accumulator, 0u);
}

TEST_F(fixture_system, prioritize_systems)
{
    system_manager_.load_systems<another_test_system, fourth_test_system>();
    ASSERT_TRUE(system_manager_.prioritize_system(fourth_test_system::class_name(),
        another_test_system::class_name(),
        another_test_system::get_system_type()));

}