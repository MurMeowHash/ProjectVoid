#pragma once

struct IDisposable {
    virtual void Dispose() = 0;
    virtual ~IDisposable() = default;
};