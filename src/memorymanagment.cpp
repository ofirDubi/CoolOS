/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//can be done better 
#include<memorymanagment.h>

using namespace coolOS;
using namespace coolOS::common;
void * printf(char*);

MemoryManager* MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(size_t start, size_t size){
    
    activeMemoryManager = this;
    
    if(size < sizeof(MemoryChunk)){
        first = 0;
    }else{
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

void* MemoryManager::malloc(size_t size){
    //iterate through the list of chunks and look for a chunk that is large enough
    MemoryChunk* result = 0;
    
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
        
        temp->allocated = false;
        temp->size =  result->size - size - sizeof(MemoryChunk);
        temp->prev = result;        
        temp->next = result->next;
        if(temp->next != 0){
            temp->next->prev = temp;
        }
        
        result->size = size;
        result->next = temp;
    }
    
    result->allocated = true;
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
    
}
// finished at 37:10
void MemoryManager::free(void* ptr){
    //ptr - a pointer to the freed memory 
    
    //chunk is a pointer to the freed memory's MemoryChunk struct
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    
    chunk->allocated = false;
    
    //merge 
    
    if(chunk->prev != 0 && !chunk->prev->allocated){
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        
        if(chunk->next != 0){
            chunk->next->prev = chunk->prev;
        }
        
        chunk = chunk->prev;
    }
    if(chunk->next != 0 && !chunk->next->allocated){
        //swallow the next chunk if it is no allocated
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        
        if(chunk->next != 0){
            chunk->next->prev = chunk;
        }
    
    }
}


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

void* operator new(unsigned size, void* ptr){
    return ptr;
}

void* operator new[](unsigned size, void* ptr){
    return ptr;
}

void operator delete(void* ptr){
     if(MemoryManager::activeMemoryManager !=0){   
        MemoryManager::activeMemoryManager->free(ptr);
    }
}

void operator delete[](void* ptr){
    if(MemoryManager::activeMemoryManager !=0){   
        MemoryManager::activeMemoryManager->free(ptr);
    }
}
