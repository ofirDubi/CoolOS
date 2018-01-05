/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memorymanegment.h
 * Author: ofir123dubi
 *
 * Created on January 5, 2018, 1:47 PM
 */

#ifndef __COOLOS__MEMORYMANEGMENT_H
#define __COOLOS__MEMORYMANEGMENT_H

#include <common/types.h>

namespace coolOS{
    struct MemoryChunk{
        MemoryChunk next;
        MemoryChunk* prev;
        bool allocated;
        common::size_t size;
    };
    
    class MemoryManager{
    protected:
        MemoryChunk* first;
    public:
        static MemoryManager *activeMemoryManager;
        
        MemoryManager(common::size_t first, common::size_t size);
        ~MemoryManager();
        
        void* malloc(common::size_t size);
        void free(void* ptr);
        
    };
}

#endif /* MEMORYMANEGMENT_H */

