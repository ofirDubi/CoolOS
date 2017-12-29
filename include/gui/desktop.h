#ifndef __COOLOS__GUI__DESKTOP_H
#define __COOLOS__GUI__DESKTOP_H

#include <gui/widget.h>
#include <drivers/mouse.h>
#include <common/types.h>


namespace coolOS{
    namespace gui{
        class Desktop : public CompositeWidget, public coolOS::drivers::MouseEventHandler{
            
        protected:
            common::uint32_t MouseX;
            common::uint32_t MouseY;
            
        public:
            Desktop(common::int32_t w,common::int32_t h, common::RGB color);
            ~Desktop();
            
            void Draw(common::GraphicsContext * gc);
            
            
            void OnMouseDown(coolOS::common::uint8_t button);
            void OnMouseUp(coolOS::common::uint8_t button);
            void OnMouseMove(int x,int y);
            
            
            
        };
    }
}

#endif