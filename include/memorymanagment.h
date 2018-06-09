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
    // a linked list of memory chunk headers
    struct MemoryChunk{
        //pointers to the next and previous items on the linked list
        MemoryChunk* next;
        MemoryChunk* prev;
        //is this chunk allocated
        bool allocated;
        //the size of this chunk (not including the header size)
        common::size_t size;
    };
    
    //a memory manager that holds a pointer to the first chunk in the heap and provides malloc and free utilities
    class MemoryManager{
    protected:
        //the first node of the memory's linked list
        MemoryChunk* first;
    public:
        
        //The current memory manager
        static MemoryManager *activeMemoryManager;
        
        MemoryManager(common::size_t first, common::size_t size);
        ~MemoryManager();
        
        //allocate a chunk of memory
        void* malloc(common::size_t size);
        
        //free a chunk of memory
        void free(void* ptr);
        
    };
}


void* operator new(unsigned size);
void* operator new[](unsigned size);

//placement new
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);


void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif /* MEMORYMANEGMENT_H */

