/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   vga.h
 * Author: ofir123dubi
 *
 * Created on December 29, 2017, 2:00 AM
 */

#ifndef __COOLOS_DRIVERS__VGA_H
#define __COOLOS_DRIVERS__VGA_H


#include <common/types.h>
#include <common/RGB.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace coolOS{

    namespace drivers{
         
        class VideoGraphicsArray{            
        protected:
            hardwarecommunication::Port<coolOS::common::uint8_t> miscPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> crtcIndexPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> crtcDataPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> sequencerIndexPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> sequencerDataPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> graphicsControllerIndexPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> graphicsControllerDataPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> attributeControllerIndexPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> attributeControllerReadPort;
            hardwarecommunication::Port<coolOS::common::uint8_t> attributeControllerWritePort;
            hardwarecommunication::Port<coolOS::common::uint8_t> attributeControllerResetPort;
            
            void WriteRegisters(coolOS::common::uint8_t* registers);
            coolOS::common::uint8_t * GetFrameBufferSegment();
            
            virtual coolOS::common::uint8_t GetColorIndex(coolOS::common::RGB rgb);
            
        public:
            
            VideoGraphicsArray();
            ~VideoGraphicsArray();
            
            
            virtual bool SupportsMode(coolOS::common::uint32_t width, coolOS::common::uint32_t height, coolOS::common::uint32_t colordepth);
            
            virtual bool SetMode(coolOS::common::uint32_t width, coolOS::common::uint32_t height, coolOS::common::uint32_t colordepth);
            
            virtual void PutPixel(coolOS::common::int32_t x, coolOS::common::int32_t y, coolOS::common::RGB color);
            
            //move to protected later
            virtual void PutPixel(coolOS::common::int32_t x, coolOS::common::int32_t y, coolOS::common::uint8_t colorIndex);
            
            virtual void FillRectangle(coolOS::common::int32_t x, coolOS::common::int32_t y, coolOS::common::uint32_t w,
                                        coolOS::common::uint32_t h, coolOS::common::RGB color);

        };
        
    }
}


#endif /* VGA_H */

