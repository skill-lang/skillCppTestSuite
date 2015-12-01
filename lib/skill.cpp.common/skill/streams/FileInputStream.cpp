//
// Created by feldentm on 03.11.15.
//

#include <sys/stat.h>
#include <sys/mman.h>
#include "FileInputStream.h"

using namespace skill::streams;

FileInputStream::FileInputStream(const std::string path)
        : path(path), file(fopen(path.c_str(), "r")) {
    if (nullptr == file)
        throw SkillException(std::string("could not open file ") + path);

    FILE *stream = (FILE *) file;

    struct stat fileStat;
    if (-1 == fstat(fileno(stream), &fileStat))
        throw SkillException("Execution of function fstat failed.");

    size_t const length = fileStat.st_size;

    if (!length) {
        base = position = nullptr;
        end = nullptr;
        return;
    }

    base = position = (uint8_t *) mmap(NULL, length, PROT_READ, MAP_SHARED, fileno(stream), 0);
    end = position + length;

    if (MAP_FAILED == base)
        throw SkillException("Execution of function mmap failed.");

    if (-1 == posix_madvise(position, length, MADV_WILLNEED))
        throw SkillException("Execution of function madvise failed.");
}

FileInputStream::~FileInputStream() {
    if (nullptr != file) {
        fclose((FILE *) file);

        if (nullptr != base)
            munmap((void *) base, (size_t) end - (size_t) base);
    }
}
