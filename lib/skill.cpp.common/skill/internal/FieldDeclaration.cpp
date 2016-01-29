//
// Created by Timm Felden on 20.11.15.
//

#include "FieldDeclaration.h"

void skill::internal::FieldDeclaration::addRestriction(const skill::restrictions::FieldRestriction *r) {
    if (dynamic_cast<const restrictions::CheckableRestriction *>(r))
        checkedRestrictions.insert((restrictions::CheckableRestriction *) r);
    else
        otherRestrictions.insert(r);
}

