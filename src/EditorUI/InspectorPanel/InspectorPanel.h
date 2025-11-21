#pragma once
#include "../EditorPanel.h"
#include <imgui/imgui.h>
#include <string>
#include <glm/glm.hpp>

class GameObject;
class ObjectComponent;

class InspectorPanel : public EditorPanel {
private:
    std::string selectedObjectName;
    char nameBuffer[256];
    bool nameBufferDirty;

    glm::vec3 cachedPosition;
    glm::vec3 cachedRotation;
    glm::vec3 cachedScale;
    bool positionActive;
    bool rotationActive;
    bool scaleActive;

    void RenderObjectName(GameObject* obj);
    void RenderContextMenu(GameObject* obj);
    void RenderComponents(GameObject* obj);

    void RenderTransformComponent(ObjectComponent* component, GameObject* obj);
    void RenderMeshRenderDataComponent(ObjectComponent* component, GameObject* obj);
    void RenderLightComponent(ObjectComponent* component);
    void RenderRigidbodyComponent(ObjectComponent* component);
    void RenderColliderComponent(ObjectComponent* component);

    void UpdateTransformCache(ObjectComponent* transform);
    void RenderAddComponentMenu(GameObject* obj);

public:
    InspectorPanel(const ImVec2& pos, const ImVec2& size);
    void Render() override;
    void SetSelectedObject(const std::string& objectName);
};
