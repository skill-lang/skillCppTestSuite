//
// Created by Timm Felden on 20.11.15.
//
// This file contains file structure information, i.e. Blocks and Chunks
//

#ifndef SKILL_CPP_COMMON_FILESTRUCTURE_H
#define SKILL_CPP_COMMON_FILESTRUCTURE_H

#include "../common.h"
#include <cstddef>

namespace skill {
    namespace internal {

/**
 * A block contains information about instances in a type. A StoragePool holds
 * blocks in order of appearance in a file with the invariant, that the latest
 * block in the list will be the latest block in the file. If a StoragePool
 * holds no block, then it has no instances in a file.
 *
 * @author Timm Felden
 * @note While writing a Pool to disk, the latest block is the block currently
 *       written.
 */
        struct Block {
            Block(int blockIndex, SKilLID bpo, SKilLID staticCount, SKilLID dynamicCount)
                    : blockIndex(blockIndex), bpo(bpo), staticCount(staticCount),
                      dynamicCount(dynamicCount) { }

            //! the index of the block inside of the file, starting from 0
            const int blockIndex;
            //! absolute offset relative to base pools first index, because this is
            //! required almost always
            const SKilLID bpo;

            /**
             * is a var, because the number of static instances can only be known after
             * the sub type has been read
             */
            SKilLID staticCount;
            const SKilLID dynamicCount;
        };


        /**
         * Chunks contain information on where field data can be found.
         *
         * @author Timm Felden
         * @note indices of recipient of the field data is not necessarily continuous;
         *       make use of staticInstances!
         * @note in contrast to other implementations, the offset is relative, because read will use a buffer over the whole
         * data segment, instead of buffering each segment itself
         */
        struct Chunk {
            Chunk(size_t begin, size_t end, SKilLID count)
                    : begin(begin), end(end), count(count) { }

            virtual ~Chunk() { }

            size_t begin;
            size_t end;
            const SKilLID count;
        };

/**
 * A chunk used for regular appearances of fields.
 *
 * @author Timm Felden
 */
        struct SimpleChunk : public Chunk {

            SimpleChunk(size_t begin, size_t end, SKilLID count, const SKilLID bpo)
                    : Chunk(begin, end, count), bpo(bpo) { }

            virtual ~SimpleChunk() { }

            const SKilLID bpo;
        };

/**
 * A chunk that is used iff a field is appended to a preexisting type in a
 * block.
 *
 * @author Timm Felden
 *
 * @param blockCount the number of blocks that have to be processed
 */
        struct BulkChunk : public Chunk {
            BulkChunk(size_t begin, size_t end, SKilLID count, const int blockCount)
                    : Chunk(begin, end, count), blockCount(blockCount) { }

            virtual ~BulkChunk() { }

            const int blockCount;
        };


    }
}

#endif //SKILL_CPP_COMMON_FILESTRUCTURE_H
