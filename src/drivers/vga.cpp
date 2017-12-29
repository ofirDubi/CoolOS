/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/vga.h>

using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;
using namespace coolOS::common;
   

        


VideoGraphicsArray::VideoGraphicsArray():
        miscPort(0x3c2),
        crtcIndexPort(0x3d4),
        crtcDataPort(0x3d5),
        sequencerIndexPort(0x3c4),
        sequencerDataPort(0x3c5),
        graphicsControllerIndexPort(0x3ce),
        graphicsControllerDataPort(0x3cf),
        attributeControllerIndexPort(0x3c0),
        attributeControllerReadPort(0x3c1),
        attributeControllerWritePort(0x3dc0),
        attributeControllerResetPort(0x3da)
{
    
}
VideoGraphicsArray::~VideoGraphicsArray(){
    
}


void VideoGraphicsArray::WriteRegisters(uint8_t* registers){
    miscPort.Write(*(registers++)); //write misc part
    
    //sequencer 
    for(uint8_t i=0; i< 5; i++){
        sequencerIndexPort.Write(i); //where to write the data
        sequencerDataPort.Write(*(registers++)); //what to write
    }
    
    //cathode ray tube controller 
    crtcIndexPort.Write(0x03);
    crtcDataPort.Write(crtcDataPort.Read() | 0x80);
    crtcIndexPort.Write(0x011);
    crtcDataPort.Write(crtcDataPort.Read() & ~0x80);
    
    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80;
    
    for(uint8_t i=0; i< 25; i++){
        crtcIndexPort.Write(i); //where to write the data
        crtcDataPort.Write(*(registers++)); //what to write
    }
    
    // graphics controller
    for(uint8_t i=0; i< 9; i++){
        graphicsControllerIndexPort.Write(i); //where to write the data
        graphicsControllerDataPort.Write(*(registers++)); //what to write
    }
    
    // attribute controller
    for(uint8_t i=0; i< 21; i++){
        
        attributeControllerResetPort.Read();
        attributeControllerIndexPort.Write(i); //where to write the data
        attributeControllerWritePort.Write(*(registers++)); //what to write
    }
    
    attributeControllerResetPort.Read();
    attributeControllerIndexPort.Write(0x20);
}



bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth){
    return width == 320 && height == 200 && colordepth ==8;
}

bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height, uint32_t colordepth){
    if(!SupportsMode(width, height, colordepth)){
        return false;
    }
    
    unsigned char g_320x200x256[] =
     {
         /* MISC */
             0x63,
         /* SEQ */
             0x03, 0x01, 0x0F, 0x00, 0x0E,
         /* CRTC */
             0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
             0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
             0xFF,
         /* GC */
             0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
             0xFF,
         /* AC */
             0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
             0x41, 0x00, 0x0F, 0x00, 0x00
     };
     
    WriteRegisters(g_320x200x256);
    return true;
 }

uint8_t *  VideoGraphicsArray::GetFrameBufferSegment(){
    graphicsControllerIndexPort.Write(0x06);
    
    //get the location of video memory
    
                                //get last 2 bits
    uint8_t segmentNumber = ((graphicsControllerDataPort.Read() >>2 ) & 0x03) ;
    
    switch(segmentNumber){
        default:
           
        case 0x00: return (uint8_t*)0x00000;
        case 0x01: return (uint8_t*)0xA0000;
        case 0x10: return (uint8_t*)0xB0000;
        case 0x11: return (uint8_t*)0xBB000;
    }
    
}

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t colorIndex){
    if(x < 0 || 320 <= x
     || y <0 || 200 <= y){
        return;
    }       
    //store hight and width insted of hare coding it
    uint8_t* pixelAddress = GetFrameBufferSegment() + 320*y + x;
    *pixelAddress = colorIndex;
}

uint8_t VideoGraphicsArray::GetColorIndex(RGB color){
    
   
    if(color.r == 0x00 && color.g== 0x00 && color.b == 0x00) return 0x00; // black
    if(color.r == 0x00 && color.g== 0x00 && color.b == 0xA8) return 0x01; //blue
    if(color.r == 0x00 && color.g== 0xA8 && color.b == 0x00) return 0x02; //green
    if(color.r == 0xA8 && color.g== 0x00 && color.b == 0x00) return 0x04; //red
    if(color.r == 0xFF && color.g== 0xFF && color.b == 0xFF) return 0x3F; //white
    return 0x00; //default is black
    
 }




void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, RGB color){
    PutPixel(x,y, GetColorIndex(color));
}


void VideoGraphicsArray::FillRectangle(int32_t x, int32_t y, uint32_t w,
                                        uint32_t h, RGB color){
     for(int32_t Y = y; Y<y+h; Y++){
        for(int32_t X = x; X<x+w; X++){
            
            PutPixel(X,Y,color);
        }
    }
 }
