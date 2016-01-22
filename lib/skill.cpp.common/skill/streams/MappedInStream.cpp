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

skill::streams::MappedInStream::MappedInStream(const skill::streams::MappedInStream *other, size_t begin, size_t end) {
    this->base = other->position + begin;
    this->position = (uint8_t *) base;
    this->end = other->position + end;
}
