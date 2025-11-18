#pragma once
#include "ComponentRegistry.h"
#include <nlohmann/json.hpp>
#include <string>

#define GET_COMPONENT_TYPE_NAME(ComponentClass) \
    std::string GetComponentTypeName() const override { return #ComponentClass; }

#define REGISTER_COMPONENT_FROM_JSON(ComponentClass) \
    REGISTER_COMPONENT_FROM_JSON_WITH_UI(ComponentClass, #ComponentClass)

// Макрос для реєстрації з можливістю видалення (для звичайних компонентів)
#define REGISTER_COMPONENT_FROM_JSON_WITH_UI(ComponentClass, DisplayName) \
    namespace { \
        struct ComponentClass##Registrar { \
            ComponentClass##Registrar() { \
                ComponentRegistry::Instance().Register(#ComponentClass, ComponentClass::CreateFromJson); \
                ComponentRegistry::Instance().RegisterSerializer(#ComponentClass, [](const ObjectComponent* comp) { \
                    return static_cast<const ComponentClass*>(comp)->SerializeToJson(); \
                }); \
                ComponentRegistry::Instance().RegisterForUI( \
                    #ComponentClass, \
                    DisplayName, \
                    [](GameObject* obj) { obj->AddComponent<ComponentClass>(); }, \
                    [](GameObject* obj) { return obj->GetComponent<ComponentClass>() != nullptr; }, \
                    [](GameObject* obj) { obj->RemoveComponent<ComponentClass>(); } \
                ); \
            } \
        }; \
        static ComponentClass##Registrar g_##ComponentClass##Registrar; \
    }

// Макрос для реєстрації без можливості видалення (для Transform)
#define REGISTER_COMPONENT_FROM_JSON_WITH_UI_NO_REMOVE(ComponentClass, DisplayName) \
    namespace { \
        struct ComponentClass##Registrar { \
            ComponentClass##Registrar() { \
                ComponentRegistry::Instance().Register(#ComponentClass, ComponentClass::CreateFromJson); \
                ComponentRegistry::Instance().RegisterSerializer(#ComponentClass, [](const ObjectComponent* comp) { \
                    return static_cast<const ComponentClass*>(comp)->SerializeToJson(); \
                }); \
                ComponentRegistry::Instance().RegisterForUI( \
                    #ComponentClass, \
                    DisplayName, \
                    [](GameObject* obj) { obj->AddComponent<ComponentClass>(); }, \
                    [](GameObject* obj) { return obj->GetComponent<ComponentClass>() != nullptr; }, \
                    [](GameObject* obj) { /* Transform cannot be removed */ } \
                ); \
            } \
        }; \
        static ComponentClass##Registrar g_##ComponentClass##Registrar; \
    }
