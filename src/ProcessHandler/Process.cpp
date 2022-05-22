#include "Process.h"
#include "ProcessHandler.h"

#include "Memory/Heap.h"
#include "Renderer/Renderer.h"

void Process::Render(STL::Framebuffer* Buffer)
{
    if (this->Pos.X < 0 || this->Pos.X + this->FrameBuffer.Width > Buffer->Width || this->Pos.Y < 0 || this->Pos.Y + this->FrameBuffer.Height > Buffer->Height)
    {
        return;
    }

    void* Source = (uint8_t*)(this->FrameBuffer.Base);
    void* Dest = (uint8_t*)(Buffer->Base + this->Pos.X + Buffer->PixelsPerScanline * this->Pos.Y);
    for (uint32_t y = 0; y < this->FrameBuffer.Height; y++)
    {             
        STL::CopyMemory(Source, Dest, this->FrameBuffer.PixelsPerScanline * 4);
        Source = (void*)((uint64_t)Source + this->FrameBuffer.PixelsPerScanline * 4);
        Dest = (void*)((uint64_t)Dest + Buffer->PixelsPerScanline * 4);   
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
        //Compositor::Update(i);
    }
    break;
    case STL::PROR::DRAW:
    {
        this->SendMessage(STL::PROM::DRAW, &this->FrameBuffer);
        //Compositor::Update(i);
    }
    break;
    case STL::PROR::KILL:
    {
        this->Kill();
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
    if (this->Type != STL::PROT::MINIMIZED && Request != STL::PROR::SUCCESS && RequestAmount < 16)
    {
        Requests[RequestAmount] = Request;
        RequestAmount++;
    }
}

STL::PROR Process::PopRequest()
{
    if (this->Type != STL::PROT::MINIMIZED && RequestAmount > 0)
    {        
        RequestAmount--;
        return Requests[RequestAmount];
    }   
    return STL::PROR::SUCCESS;
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

    delete this;
}

Process::Process(STL::PROC Procedure, Process* Parent)
{
    this->Procedure = Procedure;

    STL::PINFO Info;
    this->SendMessage(STL::PROM::INIT, &Info);    
    
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
    }

    ProcessHandler::FocusedProcess = this;
} 