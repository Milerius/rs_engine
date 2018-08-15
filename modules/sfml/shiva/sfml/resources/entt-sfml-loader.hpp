//
// Created by roman Sztergbaum on 19/07/2018.
//

#pragma once

#include <fstream>
#include <SFML/Audio.hpp>
#include <entt/resource/loader.hpp>
#include <shiva/sfml/common/animation_config.hpp>

namespace shiva::sfml
{
    template <typename ResourceType>
    struct loader final : ::entt::ResourceLoader<loader<ResourceType>, ResourceType>
    {
        template <typename ... Args>
        std::shared_ptr<ResourceType> load(Args &&...args) const
        {
            auto resource_ptr = std::make_shared<ResourceType>();
            if (!resource_ptr->loadFromFile(std::forward<Args>(args)...)) {
                throw std::runtime_error("Impossible to load file");
            }
            return resource_ptr;
        }
    };

    template <>
    struct loader<sf::Music> final : ::entt::ResourceLoader<loader<sf::Music>, sf::Music>
    {
        template <typename ... Args>
        std::shared_ptr<sf::Music> load(Args &&...args) const
        {
            auto resource_ptr = std::make_shared<sf::Music>();
            if (!resource_ptr->openFromFile(std::forward<Args>(args)...)) {
                throw std::runtime_error("Impossible to load file");
            }
            return resource_ptr;
        }
    };

    template <>
    struct loader<animation_config> final : ::entt::ResourceLoader<loader<animation_config>, animation_config>
    {
        template <typename ... Args>
        std::shared_ptr<animation_config> load(const std::string& path) const
        {
            auto resource_ptr = std::make_shared<animation_config>();
            std::ifstream ifs(path);
            if (ifs.is_open()) {
                shiva::json::json j;
                ifs >> j;
                *resource_ptr = j;
            }
            return resource_ptr;
        }
    };
}
