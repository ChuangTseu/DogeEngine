#include "mesh.h"

#include <cassert>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/config.h>

bool Mesh::loadFromAssimpMesh(const aiMesh *mesh) {
    m_vertices.clear();
    m_indices.clear();

    m_materialIndex = mesh->mMaterialIndex;

    assert( mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

    for (unsigned int idFace = 0; idFace < mesh->mNumFaces; ++idFace) {
        const aiFace& triangle = mesh->mFaces[idFace];

        for (unsigned int idIndex = 0; idIndex < 3; ++idIndex) {
            m_indices.push_back(triangle.mIndices[idIndex]);
        }
    }

    assert( mesh->HasNormals() );
//    assert( mesh->HasTangentsAndBitangents() );

    for (unsigned int idVertex = 0; idVertex < mesh->mNumVertices; ++idVertex) {
        const aiVector3D& vertex = mesh->mVertices[idVertex];
        const aiVector3D& normal = mesh->mNormals[idVertex];

        Vertex v;
        v.position.x = vertex.x;
        v.position.y = vertex.y;
        v.position.z = vertex.z;

        v.normal.x = normal.x;
        v.normal.y = normal.y;
        v.normal.z = normal.z;

        if (mesh->HasTangentsAndBitangents()) {
            const aiVector3D& texcoord = (mesh->HasTextureCoords(0) ?
                                              mesh->mTextureCoords[0][idVertex] : aiVector3D{0.f, 0.f, 0.f});

            const aiVector3D& tangent = mesh->mTangents[idVertex];

            v.texcoord.x = texcoord.x;
            v.texcoord.y = texcoord.y;

            v.tangent.x = tangent.x;
            v.tangent.y = tangent.y;
            v.tangent.z = tangent.z;
        }



        //            const aiVector3D& aiBitangent = mesh->mBitangents[idVertex];

        //        vec3 bitangent{aiBitangent.x, aiBitangent.y, aiBitangent.z};

        //        std::cerr << "normal " << idVertex << ": " << v.normal << ' ';
        //        std::cerr << "tangent " << idVertex << ": " << v.tangent << ' ';
        //        std::cerr << "bitangent " << idVertex << ": " << bitangent << ' ';
        //        std::cerr << "cross " << idVertex << ": " << cross(v.tangent, v.normal) << '\n';

        m_vertices.push_back(v);
    }

    m_vbo.submitData(m_vertices.data(), m_vertices.size());
    m_ibo.submitData(m_indices.data(), m_indices.size());

    m_vao.bind();

    m_vbo.bind();
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0));
    GL(glEnableVertexAttribArray(0));

    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(sizeof(vec3))));
    GL(glEnableVertexAttribArray(1));

    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(vec3))));
    GL(glEnableVertexAttribArray(2));

    GL(glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(vec3) + sizeof(vec2))));
    GL(glEnableVertexAttribArray(3));

    m_ibo.bind();

    VAO::unbind();

    VBO::unbind();
    IBO::unbind();

    return true;
}

void Mesh::draw() const
{
    drawAsTriangles();
}

void Mesh::drawAsTriangles() const
{
    m_vao.bind();

    GL(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr));

    VAO::unbind();
}

void Mesh::drawAsPatch() {
    m_vao.bind();

    GL(glPatchParameteri(GL_PATCH_VERTICES, 3));

    GL(glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_INT, nullptr));

    VAO::unbind();
}

bool Mesh::loadFullscreenQuad() {
    m_vertices.clear();
    m_indices.clear();

    std::vector<vec3> quadVertices;
    std::vector<GLuint> quadIndices;

    quadVertices = std::vector<vec3>{vec3{-1.f, -1.f, 0.f}, vec3{1.f, -1.f, 0.f}, vec3{1.f, 1.f, 0.f}, vec3{-1.f, 1.f, 0.f}};
    quadIndices = std::vector<GLuint>{2, 1, 0,
                                      3, 2, 0};

    m_vbo.submitData(quadVertices.data(), quadVertices.size());
    m_ibo.submitData(quadIndices.data(), quadIndices.size());

    m_indices = std::move(quadIndices);

    m_vao.bind();

    m_vbo.bind();
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
    GL(glEnableVertexAttribArray(0));

    m_ibo.bind();

    VAO::unbind();

    VBO::unbind();
    IBO::unbind();

    return true;
}
