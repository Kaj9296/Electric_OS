#include "Process.h"
#include "ProcessHandler.h"
#include "Compositor.h"

#include "Memory/Heap.h"
#include "Renderer/Renderer.h"
#include "Debug/Debug.h"

#define FRAME_OFFSET STL::Point(0, 30)
#define CLOSE_BUTTON_SIZE STL::Point(16, 16)
#define CLOSE_BUTTON_OFFSET STL::Point(((FRAME_OFFSET.X - CLOSE_BUTTON_SIZE.X) / 2 - CLOSE_BUTTON_SIZE.X), -FRAME_OFFSET.Y / 2 - CLOSE_BUTTON_SIZE.Y / 2)
#define MINIMIZE_BUTTON_SIZE STL::Point(16, 16)
#define MINIMIZE_BUTTON_OFFSET CLOSE_BUTTON_OFFSET - STL::Point(16 + RAISED_WIDTH * 3, 0)

void RenderSimpleFrame(STL::Framebuffer* Buffer, STL::Point ScreenPos, STL::Point WindowSize, STL::String& Title, bool IsFocused)
{
    STL::ARGB Background;
    STL::ARGB Foreground;
    if (IsFocused)
    {
        Background = STL::ARGB(255, 14, 0, 135);            
        Foreground = STL::ARGB(255);
    }
    else
    {
        Background = STL::ARGB(128);            
        Foreground = STL::ARGB(192);            
    }

    //Draw topbar
    Buffer->DrawRaisedRectEdge(ScreenPos - FRAME_OFFSET, ScreenPos + WindowSize);
    Buffer->DrawRect(ScreenPos - FRAME_OFFSET, ScreenPos + STL::Point(WindowSize.X, 0), Background);

    //Draw close button                    
    STL::Point CloseButtonPos = CLOSE_BUTTON_OFFSET + STL::Point(WindowSize.X, 0) + ScreenPos;                    
    Buffer->DrawRect(CloseButtonPos, CloseButtonPos + CLOSE_BUTTON_SIZE, STL::ARGB(200));
    Buffer->PutChar('X', CloseButtonPos + STL::Point(RAISED_WIDTH, 2), 1, STL::ARGB(0), STL::ARGB(200));
    Buffer->DrawRaisedRectEdge(CloseButtonPos, CloseButtonPos + CLOSE_BUTTON_SIZE);

    //Draw minimize button                    
    STL::Point MinimizeButtonPos = MINIMIZE_BUTTON_OFFSET + STL::Point(WindowSize.X, 0) + ScreenPos;
    Buffer->DrawRect(MinimizeButtonPos, MinimizeButtonPos + MINIMIZE_BUTTON_SIZE, STL::ARGB(200));
    Buffer->PutChar('_', MinimizeButtonPos + STL::Point(RAISED_WIDTH, 0), 1, STL::ARGB(0), STL::ARGB(200));
    Buffer->DrawRaisedRectEdge(MinimizeButtonPos, MinimizeButtonPos + MINIMIZE_BUTTON_SIZE);

    //Print Title
    STL::Point TextPos = ScreenPos + STL::Point(RAISED_WIDTH * 2, -FRAME_OFFSET.Y / 2 - 8);
    Buffer->Print(Title.cstr(), TextPos, 1, Foreground, Background); 
}

void Process::Render(STL::Framebuffer* Buffer, STL::Point DomainOrigin, STL::Point DomainSize)
{
    STL::Point ScreenPos = DomainOrigin + this->Pos;

    if (ScreenPos.X < 0 || ScreenPos.X + (int32_t)this->FrameBuffer.Width > DomainSize.X || ScreenPos.Y < 0 || ScreenPos.Y + (int32_t)this->FrameBuffer.Height > DomainSize.Y)
    {
        return;
    }

    void* Source = (uint8_t*)(this->FrameBuffer.Base);
    void* Dest = (uint8_t*)(Buffer->Base + ScreenPos.X + Buffer->PixelsPerScanline * ScreenPos.Y);
    for (uint32_t y = 0; y < this->FrameBuffer.Height; y++)
    {             
        STL::CopyMemory(Source, Dest, this->FrameBuffer.PixelsPerScanline * 4);
        Source = (void*)((uint64_t)Source + this->FrameBuffer.PixelsPerScanline * 4);
        Dest = (void*)((uint64_t)Dest + Buffer->PixelsPerScanline * 4);   
    }

    if (this->RenderFrameFunction != nullptr)
    {
        this->RenderFrameFunction(Buffer, ScreenPos, STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height), this->Title, this == ProcessHandler::FocusedProcess);
    }

    for (uint32_t i = 0; i < this->Children.Length(); i++)
    {
        this->Children[i]->Render(Buffer, ScreenPos, STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height));
    }
}

void Process::HandleRequest()
{
    for (uint64_t i = 0; i < this->Children.Length(); i++)
    {
        this->Children[i]->HandleRequest();
    }

    switch (this->PopRequest())
    {
    case STL::PROR::CLEAR:
    {
        this->FrameBuffer.Clear();
        this->SendMessage(STL::PROM::CLEAR, &this->FrameBuffer);        
        Compositor::RequestRender();
    }
    break;
    case STL::PROR::DRAW:
    {
        this->SendMessage(STL::PROM::DRAW, &this->FrameBuffer);
        Compositor::RequestRender();
    }
    break;
    case STL::PROR::KILL:
    {
        this->Kill();
        Compositor::RequestRender();
        return;
    }
    break;
    case STL::PROR::RESET:
    {
        //KillAllProcesses();
        //Loop();
    }
    break;
    default:
    {

    }
    break;
    }
}

void Process::HandleMouseData(STL::MDATA MouseData, STL::Point DomainOrigin)
{    
    STL::Point ScreenPos = DomainOrigin + this->Pos;
    STL::Point TopLeft = ScreenPos - FrameSize;
    STL::Point BottomRight = ScreenPos + STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height) + FrameSize;

    if (STL::Contains(TopLeft, BottomRight, MouseData.Pos))
    {
        for (uint64_t i = this->Children.Length(); i --> 0; )
        {
            STL::Point ChildScreenPos = ScreenPos + this->Children[i]->Pos;
            STL::Point ChildTopLeft = ChildScreenPos - this->Children[i]->FrameSize;
            STL::Point ChildBottomRight = ChildScreenPos + STL::Point(this->Children[i]->FrameBuffer.Width, this->Children[i]->FrameBuffer.Height) + this->Children[i]->FrameSize;    
            
            if (STL::Contains(ChildTopLeft, ChildBottomRight, MouseData.Pos))
            {            
                this->Children[i]->HandleMouseData(MouseData, ScreenPos);
                return;
            }
        }

        MouseData.Pos = MouseData.Pos - ScreenPos;
        this->SendMessage(STL::PROM::MOUSE, &MouseData);

        if (MouseData.LeftHeld)
        {
            ProcessHandler::FocusedProcess = this;
        }
    }
}

void Process::SendFamilyMessage(STL::PROM Message, STL::PROI Input)
{
    for (uint64_t i = 0; i < this->Children.Length(); i++)
    {
        this->Children[i]->SendFamilyMessage(Message, Input);
    }

    this->SendMessage(Message, Input);
}

void Process::SendMessage(STL::PROM Message, STL::PROI Input)
{
    ProcessHandler::ActiveProcess = this;

    STL::PROR NewRequest = this->Procedure(Message, Input);

    ProcessHandler::ActiveProcess = nullptr;

    this->PushRequest(NewRequest);
}

void Process::PushRequest(STL::PROR Request)
{
    if (Request != STL::PROR::SUCCESS && RequestAmount < 16)
    {
        Requests[RequestAmount] = Request;
        RequestAmount++;
    }
}

void Process::Adopt(Process* Child)
{
    if (Child->Parent != nullptr)
    {
        return;
    }

    Child->Parent = this;
    this->Children.Push(Child);
}

Process* Process::FindFamilyMember(const char* TargetTitle)
{
    if (this->Title == TargetTitle)
    {
        return this;
    }

    for (uint32_t i = 0; i < this->Children.Length(); i++)
    {
        if (this->Children[i]->Title == TargetTitle)
        {
            return this->Children[i];
        }
        else
        {
            Process* Temp = this->Children[i]->FindFamilyMember(TargetTitle);
            if (Temp != nullptr)
            {
                return Temp;
            }
        }
    }

    return nullptr;
}

void Process::Kill()
{
    this->SendMessage(STL::PROM::KILL, nullptr);

    Heap::Free(FrameBuffer.Base);

    if (this->Parent != nullptr)
    {
        int64_t Index = this->Parent->Children.Find(this);
        if (Index != -1)
        {
            this->Parent->Children.Erase(Index);
        }
    }
    if (this == ProcessHandler::ActiveProcess)
    {
        ProcessHandler::ActiveProcess = nullptr;
    }
    if (this == ProcessHandler::GrandFatherProcess)
    {
        ProcessHandler::GrandFatherProcess = nullptr;
    }   
    if (this == ProcessHandler::FocusedProcess)
    {
        ProcessHandler::FocusedProcess = nullptr;
    }
    
    Heap::Free(this);
}

STL::PROR Process::PopRequest()
{
    if (RequestAmount > 0)
    {        
        RequestAmount--;
        return Requests[RequestAmount];
    }
    return STL::PROR::SUCCESS;
}

Process::Process(STL::PROC Procedure, Process* Parent)
{
    this->Procedure = Procedure;

    STL::PINFO Info;
    this->SendMessage(STL::PROM::INIT, &Info);    
    
    this->Title = Info.Title;

    this->FrameSize = STL::Point(0, 0);
    this->RenderFrameFunction = nullptr;

    this->RequestAmount = 0;

    switch(Info.Type)
    {
    case STL::PROT::GRANDFATHER:
    {
        this->Pos = STL::Point(0, 0);
        this->FrameBuffer.Height = Renderer::GetScreenSize().Y;  
        this->FrameBuffer.Width = Renderer::GetScreenSize().X;
        this->FrameBuffer.PixelsPerScanline = this->FrameBuffer.Width + 1;
        this->FrameBuffer.Size = (this->FrameBuffer.Height + 1) * this->FrameBuffer.PixelsPerScanline * 4;
        this->FrameBuffer.Base = (STL::ARGB*)Heap::Allocate(this->FrameBuffer.Size);        
        
        this->FrameBuffer.Clear();
        this->PushRequest(STL::PROR::DRAW);

        this->Parent = nullptr;
        if (ProcessHandler::GrandFatherProcess != nullptr)
        {
            ProcessHandler::GrandFatherProcess->Kill();
        }
        ProcessHandler::GrandFatherProcess = this;
    }
    break;
    case STL::PROT::WINDOWED:
    {
        this->FrameSize = FRAME_OFFSET + RAISED_WIDTH;
        this->RenderFrameFunction = RenderSimpleFrame;
    }
    case STL::PROT::FRAMELESSWINDOW:
    {        
        this->Pos = STL::Point(Info.Left, Info.Top);
        this->FrameBuffer.Height = Info.Height;  
        this->FrameBuffer.Width = Info.Width;
        this->FrameBuffer.PixelsPerScanline = Info.Width + 1;
        this->FrameBuffer.Size = (this->FrameBuffer.Height + 1) * this->FrameBuffer.PixelsPerScanline * 4;
        this->FrameBuffer.Base = (STL::ARGB*)Heap::Allocate(this->FrameBuffer.Size);

        this->FrameBuffer.Clear();
        this->PushRequest(STL::PROR::DRAW);        
        
        if (ProcessHandler::GrandFatherProcess == nullptr)
        {
            Debug::Error("Unable to adopt process");
        }
        else
        {
            if (Parent == nullptr)
            {
                ProcessHandler::GrandFatherProcess->Adopt(this);
            }
            else
            {
                Parent->Adopt(this);
            }
        }
    }
    break;
    }

    ProcessHandler::FocusedProcess = this;
} 