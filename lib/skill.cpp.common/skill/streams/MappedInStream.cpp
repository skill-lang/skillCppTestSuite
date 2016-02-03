//
// Created by Timm Felden on 04.11.15.
//

#include "MappedInStream.h"

skill::streams::MappedInStream::MappedInStream(
        void *base, uint8_t *position, void *end)
        : InStream(base, end) {
    this->position = position;
}

skill::streams::MappedInStream::MappedInStream(const skill::streams::MappedInStream *other, size_t begin, size_t end)
        : InStream(other->position + begin, other->position + end) {
    this->position = (uint8_t *) base;
}
