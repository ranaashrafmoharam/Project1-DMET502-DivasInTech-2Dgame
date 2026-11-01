#pragma once
// ====================================================================
// FILE:     src/entities/CharacterModels.h
// Builds distinct multi-part character models.
// ====================================================================

#include "HierarchicalModel.h"

class CharacterModels {
public:
    static CharacterModels& instance();

    HierarchicalModel createKaiModel();
    HierarchicalModel createZaneModel();
    HierarchicalModel createColeModel();
    HierarchicalModel createVenomariModel();
    HierarchicalModel createHypnobraiModel();
    HierarchicalModel createConstrictaiModel();

private:
    CharacterModels() = default;
};
