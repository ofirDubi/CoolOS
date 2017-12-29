
#include <gui/widget.h>


using namespace coolOS::gui;
using namespace coolOS::common;


Widget::Widget(Widget * parent, 
        int32_t x, int32_t y, int32_t w, int32_t h, 
        RGB color)
        : KeyBoardEventHandler()
{
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->color = color;
    this->Focussable = true;
}
Widget::~Widget(){
    
}

void Widget::GetFocus(Widget* widget){
    if(parent != 0){
        parent->GetFocus(widget);
    }
}


bool Widget::ContainsCoordinate(int32_t x, int32_t y){
    return this->x <= x && x < this->x + this->w 
            && this->y <=y && y < this->y + this->h;
}

void Widget::ModelToScreen(int32_t &x , int32_t &y){
    //needs to be changed to match pass-by-reference
    if(parent != 0){
        parent->ModelToScreen(x,y);
    }
    x+= this->x;
    y += this->y;
    
}

void Widget::Draw(GraphicsContext* gc){
    
    int X = x;
    int Y = y;
    ModelToScreen(X, Y);
    gc->FillRectangle(X,Y,w,h,color);

}

void Widget::OnMouseDown(int32_t x, int32_t y, uint8_t button){
    
    if(Focussable){
        GetFocus(this);
    }
    
}

void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t button){
    
}
void Widget::OnMouseMove(int32_t old_x, int32_t old_y,int32_t new_x, int32_t new_y ){
    
}






CompositeWidget::CompositeWidget(Widget * parent, 
        int32_t x, int32_t y, int32_t w, int32_t h, 
        RGB color)
: Widget(parent, x, y, w , h, color)
{
    focussedChild = 0;
    numChildren = 0;
}
CompositeWidget::~CompositeWidget(){
    
}

void CompositeWidget::GetFocus(Widget* widget){
    this->focussedChild = widget;
    if(parent != 0){
        parent->GetFocus(this);
    }
}

bool CompositeWidget::AddChild(Widget* child){
    if(numChildren >= 100){
        return false;
    }
    children[numChildren++] = child;
    return true;
}



void CompositeWidget::Draw(GraphicsContext* gc){
    //first draw its background
    
    Widget::Draw(gc);
    for(int i=numChildren-1; i>=0; --i){
        children[i]->Draw(gc);
    }
}

void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button){
    //pass to the correct child
    
    for(int i=0; i<numChildren; i++){
        if(children[i]->ContainsCoordinate(x-this->x, y- this->y)){
            
            children[i]->OnMouseDown(x-this->x, y- this->y, button);
            break;
        }
    }
}
void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button){
     for(int i=0; i<numChildren; i++){
        if(children[i]->ContainsCoordinate(x-this->x, y- this->y)){

            children[i]->OnMouseUp(x-this->x, y- this->y,  button);
            break;
        }
       
    }
}
//TODO: split to onMouseEnter and OnMouseLeve
void CompositeWidget::OnMouseMove(int32_t old_x, int32_t old_y,int32_t new_x, int32_t new_y ){
    
    int firstchild = -1;
    
    for(int i=0; i<numChildren; i++){
        if(children[i]->ContainsCoordinate(old_x-this->x, old_y- this->y)){

            children[i]->OnMouseMove(old_x-this->x, old_y- this->y, new_x-this->x, new_y- this->y);
            firstchild = i;
            break;
        }
        
    }
     
    for(int i=0; i<numChildren; i++){
        if(children[i]->ContainsCoordinate(new_x-this->x, new_y- this->y)){
            //needs to change from gc
            if(firstchild != i);
            children[i]->OnMouseMove(old_x-this->x, old_y- this->y, new_x-this->x, new_y- this->y);
            break;
        }
        
    }
}


void CompositeWidget::OnKeyDown(char str){
    if(focussedChild !=0){
        focussedChild->OnKeyDown(str);
    }
}
void CompositeWidget::OnKeyUp(char str){
    if(focussedChild !=0){
        focussedChild->OnKeyUp(str);
    }
}