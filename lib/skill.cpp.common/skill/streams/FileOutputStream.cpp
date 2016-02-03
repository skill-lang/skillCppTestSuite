//
// Created by Timm Felden on 03.02.16.
//

#include "FileOutputStream.h"

skill::streams::FileOutputStream::FileOutputStream(const std::string &path, bool append)
        : Stream(&buffer, &buffer + BUFFER_SIZE),
          path(path), file(fopen(path.c_str(), append ? "ra+" : "w")) {

}

skill::streams::FileOutputStream::~FileOutputStream() {

}

void skill::streams::FileOutputStream::flush() {

}

skill::streams::MappedOutStream *skill::streams::FileOutputStream::jumpAndMap(long offset) {
    return nullptr;
}
