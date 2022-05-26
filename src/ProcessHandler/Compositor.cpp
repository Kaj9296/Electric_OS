#include "Compositor.h"
#include "ProcessHandler.h"

#include "STL/Memory/Memory.h"

namespace Compositor
{            
    STL::Framebuffer* Frontbuffer;
    STL::Framebuffer Backbuffer;

    bool DrawMouse = false;

    bool RenderRequest = false;

    STL::Point OldMousePos;     
    STL::Point MousePos;     

    void SwapBuffers()
    {    
        STL::ARGB BeforeCursor[16 * 16];
    
        if (DrawMouse)
        {
            DrawMouse = false;
            
            OldMousePos = MousePos;

            for (int Y = 0; Y < 12; Y++)
            {
                for (int X = 0; X < 12 - Y; X++)
                {
                    BeforeCursor[X + Y * 16] = Backbuffer.GetPixel(STL::Point(MousePos.X + X,MousePos.Y + Y));
                    Backbuffer.PutPixel(STL::Point(MousePos.X + X, MousePos.Y + Y), STL::ARGB(255));
                }
            }

            STL::CopyMemory(Backbuffer.Base, Frontbuffer->Base, Backbuffer.Size);

            for (int Y = 0; Y < 12; Y++)
            {
                for (int X = 0; X < 12 - Y; X++)
                {
                    Backbuffer.PutPixel(STL::Point(MousePos.X + X, MousePos.Y + Y), BeforeCursor[X + Y * 16]);
                }
            }
            
            DrawMouse = true;
        }
        else
        {
            STL::CopyMemory(Backbuffer.Base, Frontbuffer->Base, Backbuffer.Size);
        }
    }

    void Init(STL::Framebuffer* Screenbuffer)
    {
        Frontbuffer = Screenbuffer;
        Backbuffer = *Frontbuffer;
        
        Backbuffer.Base = (STL::ARGB*)STL::Malloc(Backbuffer.Size);
        Backbuffer.Clear();
    }

    void UpdateMouse(STL::MDATA MouseData)
    {
        if (DrawMouse)
        {                  
            DrawMouse = false;
         
            MousePos = MouseData.Pos;
      
            for (int Y = 0; Y < 12; Y++)
            {
                STL::CopyMemory(Backbuffer.Base + (OldMousePos.Y + Y) * Backbuffer.PixelsPerScanline + (OldMousePos.X), 
                Frontbuffer->Base + (OldMousePos.Y + Y) * Frontbuffer->PixelsPerScanline + (OldMousePos.X), (12 - Y) * 4);
            }

            OldMousePos = MousePos;

            for (int Y = 0; Y < 12; Y++)
            {              
                STL::SetMemory(Frontbuffer->Base + (MousePos.Y + Y) * Frontbuffer->PixelsPerScanline + (MousePos.X), 255, (12 - Y) * 4);
            }            
            
            DrawMouse = true;
        }  
    }

    void RequestRender()
    {
        RenderRequest = true;
    }

    void Update()
    {            
        if (RenderRequest)
        {
            if (ProcessHandler::GrandFatherProcess != nullptr)
            {
                ProcessHandler::GrandFatherProcess->Render(&Backbuffer, STL::Point(0, 0), STL::Point(Backbuffer.Width, Backbuffer.Height));
            }
            SwapBuffers();

            RenderRequest = false;
        }
    }
}