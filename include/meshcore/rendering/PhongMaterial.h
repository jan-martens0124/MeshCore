//
// Created by Jonas on 2/04/2024.
//

#ifndef EXTENDEDMESHCORE_PHONGMATERIAL_H
#define EXTENDEDMESHCORE_PHONGMATERIAL_H

#include "../core/Color.h"

class PhongMaterial{

    Color diffuseColor = Color(0.0f);
    Color specularColor = Color(0.0f);

public:

    PhongMaterial() = default;

    explicit PhongMaterial(const Color& diffuseColor){
        this->diffuseColor = diffuseColor;
        this->specularColor = Color(0.0f);
    }

    explicit PhongMaterial(const Color& diffuseColor, const Color& specularColor){
        this->diffuseColor = diffuseColor;
        this->specularColor = specularColor;
    }

    [[nodiscard]] const Color &getSpecularColor() const{
        return specularColor;
    }

    [[nodiscard]] const Color &getDiffuseColor() const{
        return diffuseColor;
    }

    static PhongMaterial interpolate(const PhongMaterial& a, const PhongMaterial& b, float t){
        PhongMaterial returnObject;

        // When interpolating, we interpolate meaningful components separately
        returnObject.diffuseColor = Color((1.0f - t) * a.diffuseColor + t * b.diffuseColor);
        returnObject.specularColor = Color((1.0f - t) * a.specularColor + t * b.specularColor);
        return returnObject;
    }
};

#endif //EXTENDEDMESHCORE_PHONGMATERIAL_H
