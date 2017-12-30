/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <multitasking.h>
using namespace coolOS;
using namespace coolOS::common;


Task::Task(GlobalDescriptorTable *gdt, void entrypoint()){
    cpustate = (CPUState*)(stack + sizeof(stack) -sizeof(CPUState));
    
    cpustate->eax =0;
    cpustate->ebx =0;
    cpustate->ecx=0;
    cpustate->edx =0;
    cpustate->esi =0;
    cpustate->edi =0;
    cpustate->ebp =0;
    
    /*
    cpustate-> gs = 0; //g?
    cpustate-> fs = 0; //
    cpustate-> es = 0; //extra segment
    cpustate-> ds = 0; //data segment
    */

    //cpustate> error =0;
    
    // cpustate->esp =0; //uncomment when using security lvels
    cpustate->eip = (uint32_t)entrypoint;
    cpustate->cs = gdt->getCodeSegmentSelector();
    //cpustate->ss =0; //uncomment when implementing securety levels
    cpustate->eflags =0x202; //check what that means
}
Task::~Task(){

}



Task* tasks[256];
int numTasks;
int currentTask;

TaskManager::TaskManager(){
    numTasks = 0;
    currentTask = -1; 
}
TaskManager::~TaskManager(){
    
}

bool TaskManager::AddTask(Task * task){
    if(numTasks >= 256){
        return false;
    }
    tasks[numTasks++] = task;
    return true;
    
}
CPUState* TaskManager::Schedule(CPUState* cpustate){
    if(numTasks <=0){
        return cpustate;
    }
    if(currentTask >= 0){
        tasks[currentTask]->cpustate = cpustate;
    }
    
    if(++currentTask >= numTasks){
        currentTask %= numTasks;
    }
    return tasks[currentTask]->cpustate;
}
