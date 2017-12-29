/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RGB.h
 * Author: ofir123dubi
 *
 * Created on December 29, 2017, 1:35 PM
 */

#ifndef __COOLOS__COMMON__RGB_H
#define __COOLOS__COMMON__RGB_H

#define RGB_WHITE {0xff, 0xff, 0xff} 
#define RGB_BLACK {0x00, 0x00, 0x00} 
#define RGB_BLUE {0x00, 0x00, 0xA8} 
#define RGB_GREEN {0x00, 0xA8, 0x00} 
#define RGB_RED {0xA8, 0x00, 0x00}

namespace coolOS{
    namespace common{
       
        typedef struct {
                         
                coolOS::common::uint8_t r;
                coolOS::common::uint8_t g;
                coolOS::common::uint8_t b;
        }RGB;
    }
}



#endif /* RGB_H */

