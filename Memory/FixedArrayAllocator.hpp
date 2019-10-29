#pragma once

#include <iostream>
#include <cstddef>

namespace wlEngine {

    template <typename T, size_t N>
    class FixedArrayAllocator {
    private:
        struct MemoryChunk {
            struct Arena {
                static_assert(sizeof(T) > sizeof(void*), "Component Class has to have a size larger than a size of a pointer");
                char bytes[sizeof(T)];
                MemoryChunk* chunk;
            }  arena[N]; //store N arena, 1 arena for 1 object of type T

            size_t freeCount = N;
            /**
             * @brief Initialize Component Allocator
             */
            MemoryChunk()  {
                size_t i = 0;
                for (; i < N - 1; i++) {
                    arena[i].chunk = this;
                    *reinterpret_cast<Arena**>(arena[i].bytes) = &arena[i+1];
                }
                arena[i].chunk = this;
                *reinterpret_cast<Arena**>(arena[i].bytes) = nullptr;
            }
            
            ~MemoryChunk() = default;
        };
    public:
        FixedArrayAllocator() {
			beacon = allocateNewChunk();
			chunkCount = 1;
        }
        
        ~FixedArrayAllocator() = default;
        
        template<typename... Args>
        T* allocate(Args&& ... params) {
			
            auto succ = *reinterpret_cast<typename MemoryChunk::Arena**>(beacon->bytes);
            auto temp = beacon;
            
            beacon->chunk->freeCount -= 1;
            if (succ)
                beacon = succ;
            else
                beacon = allocateNewChunk();

			return new (reinterpret_cast<T*>(temp)) T(params...);
        }

        void deallocate(T* ptr) {
			ptr->~T();
            auto arenaPtr = reinterpret_cast<typename MemoryChunk::Arena*>(ptr);
            auto chunk = arenaPtr->chunk;
            chunk->freeCount += 1;
            
            if (beacon->chunk->freeCount == N && chunk != beacon->chunk && chunkCount != 1) {
                chunkCount -= 1;
                delete beacon->chunk;
                beacon = nullptr;
            }
            
            *reinterpret_cast<typename MemoryChunk::Arena**>(arenaPtr->bytes) = beacon;
            beacon = arenaPtr;
        }
    private:
        typename MemoryChunk::Arena* beacon;
        size_t chunkCount;

        typename MemoryChunk::Arena* allocateNewChunk() {
            chunkCount += 1;
            auto chunk = new MemoryChunk;
            return chunk->arena;
        }
    };
}


