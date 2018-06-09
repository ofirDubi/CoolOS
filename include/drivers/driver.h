/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   driver.h
 * Author: ofir123dubi
 *
 * Created on December 18, 2017, 6:44 AM
 */

#ifndef __COOLOS_DRIVERS__DRIVER_H
#define __COOLOS_DRIVERS__DRIVER_H
namespace coolOS{
    namespace drivers{
        
        //a class that represents a generic Driver
        class Driver{

        public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual void Deactivate();
            //restart the connected device
            virtual int Reset();

        };
        class DriverManager{
        private:
            //an array of drivers
            Driver* drivers[265];
            // the number of current drivers in the array
            int numDrivers;

        public:
            DriverManager();
            //a function that adds drivers
            void AddDriver(Driver* drv);
            //a function that activate each driver in the drivers array
            void ActivateAll();

        };
   }
}
#endif /* DRIVER_H */

