#include "Process.h"
#include "ProcessHandler.h"
#include "Compositor.h"

#include "Kernel/Memory/Heap.h"
#include "Kernel/Renderer/Renderer.h"
#include "Kernel/Debug/Debug.h"

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

    if (this->WindowFrame.RenderFunction != nullptr)
    {
        this->WindowFrame.RenderFunction(Buffer, ScreenPos, STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height), this->Title, this == ProcessHandler::FocusedProcess);
    }

    for (int i = 0; i < this->WindowButtons.Length(); i++)
    {
        this->WindowButtons[i].RenderFunction(Buffer, ScreenPos, STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height));
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

void Process::ProcessMouseData(STL::MDATA MouseData, STL::Point DomainOrigin)
{    
    static STL::Point PrevMousePos = STL::Point(0, 0);
    static Process* MovingWindow = nullptr;
    if (MovingWindow != nullptr)
    {
        if (!MouseData.LeftHeld)
        {
            MovingWindow = nullptr;
        }
        
        MovingWindow->SetPos(MovingWindow->Pos + (MouseData.Pos - PrevMousePos));
        PrevMousePos = MouseData.Pos;
        Compositor::RequestRender();

        return;
    }

    STL::Point ScreenPos = DomainOrigin + this->Pos;
    STL::Point TopLeft = ScreenPos - this->WindowFrame.TopLeftOffset;
    STL::Point BottomRight = ScreenPos + STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height) + this->WindowFrame.BottomRightOffset;

    if (STL::Contains(TopLeft, BottomRight, MouseData.Pos)) //If mouse over program
    {
        for (uint64_t i = this->Children.Length(); i --> 0; )
        {
            STL::Point ChildScreenPos = ScreenPos + this->Children[i]->Pos;
            STL::Point ChildTopLeft = ChildScreenPos - this->Children[i]->WindowFrame.TopLeftOffset;
            STL::Point ChildBottomRight = ChildScreenPos + STL::Point(this->Children[i]->FrameBuffer.Width, this->Children[i]->FrameBuffer.Height) + this->Children[i]->WindowFrame.BottomRightOffset;    
            
            if (STL::Contains(ChildTopLeft, ChildBottomRight, MouseData.Pos))
            {            
                this->Children[i]->ProcessMouseData(MouseData, ScreenPos);
                return;
            }
        }

        if (MouseData.LeftHeld)
        {          
            ProcessHandler::FocusedProcess = this; 
            
            for (int i = 0; i < this->WindowButtons.Length(); i++)
            {
                STL::Point ButtonPos = ScreenPos + STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height) * this->WindowButtons[i].Pos + this->WindowButtons[i].Offset;
                            
                if (STL::Contains(ButtonPos, ButtonPos + this->WindowButtons[i].Size, MouseData.Pos)) //If mouse on button
                {
                    this->PushRequest(this->WindowButtons[i].Request);
                    Compositor::RequestRender();
                    return;
                }
            }

            if (!STL::Contains(ScreenPos, ScreenPos + STL::Point(this->FrameBuffer.Width, this->FrameBuffer.Height), MouseData.Pos)) //If mouse on frame
            {
                this->MoveToFront();
                MovingWindow = this;
                PrevMousePos = MouseData.Pos;
                return;
            }
        }

        MouseData.Pos = MouseData.Pos - ScreenPos;
        this->SendMessage(STL::PROM::MOUSE, &MouseData);
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
        Debug::Error("Unable to adopt process");
    }

    Child->Parent = this;
    this->Children.Push(Child);
}

void Process::MoveToFront()
{
    if (this->Parent != nullptr)
    {
        for (int i = 0; i < this->Parent->Children.Length() - 1; i++)
        {
            if (this->Parent->Children[i] == this)
            {
                Process* Temp = this->Parent->Children.Last();
                this->Parent->Children.Last() = this;
                this->Parent->Children[i] = Temp;
                return;
            }
        }
    }
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

void Process::SetPos(STL::Point NewPos)
{
    this->Pos = NewPos;

    STL::Point ScreenSize = Renderer::GetScreenSize();
    this->Pos.X = STL::Clamp(this->Pos.X, (int32_t)this->WindowFrame.TopLeftOffset.X, (int32_t)(ScreenSize.X - this->FrameBuffer.Width - (int32_t)this->WindowFrame.BottomRightOffset.X));
    this->Pos.Y = STL::Clamp(this->Pos.Y, (int32_t)this->WindowFrame.TopLeftOffset.Y, (int32_t)(ScreenSize.Y - this->FrameBuffer.Height - (int32_t)this->WindowFrame.BottomRightOffset.Y));  
}

void Process::Kill()
{
    for (uint64_t i = 0; i < this->Children.Length(); i++)
    {
        this->Children[i]->Kill();
    }

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

    Compositor::RequestRender();
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
    this->WindowFrame = Info.Frame;
    this->RequestAmount = 0;
    this->Parent = nullptr;

    for (int i = 0; i < Info.FrameButtons.Length(); i++)
    {
        this->WindowButtons.Push(Info.FrameButtons[i]); 
    }

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
    case STL::PROT::CHILD:
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