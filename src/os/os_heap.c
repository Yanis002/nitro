#include "nitro/os.h"

#define ROUND_UP_32(x) (((u32) (x) + 0x1f) & ~0x1f)
#define ROUND_DOWN_32(x) ((u32) (x) & ~0x1f)
#define MOD_32(x) ((u32) (x) & 0x1f)

#define MIN_BLOCK_SIZE ROUND_UP_32(sizeof(OSMemoryBlock) + 0x20)

static OSAlloc *sAllocList[OS_ARENA_COUNT];

OSMemoryBlock *OS_AddOccupiedMemoryBlock(OSMemoryBlock *head, OSMemoryBlock *block) {
    block->next = head;
    block->prev = NULL;
    if (head != NULL) {
        head->prev = block;
    }
    return block;
}

OSMemoryBlock *OS_RemoveMemoryBlock(OSMemoryBlock *head, OSMemoryBlock *block) {
    if (block->next != NULL) {
        block->next->prev = block->prev;
    }
    if (block->prev == NULL) {
        return block->next;
    } else {
        block->prev->next = block->next;
        return head;
    }
}

OSMemoryBlock *OS_AddFreeMemoryBlock(OSMemoryBlock *head, OSMemoryBlock *block) {
    OSMemoryBlock *prev;
    OSMemoryBlock *iter;
    OSMemoryBlock *next;

    prev = NULL;
    for (iter = head; iter != NULL; iter = iter->next) {
        if (block <= iter) {
            break;
        }
        prev = iter;
    }
    next = iter;

    block->prev = prev;
    block->next = next;
    if (next != NULL) {
        next->prev = block;
        if (((void *) block + block->size) == next) {
            // Merge with next block
            block->size += next->size;
            next        = next->next;
            block->next = next;
            if (next != NULL) {
                next->prev = block;
            }
        }
    }
    if (prev != NULL) {
        prev->next = block;
        if (((void *) prev + prev->size) == block) {
            // Merge with previous block
            prev->size += block->size;
            prev->next = next;
            if (next != NULL) {
                next->prev = prev;
            }
        }
        return head;
    }
    return block;
}

void *OS_AllocFromHeap(u32 arena, OSHeapHandle heapHandle, s32 size) {
    OSAlloc *alloc;
    OSHeap *heap;
    OSMemoryBlock *block;
    OSMemoryBlock *newBlock;
    u32 extraSpace;
    OSIntrMode irq;

    irq   = OS_DisableInterrupts();
    alloc = sAllocList[arena];
    if (alloc == NULL) {
        OS_RestoreInterrupts(irq);
        return NULL;
    }
    if (heapHandle < 0) {
        heapHandle = alloc->currentHeap;
    }
    heap = &alloc->heaps[heapHandle];
    size = ROUND_UP_32(size + 0x20);
    for (block = heap->free; block != NULL; block = block->next) {
        if (size <= (s32) block->size) {
            break;
        }
    }
    if (block == NULL) {
        OS_RestoreInterrupts(irq);
        return NULL;
    }
    extraSpace = block->size - size;
    if (extraSpace < MIN_BLOCK_SIZE) {
        // Use whole block
        heap->free = OS_RemoveMemoryBlock(heap->free, block);
    } else {
        // Split the block
        block->size    = size;
        newBlock       = (OSMemoryBlock *) ((void *) block + size);
        newBlock->size = extraSpace;
        newBlock->prev = block->prev;
        newBlock->next = block->next;
        if (newBlock->next != NULL) {
            newBlock->next->prev = newBlock;
        }
        if (newBlock->prev != NULL) {
            newBlock->prev->next = newBlock;
        } else {
            heap->free = newBlock;
        }
    }
    heap->occupied = OS_AddOccupiedMemoryBlock(heap->occupied, block);
    OS_RestoreInterrupts(irq);
    return (void *) block + ROUND_UP_32(sizeof(OSMemoryBlock));
}

void OS_FreeFromHeap(u32 arena, OSHeapHandle heapHandle, void *ptr) {
    OSMemoryBlock *block;
    OSHeap *heap;
    OSAlloc *alloc;
    OSIntrMode irq;

    irq   = OS_DisableInterrupts();
    alloc = sAllocList[arena];
    if (heapHandle < 0) {
        heapHandle = alloc->currentHeap;
    }
    heap           = &alloc->heaps[heapHandle];
    block          = ptr - ROUND_UP_32(sizeof(OSMemoryBlock));
    heap->occupied = OS_RemoveMemoryBlock(heap->occupied, block);
    heap->free     = OS_AddFreeMemoryBlock(heap->free, block);
    OS_RestoreInterrupts(irq);
}

OSHeapHandle OS_SetCurrentHeap(u32 arena, OSHeapHandle heap) {
    OSAlloc *alloc;
    s32 oldHeap;
    OSIntrMode irq;

    irq                = OS_DisableInterrupts();
    alloc              = sAllocList[arena];
    oldHeap            = alloc->currentHeap;
    alloc->currentHeap = heap;
    OS_RestoreInterrupts(irq);
    return oldHeap;
}

void *OS_InitAlloc(u32 arena, void *addrLo, void *addrHi, u32 numHeaps) {
    OSHeap *heap;
    OSAlloc *alloc;
    OSIntrMode irq;
    u32 heapsArraySize;
    s32 i;

    alloc             = addrLo;
    irq               = OS_DisableInterrupts();
    heapsArraySize    = numHeaps * sizeof(*alloc->heaps);
    sAllocList[arena] = alloc;
    alloc->heaps      = alloc->heapsArray;
    alloc->numHeaps   = numHeaps;
    for (i = 0; i < alloc->numHeaps; ++i) {
        heap           = &alloc->heaps[i];
        heap->size     = -1;
        heap->occupied = NULL;
        heap->free     = NULL;
    }
    alloc->currentHeap = OS_CURRENT_HEAP_HANDLE;
    alloc->lo          = (void *) ROUND_UP_32((u32) alloc->heaps + heapsArraySize);
    alloc->hi          = (void *) ROUND_DOWN_32(addrHi);
    OS_RestoreInterrupts(irq);
    return alloc->lo;
}

OSHeapHandle OS_CreateHeap(u32 arena, void *addrLo, void *addrHi) {
    OSMemoryBlock *firstBlock;
    OSAlloc *alloc;
    OSHeap *heap;
    OSIntrMode irq;
    s32 i;

    irq    = OS_DisableInterrupts();
    alloc  = sAllocList[arena];
    addrLo = (void *) ROUND_UP_32(addrLo);
    addrHi = (void *) ROUND_DOWN_32(addrHi);
    for (i = 0; i < alloc->numHeaps; ++i) {
        heap = &alloc->heaps[i];
        if (heap->size < 0) {
            heap->size       = addrHi - addrLo;
            firstBlock       = (OSMemoryBlock *) addrLo;
            firstBlock->prev = NULL;
            firstBlock->next = NULL;
            firstBlock->size = (s32) heap->size;
            heap->free       = firstBlock;
            heap->occupied   = NULL;
            OS_RestoreInterrupts(irq);
            return i;
        }
    }
    OS_RestoreInterrupts(irq);
    return -1;
}

s32 OS_CheckHeap(u32 arena, OSHeapHandle heapHandle) {
    OSHeap *heap;
    OSMemoryBlock *block;
    OSAlloc *alloc;
    OSIntrMode irq;
    s32 heapSize;
    s32 totalSize;
    s32 allocatedBytes;
    s32 result;

    totalSize      = 0;
    allocatedBytes = 0;
    result         = -1;
    irq            = OS_DisableInterrupts();
    alloc          = sAllocList[arena];
    if (heapHandle == OS_CURRENT_HEAP_HANDLE) {
        heapHandle = alloc->currentHeap;
    }

    if (alloc->heaps == NULL || heapHandle < 0 || heapHandle >= alloc->numHeaps) {
        // Invalid heap
        goto end;
    }

    heap     = &alloc->heaps[heapHandle];
    heapSize = heap->size;
    if (heapSize < 0) {
        // Heap is not initialized
        goto end;
    }

    block = heap->occupied;
    if (block != NULL && block->prev != NULL) {
        // First block has a preceding block
        goto end;
    }

    while (block != NULL) {
        if (MOD_32(block) != 0) {
            // Misaligned block
            goto end;
        }
        if (block->next != NULL && block->next->prev != block) {
            // Next block does not link back to this block
            goto end;
        }
        if (block->size < MIN_BLOCK_SIZE) {
            // Block is too small
            goto end;
        }
        if (MOD_32(block->size) != 0) {
            // Misaligned block size
            goto end;
        }
        totalSize += block->size;
        if (totalSize <= 0) {
            // Invalid total size
            goto end;
        }
        if (totalSize > heapSize) {
            // Total size exceeds heap size
            goto end;
        }
        block = block->next;
    }

    block = heap->free;
    if (block != NULL && block->prev != NULL) {
        // First block has a preceding block
        goto end;
    }

    while (block != NULL) {
        if (MOD_32(block) != 0) {
            // Misaligned block
            goto end;
        }
        if (block->next != NULL && block->next->prev != block) {
            // Next block does not link back to this block
            goto end;
        }
        if (block->size < MIN_BLOCK_SIZE) {
            // Block is too small
            goto end;
        }
        if (MOD_32(block->size) != 0) {
            // Misaligned block size
            goto end;
        }
        if (block->next != NULL && (u32) block + block->size >= (u32) block->next) {
            // Free list not sorted in descending order
            goto end;
        }
        totalSize += block->size;
        allocatedBytes += block->size - 0x20;
        if (totalSize <= 0) {
            // Invalid total size
            goto end;
        }
        if (totalSize > heapSize) {
            // Total size exceeds heap size
            goto end;
        }
        block = block->next;
    }
    if (totalSize == heapSize) {
        result = allocatedBytes;
    }
end:
    OS_RestoreInterrupts(irq);
    return result;
}