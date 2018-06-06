//
// Created by feldentm on 03.11.15.
//

#include <sys/stat.h>
#include <sys/mman.h>
#include "FileInputStream.h"

using namespace skill::streams;


FileInputStream::FileInputStream(void *begin, void *end, const std::string* path, const FILE *file)
        : InStream(begin, end), path(path), file(file) {
}


FileInputStream::FileInputStream(const std::string& path, const char *openMode)
        : InStream(nullptr, nullptr), path(new std::string(path)), file(nullptr) {
    FILE *stream = fopen(path.c_str(), openMode);

    if (nullptr == stream)
        throw SkillException(std::string("could not open file ") + path);

    if (openMode[0] != 'r')
        return; // do not perform any operations on a file that is not used anyway

    struct stat fileStat;
    if (-1 == fstat(fileno(stream), &fileStat))
        throw SkillException("Execution of function fstat failed.");

    size_t const length = fileStat.st_size;

    if (!length) {
        return;
    }

    void *base = position = (uint8_t *) mmap(NULL, length, PROT_READ, MAP_SHARED, fileno(stream), 0);
    void *end = position + length;

    if (MAP_FAILED == base)
        throw SkillException("Execution of function mmap failed.");

    if (-1 == posix_madvise(position, length, MADV_WILLNEED))
        throw SkillException("Execution of function madvise failed.");

    // set begin and end
    new(this) FileInputStream(base, end, this->path.get(), stream);
}

FileInputStream::~FileInputStream() {
    if (nullptr != file) {
        fclose((FILE *) file);

        if (nullptr != base)
            munmap((void *) base, (size_t) end - (size_t) base);
    }
}
