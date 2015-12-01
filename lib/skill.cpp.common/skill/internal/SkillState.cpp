//
// Created by Timm Felden on 04.11.15.
//

#include "SkillState.h"

skill::internal::SkillState::~SkillState() {
    delete annotation;
}
