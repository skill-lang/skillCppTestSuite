//
// Created by Timm Felden on 20.11.15.
//

#include "AbstractField.h"

using namespace skill::api;

AbstractField::~AbstractField() {
    if (typeRequiresDestruction)
        delete type;
}
