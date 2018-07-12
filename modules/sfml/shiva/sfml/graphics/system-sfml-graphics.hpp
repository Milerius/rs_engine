//
// Created by roman Sztergbaum on 15/06/2018.
//

#pragma once

#include <SFML/Graphics.hpp>
#include <shiva/entt_shared/entt_shared.hpp>
#include <shiva/ecs/system.hpp>
#include <shiva/event/after_load_systems_plugins.hpp>
#include <shiva/sfml/event/set_render_window.hpp>

namespace shiva::plugins
{
    class render_system : public shiva::ecs::post_update_system<render_system>
    {
    public:
        ~render_system() noexcept override = default;

        render_system(shiva::entt::dispatcher& dispatcher, shiva::entt::entity_registry &registry, const float& fixed_delta_time) noexcept :
        system(dispatcher, registry, fixed_delta_time)
        {
            user_data_ = &win_;
            shiva::entt_shared::init_library(entity_registry_, dispatcher_);
        }

        static std::unique_ptr<shiva::ecs::base_system> system_creator(entt::dispatcher &dispatcher,
                                                                       entt::entity_registry &registry,
                                                                       const float& fixed_delta_time) noexcept;
        void update() noexcept final;


    public:
        reflect_class(render_system)

        static constexpr auto reflected_functions() noexcept
        {
            return meta::makeMap(reflect_function(&render_system::update));
        }

        static constexpr auto reflected_members() noexcept
        {
            return meta::makeMap();
        }

    private:
        //TODO: Parse a config file
        sf::RenderWindow win_{sf::VideoMode(800, 600), "SFML window"};
    };
}