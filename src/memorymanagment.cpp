/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include<memorymanagment.h>
#include <common/coolio.h>


using namespace coolOS;
using namespace coolOS::common;


MemoryManager* MemoryManager::activeMemoryManager = 0;

//initialize a linked list with one chunk
MemoryManager::MemoryManager(size_t start, size_t size){
    //set this memory manager as the active one
    activeMemoryManager = this;
    
    //check if we have enough space to allocate a chunks header
    if(size < sizeof(MemoryChunk)){
        first = 0;
    }else{
        //create one chunk with the specified size specified starting location 
        first = (MemoryChunk*)start;

        first->allocated = false;
        first->prev = 0;
        first->next = 0;
        first->size = size - sizeof(MemoryChunk);
    }
    
}

MemoryManager::~MemoryManager(){
    if(activeMemoryManager == this){
        activeMemoryManager = 0;
    }
}

//allocate a new chunk of memory
void* MemoryManager::malloc(size_t size){
   
    MemoryChunk* result = 0;
     //iterate through the list of chunks and look for a chunk that is large enough
    for(MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next){
        if(chunk->size > size && !chunk->allocated){
            result = chunk;
        }
    }
    if(result==0){
        printf("\nnot enogh memory\n");
        return 0;
    }
    
    if(result->size >= size + sizeof(MemoryChunk)+1){
        //if the chunk is  big enough to split 
        //split the chunk into smaller parts - only use what you need
        
        //temp is the MemoryChunk that will come after result
        MemoryChunk* temp = (MemoryChunk*)((size_t)result+ sizeof(MemoryChunk) + size);
        
        //initiate temp to an unallocated chunk with the leftover memory from the original chunk
        temp->allocated = false;
        temp->size =  result->size - size - sizeof(MemoryChunk);
        temp->prev = result;        
        //place temp between result and the next chunk
        temp->next = result->next;
        if(temp->next != 0){
            temp->next->prev = temp;
        }
        
        result->size = size;
        result->next = temp;
    }
    
    result->allocated = true;
    //return a pointer to the chunk after it's header.
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
    
}

//free a chunk of memory
void MemoryManager::free(void* ptr){
    //ptr - a pointer to the freed memory 
    
    //chunk is a pointer to the freed memory's MemoryChunk struct
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    
    chunk->allocated = false;
    
    //merge
    
    //if there is a previous chunk and the next chunk is unallocated, merge this chunk into the next one
    if(chunk->prev != 0 && !chunk->prev->allocated){
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        
        if(chunk->next != 0){
            chunk->next->prev = chunk->prev;
        }
        
        chunk = chunk->prev;
    }
    //if there is a next chunk and this chunk is not allocated, merge the next chunk into this one
    if(chunk->next != 0 && !chunk->next->allocated){
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        
        if(chunk->next != 0){
            chunk->next->prev = chunk;
        }
    
    }
}

//create a new instance of a specific class with this size
void* operator new(unsigned size){
    if(MemoryManager::activeMemoryManager ==0){
     return 0;   
    }
    return MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](unsigned size){
     if(MemoryManager::activeMemoryManager ==0){
     return 0;   
    }
    return MemoryManager::activeMemoryManager->malloc(size);
}

//transfer object - not implemented yet
void* operator new(unsigned size, void* ptr){
    return ptr;
}

//transfer array - not implemented yet
void* operator new[](unsigned size, void* ptr){
    return ptr;
}

//delete an object
void operator delete(void* ptr){
     if(MemoryManager::activeMemoryManager !=0){   
        MemoryManager::activeMemoryManager->free(ptr);
    }
}

//delete array
void operator delete[](void* ptr){
    if(MemoryManager::activeMemoryManager !=0){   
        MemoryManager::activeMemoryManager->free(ptr);
    }
}
