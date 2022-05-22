#pragma once

#include "STL/List/List.h"
#include "STL/Process/Process.h"
#include "STL/Graphics/Framebuffer.h"

class Process
{
public:

    void Render(STL::Framebuffer* Buffer);

    void HandleRequest();

    void SendFamilyMessage(STL::PROM Message, STL::PROI Input = nullptr);

    void SendMessage(STL::PROM Message, STL::PROI Input = nullptr);

    void PushRequest(STL::PROR Request);

    Process(STL::PROC Procedure, Process* Parent);

private:

    void Kill();

    STL::PROR PopRequest();

    STL::Point Pos;

    STL::PROT Type;
    STL::PROC Procedure;
    STL::Framebuffer FrameBuffer;
    
    uint64_t RequestAmount;
    STL::PROR Requests[16];

    STL::List<Process*> Children;
    Process* Parent;
};