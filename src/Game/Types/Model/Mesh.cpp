#include "Mesh.h"

Mesh::Mesh(const aiMesh &srcMesh, int material)
: material{material} {
    ParseExternalMesh(srcMesh);
    SetUpBuffers();
}

void Mesh::ParseExternalMesh(const aiMesh &srcMesh) {
    name = srcMesh.mName.C_Str();
    vertices.reserve(srcMesh.mNumVertices);
    for(uint i = 0; i < srcMesh.mNumVertices; i++) {
        Vertex transformedVertex;
        transformedVertex.position = Utils::ToGLMVector(srcMesh.mVertices[i]);
        hasNormals = srcMesh.HasNormals();
        if(hasNormals) {
            transformedVertex.normal = Utils::ToGLMVector(srcMesh.mNormals[i]);
        }
        hasTangents = srcMesh.HasTangentsAndBitangents();
        if(hasTangents) {
            transformedVertex.tangent = Utils::ToGLMVector(srcMesh.mTangents[i]);
            transformedVertex.bitangent = Utils::ToGLMVector(srcMesh.mBitangents[i]);
        }

        if(hasTangents
        && hasNormals) {
            transformedVertex.tangent = transformedVertex.tangent -
                    glm::dot(transformedVertex.normal, transformedVertex.tangent) * transformedVertex.normal;
            transformedVertex.bitangent = glm::cross(transformedVertex.normal, transformedVertex.tangent);
        }

        if(srcMesh.HasTextureCoords(0)) {
            transformedVertex.texCoords = Utils::ToGLMVector(srcMesh.mTextureCoords[0][i]);
        }

        vertices.push_back(transformedVertex);
    }

    for(uint i = 0; i < srcMesh.mNumFaces; i++) {
        aiFace face = srcMesh.mFaces[i];
        for(uint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
}

void Mesh::SetUpBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint)),
                 indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, tangent)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, bitangent)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Mesh::GetHandle() const {
    return vao;
}

std::string Mesh::GetName() const {
    return name;
}

int Mesh::GetMaterial() const {
    return material;
}