#include "Engine/Engine.h"

int main() {
    Engine::Initialize(ExecutionMode::Debug);
    Engine::Run();
    return 0;
}