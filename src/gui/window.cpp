
#include <gui/window.h>

using namespace coolOS::common;
using namespace coolOS::gui;

Window::Window(Widget * parent, 
                    int32_t x, int32_t y, int32_t w, int32_t h, 
                    RGB color)
: CompositeWidget(parent, x, y, w, h, color)
{
    Dragging = false;
}
Window::~Window(){
    
}


void Window::OnMouseDown(int32_t x, int32_t y, uint8_t button){
        
    Dragging = button ==1;
    CompositeWidget::OnMouseDown(x,y,button);
}
void Window::OnMouseUp(int32_t x, int32_t y, uint8_t button){

        Dragging = false;
        CompositeWidget::OnMouseUp(x,y,button);

}
void Window::OnMouseMove(int32_t old_x, int32_t old_y,int32_t new_x, int32_t new_y ){
    if(Dragging){
        this->x += new_x - old_x;
        this->y += new_y - old_y;
    }
    
    CompositeWidget::OnMouseMove(old_x, old_y, new_x, new_y);

}