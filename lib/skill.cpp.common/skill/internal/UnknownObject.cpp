//
// Created by Timm Felden on 24.11.15.
//

#include "UnknownObject.h"
#include "AbstractStoragePool.h"

const char *skill::internal::UnknownObject::skillName() const {
    return owner->name->c_str();
}
