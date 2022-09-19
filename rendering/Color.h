//
// Created by Jonas on 17/09/2022.
//

#ifndef OPTIXMESHCORE_COLOR_H
#define OPTIXMESHCORE_COLOR_H

#include <glm/glm.hpp>

class Color: public glm::vec4 {
    using glm::vec4::vec4;
public:
    // Define some colors as static public
    [[maybe_unused]] static Color Red(){return {1.0f, 0.0f, 0.0f, 1.0f};}
    [[maybe_unused]] static Color Green(){return {0.0f, 1.0f, 0.0f, 1.0f};}
    [[maybe_unused]] static Color Blue(){return {0.0f, 0.0f, 1.0f, 1.0f};}
    [[maybe_unused]] static Color White(){return {1.0f, 1.0f, 1.0f, 1.0f};}
    [[maybe_unused]] static Color Black(){return {0.0f, 0.0f, 0.0f, 1.0f};}
    [[maybe_unused]] static Color Yellow(){return {1.0f, 1.0f, 0.0f, 1.0f};}
    [[maybe_unused]] static Color Cyan(){return {0.0f, 1.0f, 1.0f, 1.0f};}
    [[maybe_unused]] static Color Magenta(){return {1.0f, 0.0f, 1.0f, 1.0f};}
    [[maybe_unused]] static Color Orange(){return {1.0f, 0.5f, 0.0f, 1.0f};}

};

#endif //OPTIXMESHCORE_COLOR_H
