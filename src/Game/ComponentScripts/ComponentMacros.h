#pragma once
#include "ComponentRegistry.h"
#include <nlohmann/json.hpp>

#define GET_COMPONENT_TYPE_NAME(ComponentClass) \
    std::string GetComponentTypeName() const override { return #ComponentClass; }

#define REGISTER_COMPONENT_FROM_JSON(ComponentClass) \
    namespace { \
        struct ComponentClass##Registrar { \
            ComponentClass##Registrar() { \
                ComponentRegistry::Instance().Register(#ComponentClass, ComponentClass::CreateFromJson); \
                ComponentRegistry::Instance().RegisterSerializer(#ComponentClass, [](const ObjectComponent* comp) { \
                    return static_cast<const ComponentClass*>(comp)->SerializeToJson(); \
                }); \
            } \
        }; \
        static ComponentClass##Registrar g_##ComponentClass##Registrar; \
    }
