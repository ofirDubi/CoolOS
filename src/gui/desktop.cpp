
#include <gui/desktop.h>

using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::gui;

Desktop::Desktop(int32_t w,int32_t h, RGB color)
: CompositeWidget(0,0,0,w,h,color), MouseEventHandler()
{
    //set mouse to center of the screen
    MouseX = w/2;
    MouseY = h/2;
}

Desktop::~Desktop(){
    
}

void  Desktop::Draw(GraphicsContext * gc){
    CompositeWidget::Draw(gc);
    
    for(int i=0; i< 4; i++){
        
        gc->PutPixel(MouseX - i, MouseY, RGB_WHITE);
        gc->PutPixel(MouseX + i, MouseY, RGB_WHITE);
        gc->PutPixel(MouseX, MouseY - i, RGB_WHITE);
        gc->PutPixel(MouseX, MouseY + i, RGB_WHITE);
    }
}


void Desktop::OnMouseDown(uint8_t button){
    CompositeWidget::OnMouseDown(MouseX, MouseY, button);
}

void Desktop::OnMouseUp(uint8_t button){
    CompositeWidget::OnMouseUp(MouseX, MouseY, button);

}

void Desktop::OnMouseMove(int x,int y){
    //movement is too fast, so devide it by 4
    x /= 4;
    y /= 4;
    
    int32_t newMouseX = MouseX + x;
    if(newMouseX < 0) newMouseX = 0;
    if(newMouseX >=w) newMouseX = w-1;
    
    int32_t newMouseY = MouseY + y;
    if(newMouseY < 0) newMouseY = 0;
    if(newMouseY >=h) newMouseY = h-1;
    
    
    CompositeWidget::OnMouseMove(MouseX, MouseY, newMouseX, newMouseY);
    
    MouseX = newMouseX;
    MouseY = newMouseY;
}
