
#ifndef __COOLOS_GUI__WIDGET_H
#define __COOLOS_GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/keyboard.h>

namespace coolOS{
    namespace gui{
        
        class Widget : public coolOS::drivers::KeyBoardEventHandler {
            
        protected:
            Widget * parent;
            common::int32_t x;
            common::int32_t y;
            common::int32_t w;
            common::int32_t h;
            
            common::RGB color;
            
            bool Focussable;
            
        public:
            Widget(Widget * parent, 
                    common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, 
                    common::RGB color);
            ~Widget();
            
            virtual bool ContainsCoordinate(common::int32_t x, common::int32_t y);
            
            virtual void GetFocus(Widget* widget);
            virtual void ModelToScreen(common::int32_t &x , common::int32_t &y);
            
            virtual void Draw(common::GraphicsContext* gc);
            virtual void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseMove(common::int32_t old_x, common::int32_t old_y,common::int32_t x, common::int32_t y );

            
            
           
            
        };
        
        
        class CompositeWidget : public Widget{
        private:
            Widget* children[100];
            int numChildren;
            Widget* focussedChild;
            
        public:
            
            CompositeWidget(Widget * parent, 
                    common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, 
                    common::RGB color);
            ~CompositeWidget();
            
            virtual void GetFocus(Widget* widget);
            virtual bool AddChild(Widget* child);
            
            
            virtual void Draw(common::GraphicsContext* gc);
            virtual void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void OnMouseMove(common::int32_t old_x, common::int32_t old_y,common::int32_t x, common::int32_t y );
            
            
            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };
    }
}

#endif