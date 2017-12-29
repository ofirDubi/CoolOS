#ifndef __COOLOS_GUI__WINDOW_H
#define __COOLOS_GUI__WINDOW_H


#include <gui/widget.h>
#include <drivers/mouse.h>
#include <common/types.h>


namespace coolOS{
    namespace gui{
        class Window : public CompositeWidget{
        protected:
            bool Dragging;
        public:
            Window(Widget * parent, 
                    common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, 
                    common::RGB color);
            ~Window();
            
            
            void OnMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            void OnMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            void OnMouseMove(common::int32_t old_x, common::int32_t old_y,common::int32_t new_x, common::int32_t new_y );
        };
    }
}

#endif