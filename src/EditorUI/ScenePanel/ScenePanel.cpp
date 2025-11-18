#include "ScenePanel.h"
#include "../../Game/Scene/Scene.h"
#include "../../Game/Types/GameObject/GameObject.h"
#include "../../Game/ComponentScripts/Light/Light.h"
#include "../../Game/ComponentScripts/ComponentRegistry.h"
#include "../../Utils/Utils.h"
#include <functional>

ScenePanel::ScenePanel(const ImVec2& pos, const ImVec2& size)
    : EditorPanel("Scene", pos, size), selectedObjectName("") {
}

std::vector<ScenePanel::GameObjectNode> ScenePanel::BuildHierarchy() {
    std::vector<GameObjectNode> rootNodes;
    std::vector<std::pair<GameObjectNode, std::string>> nodeParentPairs;
    
    // Отримуємо всі об'єкти зі сцени
    for(int i = 0; i <= static_cast<int>(Scene::GetLastGameObjectIndex()); ++i) {
        auto* obj = Scene::GetGameObjectByIndex(i);
        if(!obj) continue;
        
        GameObjectNode node;
        node.name = obj->GetName();
        node.index = i;
        
        std::string parentName = obj->GetParentName();
        if(parentName.empty() || parentName == UNDEFINED_NAME) {
            // Кореневий вузол
            rootNodes.push_back(node);
        } else {
            // Зберігаємо зв'язок батько-дочка для пізнішої обробки
            nodeParentPairs.push_back({node, parentName});
        }
    }
    
    // Будуємо ієрархію
    for(const auto& [node, parentName] : nodeParentPairs) {
        // Функція для пошуку та додавання дочірнього вузла
        std::function<bool(std::vector<GameObjectNode>&)> addChild = [&](std::vector<GameObjectNode>& nodes) -> bool {
            for(auto& n : nodes) {
                if(n.name == parentName) {
                    n.children.push_back(node);
                    return true;
                }
                if(addChild(n.children)) {
                    return true;
                }
            }
            return false;
        };
        
        if(!addChild(rootNodes)) {
            // Якщо батько не знайдений, додаємо як кореневий
            rootNodes.push_back(node);
        }
    }
    
    return rootNodes;
}

bool ScenePanel::IsDescendant(const std::string& potentialParent, const std::string& child) const {
    // Перевіряємо чи child є нащадком potentialParent
    // Якщо так, то potentialParent не може стати батьком child (бо буде цикл)
    std::function<bool(const std::string&)> checkPath = [&](const std::string& objName) -> bool {
        if(objName == potentialParent) return true; // Знайшли potentialParent в ієрархії child
        
        auto* obj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(objName));
        if(!obj) return false;
        
        std::string parentName = obj->GetParentName();
        if(parentName.empty() || parentName == UNDEFINED_NAME) {
            return false; // Досягли кореня, не знайшли potentialParent
        }
        
        // Перевіряємо батьківський об'єкт
        return checkPath(parentName);
    };
    
    return checkPath(child);
}

bool ScenePanel::CanSetParent(const std::string& childName, const std::string& potentialParentName) const {
    // Об'єкт не може стати батьком самому собі
    if(childName == potentialParentName) return false;
    
    // Перевіряємо, чи potentialParent не є прямим нащадком child (щоб уникнути циклу)
    // Але дозволяємо встановити предка поточного батька (діда, прадіда тощо)
    auto* childObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(childName));
    if(!childObj) return false;
    
    // Якщо child не має батька, можна встановити будь-якого батька (крім самого себе)
    std::string currentParent = childObj->GetParentName();
    if(currentParent.empty() || currentParent == UNDEFINED_NAME) {
        // Перевіряємо тільки, чи potentialParent не є нащадком child
        return !IsDescendant(childName, potentialParentName);
    }
    
    // Якщо child має батька, можна встановити:
    // 1. Будь-якого предка поточного батька (діда, прадіда тощо)
    // 2. Будь-який інший об'єкт, який не є нащадком child
    
    // Перевіряємо, чи potentialParent є предком поточного батька
    std::function<bool(const std::string&)> isAncestor = [&](const std::string& objName) -> bool {
        if(objName == potentialParentName) return true; // Знайшли potentialParent як предка
        
        auto* obj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(objName));
        if(!obj) return false;
        
        std::string parentName = obj->GetParentName();
        if(parentName.empty() || parentName == UNDEFINED_NAME) {
            return false; // Досягли кореня
        }
        
        return isAncestor(parentName);
    };
    
    // Якщо potentialParent є предком поточного батька, дозволяємо
    if(isAncestor(currentParent)) {
        return true;
    }
    
    // Інакше перевіряємо, чи potentialParent не є нащадком child
    return !IsDescendant(childName, potentialParentName);
}

void ScenePanel::RenderHierarchyNode(const GameObjectNode& node) {
    auto* obj = Scene::GetGameObjectByIndex(node.index);
    if(!obj) return;
    
    // Створюємо унікальний ID для TreeNode
    ImGui::PushID(node.index);
    
    // Встановлюємо сірий hover колір для всіх об'єктів
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.70f));
    
    // Визначаємо стиль для вибраного об'єкта (сірий колір)
    bool isSelected = selectedObjectName == node.name;
    if(isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.5f, 0.5f, 0.5f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.90f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.7f, 0.7f, 0.7f, 1.00f));
    }
    
    // Визначаємо, чи є дочірні об'єкти
    bool hasChildren = !node.children.empty();
    
    // Відображаємо TreeNode без емоджі
    // Якщо немає дітей, використовуємо прапорець Leaf, щоб не показувати стрілочку
    ImGuiTreeNodeFlags flags = isSelected ? ImGuiTreeNodeFlags_Selected : 0;
    if(!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    
    bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);
    
    // Відновлюємо стилі
    if(isSelected) {
        ImGui::PopStyleColor(4); // Header, HeaderHovered (перезаписаний), HeaderActive для вибраного + базовий HeaderHovered
    } else {
        ImGui::PopStyleColor(1); // HeaderHovered для звичайного
    }
    
    // Обробка вибору об'єкта
    if(ImGui::IsItemClicked()) {
        selectedObjectName = node.name;
    }
    
    // Контекстне меню при правому кліку на об'єкт
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::MenuItem("Create GameObject")) {
            GameObjectParameters params;
            params.name = "GameObject";
            params.parentName = node.name;
            uint newObjectIndex = Scene::CreateGameObject(params);
            auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
            if(newObj) {
                selectedObjectName = newObj->GetName();
            }
        }
        
        // Підменю для додавання компонентів
        if(ImGui::BeginMenu("Add Component")) {
            auto* obj = Scene::GetGameObjectByIndex(node.index);
            if(obj) {
                // Використовуємо реєстр компонентів для автоматичного відображення всіх зареєстрованих компонентів
                const auto& uiComponents = ComponentRegistry::Instance().GetUIComponents();
                for(const auto& [typeName, uiInfo] : uiComponents) {
                    // Пропускаємо компоненти, які вже є на об'єкті
                    if(!uiInfo.hasComponentFunc(obj)) {
                        if(ImGui::MenuItem(uiInfo.displayName.c_str())) {
                            uiInfo.createFunc(obj);
                        }
                    }
                }
            }
            ImGui::EndMenu();
        }
        
        // Підменю для встановлення батька
        if(ImGui::BeginMenu("Set Parent")) {
            auto* obj = Scene::GetGameObjectByIndex(node.index);
            if(obj) {
                // Опція "None" для скидання батька
                bool isRoot = obj->GetParentName().empty() || obj->GetParentName() == UNDEFINED_NAME;
                if(ImGui::MenuItem("None", nullptr, isRoot)) {
                    obj->SetParentName(UNDEFINED_NAME);
                }
                
                ImGui::Separator();
                
                // Список всіх об'єктів як можливих батьків
                for(int i = 0; i <= static_cast<int>(Scene::GetLastGameObjectIndex()); ++i) {
                    auto* potentialParent = Scene::GetGameObjectByIndex(i);
                    if(!potentialParent || potentialParent == obj) continue;
                    
                    std::string parentName = potentialParent->GetName();
                    
                    // Перевіряємо, чи можна встановити цього батька
                    if(!CanSetParent(node.name, parentName)) continue;
                    
                    bool isCurrentParent = obj->GetParentName() == parentName;
                    
                    if(ImGui::MenuItem(parentName.c_str(), nullptr, isCurrentParent)) {
                        obj->SetParentName(parentName);
                    }
                }
            }
            ImGui::EndMenu();
        }
        
        if(ImGui::MenuItem("Delete")) {
            // Вибираємо об'єкт перед видаленням
            selectedObjectName = node.name;
            DeleteSelectedObject();
        }
        
        ImGui::EndPopup();
    }
    
    // Drag-and-Drop Source: початок перетягування об'єкта
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        // Встановлюємо payload з назвою об'єкта
        ImGui::SetDragDropPayload("GAMEOBJECT_NODE", node.name.c_str(), node.name.size() + 1);
        ImGui::Text("Move %s", node.name.c_str());
        ImGui::EndDragDropSource();
    }
    
    // Drag-and-Drop Target: можливість покласти об'єкт на цей
    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_NODE")) {
            std::string draggedName(static_cast<const char*>(payload->Data));
            
            // Перевірка: чи можна встановити цього батька
            if(CanSetParent(draggedName, node.name)) {
                auto* draggedObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(draggedName));
                if(draggedObj) {
                    draggedObj->SetParentName(node.name);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    
    // Відображаємо дочірні вузли
    if(nodeOpen && hasChildren) {
        for(const auto& child : node.children) {
            RenderHierarchyNode(child);
        }
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

void ScenePanel::Render() {
    RenderDefaultPanel();
    
    ImGui::SetCursorPosY(38.0f);
    
    if(!Scene::IsInitialized()) {
        ImGui::Text("Scene not initialized");
        ImGui::End();
        return;
    }
    
    // Обробка клавіші Delete для видалення вибраного об'єкта
    bool canDelete = !selectedObjectName.empty() && 
                     Scene::GetGameObjectIndexByName(selectedObjectName) != Scene::ABSENT_OBJECT;
    if(ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        if(!selectedObjectName.empty() && canDelete) {
            DeleteSelectedObject();
        }
    }
    
    // Відображаємо ієрархію об'єктів
    auto hierarchy = BuildHierarchy();
    
    // Перевіряємо, чи вибраний об'єкт ще існує
    if(!selectedObjectName.empty()) {
        int selectedIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
        if(selectedIndex == Scene::ABSENT_OBJECT) {
            // Об'єкт більше не існує, очищаємо вибір
            selectedObjectName = "";
        }
    }
    
    // Автоматично вибираємо перший об'єкт, якщо нічого не вибрано і є об'єкти
    if(selectedObjectName.empty() && !hierarchy.empty()) {
        selectedObjectName = hierarchy[0].name;
    }
    
    if(hierarchy.empty()) {
        ImGui::Text("No objects in scene");
        
        // Контекстне меню при правому кліку на порожнє місце (коли немає об'єктів)
        if(ImGui::BeginPopupContextWindow("SceneEmptyContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if(ImGui::MenuItem("Create GameObject")) {
                GameObjectParameters params;
                params.name = "GameObject";
                uint newObjectIndex = Scene::CreateGameObject(params);
                auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
                if(newObj) {
                    selectedObjectName = newObj->GetName();
                }
            }
            
            ImGui::EndPopup();
        }
        
        // Drag-and-Drop Target для скидання батьківського об'єкта (коли немає об'єктів)
        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_NODE")) {
                std::string draggedName(static_cast<const char*>(payload->Data));
                auto* draggedObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(draggedName));
                if(draggedObj) {
                    draggedObj->SetParentName(UNDEFINED_NAME);
                }
            }
            ImGui::EndDragDropTarget();
        }
    } else {
        ImGui::BeginChild("SceneHierarchy", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        
        for(const auto& rootNode : hierarchy) {
            RenderHierarchyNode(rootNode);
        }
        
        // Додаємо невидимий елемент після всіх об'єктів для drop target на пустий простір
        ImGui::Spacing();
        ImGui::InvisibleButton("EmptyDropTarget", ImVec2(-1, ImGui::GetTextLineHeight() * 2));
        
        // Drag-and-Drop Target для скидання батьківського об'єкта (перетягнути на пустий простір під об'єктами)
        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_NODE")) {
                std::string draggedName(static_cast<const char*>(payload->Data));
                auto* draggedObj = Scene::GetGameObjectByIndex(Scene::GetGameObjectIndexByName(draggedName));
                if(draggedObj) {
                    // Скидаємо батьківський об'єкт (встановлюємо UNDEFINED_NAME)
                    draggedObj->SetParentName(UNDEFINED_NAME);
                }
            }
            ImGui::EndDragDropTarget();
        }
        
        // Контекстне меню при правому кліку на Child вікно (порожнє місце)
        if(ImGui::BeginPopupContextWindow("SceneHierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if(ImGui::MenuItem("Create GameObject")) {
                GameObjectParameters params;
                params.name = "GameObject";
                uint newObjectIndex = Scene::CreateGameObject(params);
                auto* newObj = Scene::GetGameObjectByIndex(static_cast<int>(newObjectIndex));
                if(newObj) {
                    selectedObjectName = newObj->GetName();
                }
            }
            
            ImGui::EndPopup();
        }
        
        ImGui::EndChild();
    }
    
    ImGui::End();
}

std::string ScenePanel::GetSelectedObjectName() const {
    return selectedObjectName;
}

void ScenePanel::DeleteSelectedObject() {
    if(selectedObjectName.empty()) {
        return;
    }
    
    int objectIndex = Scene::GetGameObjectIndexByName(selectedObjectName);
    if(objectIndex == Scene::ABSENT_OBJECT) {
        return;
    }
    
    uint deletedIndex = static_cast<uint>(objectIndex);
    uint lastIndex = Scene::GetLastGameObjectIndex();
    Scene::RemoveGameObject(deletedIndex);
    
    // Автоматично вибираємо сусідній об'єкт після видалення
    selectedObjectName = "";
    if(Scene::GetLastGameObjectIndex() != Scene::ABSENT_OBJECT_NUMBER) {
        // Спробуємо вибрати наступний об'єкт (той, що мав індекс deletedIndex + 1)
        if(deletedIndex < lastIndex) {
            // Наступний об'єкт тепер має індекс deletedIndex
            auto* nextObj = Scene::GetGameObjectByIndex(static_cast<int>(deletedIndex));
            if(nextObj) {
                selectedObjectName = nextObj->GetName();
            }
        }
        
        // Якщо не вдалося вибрати наступний (був останнім), вибираємо попередній
        if(selectedObjectName.empty() && deletedIndex > 0) {
            auto* prevObj = Scene::GetGameObjectByIndex(static_cast<int>(deletedIndex - 1));
            if(prevObj) {
                selectedObjectName = prevObj->GetName();
            }
        }
    }
}
