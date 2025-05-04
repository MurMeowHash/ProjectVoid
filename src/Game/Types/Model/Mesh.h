#pragma once
#include <glad/glad.h>
#include <assimp/scene.h>
#include "Vertex.h"
#include <vector>
#include "../../../Utils/Utils.h"
#include "../../../Dispose/IDisposable.h"
#include "../BoundingBox/AABB.h"

class Mesh : public IDisposable {
private:
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::string name;
    int material;

    GLuint vbo;
    GLuint ebo;
    GLuint vao;

    bool hasNormals, hasTangents;

    AABB aabb;

    void ParseExternalMesh(const aiMesh &srcMesh);
    void SetUpBuffers();
public:
    explicit Mesh(const aiMesh &srcMesh, int material = ABSENT_RESOURCE);
    Mesh(std::string name, const std::vector<Vertex> &vertices, const std::vector<uint> &indices,
         bool hasNormals = false, bool hasTangents = false);
    void SetMaterial(int matIndex);
    NODISCARD GLuint GetHandle() const;
    NODISCARD std::string GetName() const;
    NODISCARD int GetMaterial() const;
    NODISCARD uint GetIndicesCount() const;
    NODISCARD const AABB& GetAABB() const;

    void Dispose() override;
};