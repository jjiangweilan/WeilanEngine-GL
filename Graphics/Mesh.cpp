#include "Mesh.hpp"

namespace wlEngine {
    Mesh::Mesh(std::vector<Texture3D>& textures, std::vector<GLuint>& indices, std::vector<Vertex>& vertices): textures(textures), indices(indices), vertices(vertices) {
		EBO = 0;
		VAO = 0;
		VBO = 0;
		setupMesh(); 
    }
    Mesh::Mesh(std::vector<Texture3D>&& textures, std::vector<GLuint>&& indices, std::vector<Vertex>&& vertices) : textures(std::move(textures)), indices(std::move(indices)), vertices(std::move(vertices)) {
		EBO = 0;
		VAO = 0;
		VBO = 0;
		setupMesh();
    }
    void Mesh::setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW); 

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

        glBindVertexArray(0);
    }

    Mesh::Mesh(Mesh&& mesh) noexcept {
        this->vertices = std::move(mesh.vertices);
        this->indices = std::move(mesh.indices);
        this->textures = std::move(mesh.textures);
        this->EBO = mesh.EBO;
        this->VAO = mesh.VAO;
        this->VBO = mesh.VBO;
    }
}
