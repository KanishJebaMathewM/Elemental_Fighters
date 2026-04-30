cmake_minimum_required(VERSION 3.15)
project(ElementalFighters)

set(CMAKE_CXX_STANDARD 17)

# Attempt to find SFML. If SFML is not installed globally, you will need to pass the directory:
# cmake -DSFML_DIR="C:/path/to/sfml/lib/cmake/SFML" ..
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ElementalFighters
    main.cpp
    Game.cpp
    Fighter.cpp
    Attack.cpp
    ParticleSystem.cpp
    CartoonRenderer.cpp
)

target_link_libraries(ElementalFighters PRIVATE sfml-graphics sfml-window sfml-system)
