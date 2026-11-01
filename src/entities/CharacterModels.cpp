#include "CharacterModels.h"

CharacterModels& CharacterModels::instance() {
    static CharacterModels cm;
    return cm;
}

HierarchicalModel CharacterModels::createKaiModel() {
    HierarchicalModel model;
    ModelPart torso{"torso", {0, 30}, 0.0f, {28, 36}, {0.85f, 0.20f, 0.20f, 1.0f}};
    ModelPart head{"head", {0, 24}, 0.0f, {20, 20}, {0.95f, 0.75f, 0.60f, 1.0f}};
    torso.children.push_back(head);
    model.setRoot(torso);
    return model;
}

HierarchicalModel CharacterModels::createZaneModel() {
    HierarchicalModel model;
    ModelPart torso{"torso", {0, 30}, 0.0f, {28, 36}, {0.90f, 0.95f, 1.0f, 1.0f}};
    ModelPart head{"head", {0, 24}, 0.0f, {20, 20}, {0.95f, 0.75f, 0.60f, 1.0f}};
    torso.children.push_back(head);
    model.setRoot(torso);
    return model;
}

HierarchicalModel CharacterModels::createColeModel() {
    HierarchicalModel model;
    ModelPart torso{"torso", {0, 30}, 0.0f, {34, 40}, {0.20f, 0.18f, 0.18f, 1.0f}};
    ModelPart head{"head", {0, 26}, 0.0f, {22, 22}, {0.95f, 0.75f, 0.60f, 1.0f}};
    torso.children.push_back(head);
    model.setRoot(torso);
    return model;
}

HierarchicalModel CharacterModels::createVenomariModel() {
    HierarchicalModel model;
    ModelPart torso{"torso", {0, 30}, 0.0f, {28, 36}, {0.20f, 0.75f, 0.30f, 1.0f}};
    model.setRoot(torso);
    return model;
}

HierarchicalModel CharacterModels::createHypnobraiModel() {
    HierarchicalModel model;
    ModelPart torso{"torso", {0, 30}, 0.0f, {28, 36}, {0.25f, 0.35f, 0.85f, 1.0f}};
    model.setRoot(torso);
    return model;
}

HierarchicalModel CharacterModels::createConstrictaiModel() {
    HierarchicalModel model;
    ModelPart torso{"torso", {0, 30}, 0.0f, {36, 42}, {0.85f, 0.45f, 0.15f, 1.0f}};
    model.setRoot(torso);
    return model;
}
