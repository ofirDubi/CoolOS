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
        
 
        class Driver{

        public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual void Deactivate();
            virtual int Reset();

        };
        class DriverManager{
        public:
            Driver* drivers[265];
            int numDrivers;

        public:
            DriverManager();
            void AddDriver(Driver* drv);

            void ActivateAll();

        };
   }
}
#endif /* DRIVER_H */

