#include "Engine/Engine.h"

int main() {
    Engine::Initialize(ExecutionMode::Debug);
    Engine::Walk();
    return 0;
}
