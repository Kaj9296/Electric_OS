#pragma once

namespace InteruptHandlers
{
    struct InterruptFrame;

    /// <summary>
    /// Exception interrupt handlers.
    /// </summary>

    __attribute__((interrupt)) void DivideByZero(InterruptFrame* frame);

    __attribute__((interrupt)) void NoneMaskableInterrupt(InterruptFrame* frame);

    __attribute__((interrupt)) void Breakpoint(InterruptFrame* frame);

    __attribute__((interrupt)) void Overflow(InterruptFrame* frame);

    __attribute__((interrupt)) void BoundRange(InterruptFrame* frame);

    __attribute__((interrupt)) void InvalidOP(InterruptFrame* frame);

    __attribute__((interrupt)) void DeviceNotDetected(InterruptFrame* frame);

    __attribute__((interrupt)) void DoubleFault(InterruptFrame* frame);

    __attribute__((interrupt)) void InvalidTSS(InterruptFrame* frame);

    __attribute__((interrupt)) void SegmentNotPresent(InterruptFrame* frame);

    __attribute__((interrupt)) void StackSegmentFault(InterruptFrame* frame);

    __attribute__((interrupt)) void GeneralProtectionFault(InterruptFrame* frame);

    __attribute__((interrupt)) void PageFault(InterruptFrame* frame);

    __attribute__((interrupt)) void FloatingPoint(InterruptFrame* frame);

    /// <summary>
    /// IRQ interrupt handlers.
    /// </summary>

    __attribute__((interrupt)) void PIT(InterruptFrame* frame);

    __attribute__((interrupt)) void Keyboard(InterruptFrame* frame);

    __attribute__((interrupt)) void Mouse(InterruptFrame* frame);
}
