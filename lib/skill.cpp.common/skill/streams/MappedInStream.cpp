//
// Created by Timm Felden on 04.11.15.
//

#include "MappedInStream.h"

skill::streams::MappedInStream::MappedInStream(
        const void *base, uint8_t *position, const void *end) {
    this->base = base;
    this->position = position;
    this->end = end;
}
