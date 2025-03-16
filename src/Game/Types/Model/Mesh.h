#pragma once
#include <glad/glad.h>
#include <assimp/scene.h>
#include "Vertex.h"
#include <vector>
#include "../../../Utils/Utils.h"

class Mesh {
private:
    static constexpr int ABSENT_MATERIAL = -1;

    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::string name;
    int material;

    GLuint vbo;
    GLuint ebo;
    GLuint vao;

    bool hasNormals, hasTangents;

    void ParseExternalMesh(const aiMesh &srcMesh);
    void SetUpBuffers();
public:
    explicit Mesh(const aiMesh &srcMesh, int material = ABSENT_MATERIAL);
    NODISCARD GLuint GetHandle() const;
    NODISCARD std::string GetName() const;
    NODISCARD int GetMaterial() const;
};