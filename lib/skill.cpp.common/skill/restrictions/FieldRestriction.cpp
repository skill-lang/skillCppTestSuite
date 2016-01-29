//
// Created by Timm Felden on 24.11.15.
//

#include "FieldRestriction.h"

skill::restrictions::FieldRestriction::~FieldRestriction() {

}

const skill::restrictions::NonNull skill::restrictions::NonNull::instance;

const skill::restrictions::NonNull *skill::restrictions::NonNull::get() {
    return &instance;
}

const skill::restrictions::ConstantLengthPointer skill::restrictions::ConstantLengthPointer::instance;

const skill::restrictions::ConstantLengthPointer *skill::restrictions::ConstantLengthPointer::get() {
    return &instance;
}
