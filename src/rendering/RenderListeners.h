//
// Created by tolle on 6/09/2023.
//

#ifndef EXTENDEDMESHCORE_RENDERLISTENERS_H
#define EXTENDEDMESHCORE_RENDERLISTENERS_H

#include "meshcore/core/Transformation.h"

class AbstractRenderModelListener{
public:
    virtual void notify() const = 0;
    virtual void notifyNameChanged(const std::string& oldName, const std::string& newName) const = 0;
    virtual void notifyMaterialChanged(const PhongMaterial& oldMaterial, const PhongMaterial& newMaterial) const = 0;
    virtual void notifyVisibleChanged(bool oldVisible, bool newVisible) const = 0;
    virtual void notifyTransformationChanged(const Transformation& oldTransformation, const Transformation& newTransformation) const = 0;
};

class SimpleRenderModelListener: public AbstractRenderModelListener {
private:
    std::function<void()> onChanged = {};
    std::function<void(const std::string& oldName, const std::string& newName)> onNameChanged = {};
    std::function<void(const PhongMaterial& oldMaterial, const PhongMaterial& newMaterial)> onMaterialChanged = {};
    std::function<void(bool oldVisible, bool newVisible)> onVisibleChanged = {};
    std::function<void(const Transformation& oldTransformation, const Transformation& newTransformation)> onTransformationChanged = {};

    void notify() const override{
        if(this->onChanged) this->onChanged();
    }

    void notifyMaterialChanged(const PhongMaterial& oldMaterial, const PhongMaterial& newMaterial) const override {
        if(this->onMaterialChanged) this->onMaterialChanged(oldMaterial, newMaterial);
        if(this->onChanged) this->onChanged();
    }

    void notifyVisibleChanged(bool oldVisible, bool newVisible) const override {
        if(this->onVisibleChanged) this->onVisibleChanged(oldVisible, newVisible);
        if(this->onChanged) this->onChanged();
    }

    void notifyNameChanged(const std::string& oldName, const std::string& newName) const override {
        if(this->onNameChanged) this->onNameChanged(oldName, newName);
        if(this->onChanged) this->onChanged();
    }

    void notifyTransformationChanged(const Transformation& oldTransformation, const Transformation& newTransformation) const override {
        if(this->onTransformationChanged) this->onTransformationChanged(oldTransformation, newTransformation);
        if(this->onChanged) this->onChanged();
    }

public:

    [[maybe_unused]] void setOnChanged(const std::function<void()> &newOnChanged) {
        this->onChanged = newOnChanged;
    }

    [[maybe_unused]] void setOnMaterialChanged(const std::function<void(const PhongMaterial& oldMaterial, const PhongMaterial& newMaterial)> &newOnMaterialChanged) {
        this->onMaterialChanged = newOnMaterialChanged;
    }

    [[maybe_unused]] void setOnVisibleChanged(const std::function<void(bool oldVisible, bool newVisible)> &newOnVisibleChanged) {
        this->onVisibleChanged = newOnVisibleChanged;
    }

    [[maybe_unused]] void setOnNameChanged(const std::function<void(const std::string& oldName, const std::string& newName)> &newOnNameChanged) {
        this->onNameChanged = newOnNameChanged;
    }

    [[maybe_unused]] void setOnTransformationChanged(const std::function<void(const Transformation& oldTransformation, const Transformation& newTransformation)> &newOnTransformationChanged) {
        this->onTransformationChanged = newOnTransformationChanged;
    }
};

class AbstractRenderGroupListener {
public:
    virtual void notifyVisible(bool visible, bool uniform) const = 0;
};

class SimpleRenderGroupListener: public AbstractRenderGroupListener {
private:
    std::function<void(bool visible, bool uniform)> onVisibleChanged = {};

    void notifyVisible(bool visible, bool uniform) const override {
        if(this->onVisibleChanged) this->onVisibleChanged(visible, uniform);
    }

public:
    [[maybe_unused]] void setOnVisibleChanged(const std::function<void(bool visible, bool uniform)> &newOnVisibleChanged) {
        this->onVisibleChanged = newOnVisibleChanged;
    }
};

#endif //EXTENDEDMESHCORE_RENDERLISTENERS_H
