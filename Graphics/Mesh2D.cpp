#include "Mesh2D.hpp"
#include "../Manager/ResourceManager.hpp"

namespace wlEngine
{
Mesh2D::Mesh2D()
{
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
};
Mesh2D::~Mesh2D()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh2D::addTexture(const std::string &file)
{
    auto rm = ResourceManager::get();
    m_textures.emplace_back(rm->getTexture(file));

    //update vertices update according to the first texture (the main texture)
    if (m_textures.size() == 1)
    {
        Rect m_rect(0, 0, m_textures[0]->getWidth(), m_textures[0]->getHeight());
        clip(&m_rect);
    }
}

void Mesh2D::addTexture(Texture *texture)
{
    m_textures.push_back(texture);

	if (m_textures.size() == 1)
	{
		Rect m_rect(0, 0, m_textures[0]->getWidth(), m_textures[0]->getHeight());
		clip(&m_rect);
	}
}
void Mesh2D::clip(Rect *rect) const
{
    this->m_rect = *rect;

    float width = rect->width;
    float height = rect->height;

    m_vertices[0] = width / 2;
    m_vertices[1] = height / 2;
    m_vertices[5] = width / 2;
    m_vertices[6] = -height / 2;
    m_vertices[10] = -width / 2;
    m_vertices[11] = -height / 2;
    m_vertices[15] = -width / 2;
    m_vertices[16] = height / 2;


    const int &tWidth = width;
    const int &tHeight = height;
    //top right
    m_vertices[3] = (float)(rect->x + rect->width) / tWidth;
    m_vertices[4] = (float)(rect->y + rect->height) / tHeight;

    //bottom right
    m_vertices[8] = (float)(rect->x + rect->width) / tWidth;
    m_vertices[9] = (float)(rect->y) / tHeight;

    //bottom left
    m_vertices[13] = (float)(rect->x) / tWidth;
    m_vertices[14] = (float)(rect->y) / tHeight;

    //top left
    m_vertices[18] = (float)(rect->x) / tWidth;
    m_vertices[19] = (float)(rect->y + rect->height) / tHeight;

    //update to opengl
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices);
}

void Mesh2D::changeTexture(const int &loc, Texture *t)
{
	if (m_textures.size() == 0) addTexture(t);
	else
	{
		m_textures[loc] = t;
		if (loc == 0)
		{
			Rect rect(0, 0, t->getWidth(), t->getHeight());
			clip(&rect);
		}
	}
}

const Rect* Mesh2D::getClipRect() const { return &m_rect; }
const GLuint &Mesh2D::getVAO() const { return m_VAO; }
const std::vector<Texture *> *Mesh2D::getTextures() const
{
    return &m_textures;
}
} // namespace wlEngine