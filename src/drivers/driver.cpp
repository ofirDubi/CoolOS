/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/driver.h>

using namespace coolOS::drivers;


Driver::Driver(){
    
}

Driver::~Driver(){
    
}

void Driver::Activate(){
    
}

int Driver::Reset(){
    
}

void Driver::Deactivate(){
    
}

DriverManager::DriverManager() {
    numDrivers= 0;
}
void DriverManager::AddDriver(Driver* drv){
    drivers[numDrivers] = drv;
    numDrivers++;
}

void DriverManager::ActivateAll(){
    
    for(int i=0; i< numDrivers; i++){
        drivers[i]->Activate();
    }
}