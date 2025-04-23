#include "Engine/Engine.h"

int main() {
    Engine::Initialize(ExecutionMode::Release);
    Engine::Run();
    return 0;
}