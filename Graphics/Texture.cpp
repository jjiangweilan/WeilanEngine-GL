#include "Texture.hpp"
#include "../Settings.hpp"
namespace wlEngine {
    Texture::Texture(const std::string& file) {
        loadFromFile(file);
    }
    Texture* Texture::loadFromFTBitmap(const FT_GlyphSlot& glyph){
        free();
        sourcePath = "load from FTBitmap"; 
		mWidth = glyph->bitmap.width;
		mHeight = glyph->bitmap.rows;

#if SETTINGS_GAME_DIMENSION == 0
		float width = mWidth;
		float height = mHeight;
#else
        assert(0 && "no 3d considered");
		float width = rect->width / noralizationPara;
		float height = rect->height / normalizationPara;
#endif

		vertices[0] = glyph->bitmap_left + glyph->bitmap.width;
		vertices[1] = glyph->bitmap_top;
		vertices[5] = glyph->bitmap_left + glyph->bitmap.width;
		vertices[6] = -int(glyph->bitmap.rows - glyph->bitmap_top);
		vertices[10] = glyph->bitmap_left;
		vertices[11] = -int(glyph->bitmap.rows - glyph->bitmap_top);
		vertices[15] = glyph->bitmap_left;
		vertices[16] = glyph->bitmap_top;

		//top right
		vertices[8] = 1;
		vertices[9] = 1;

		//bottom right
		vertices[3] = 1;
		vertices[4] = 0;

		//bottom left
		vertices[18] = 0;
		vertices[19] = 0;

		//top left
		vertices[13] = 0;
		vertices[14] = 1;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// Generate texture
		glGenTextures(1, &mTexture);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

        glBindTexture(GL_TEXTURE_2D ,0);
		return this;
    }

    Texture* Texture::loadFromFile(const std::string& path) {
        free();
        sourcePath = path;

        unsigned char* imageData = stbi_load(path.c_str(), &mWidth, &mHeight, &nrChannel, 0);
        load(imageData);

        return this;
    }

    void Texture::load(unsigned char* imageData) {
        normalizationPara = mWidth > mHeight ? mWidth : mHeight; // for 3d

        Rect rect(0, 0, mWidth, mHeight);
        clip(&rect, false);
        unsigned int glColorChannel = nrChannel == 4 ? GL_RGBA : GL_RGB;

        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        if (imageData) {
            glGenTextures(1, &mTexture);
            glBindTexture(GL_TEXTURE_2D, mTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, glColorChannel, mWidth, mHeight, 0, glColorChannel, GL_UNSIGNED_BYTE, imageData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(imageData);
        }
#ifdef DEBUG
        else std::cerr << "Texture: no image data source: " << sourcePath << std::endl;
#endif
    }

    void Texture::free() {
        glDeleteTextures(1, &mTexture);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VAO);

        mTexture = 0;
        VBO = 0;
        EBO = 0;
        VAO = 0;
    }

    void Texture::clip(Rect* rect, bool subData){
        this->rect = *rect;
#if SETTINGS_GAME_DIMENSION == 0
        float width = rect->width;
        float height = rect->height;
#else
        float width = rect->width / noralizationPara;
        float height = rect->height / normalizationPara;
#endif

        vertices[0] = width / 2;
        vertices[1] = height / 2;
        vertices[5] = width / 2;
        vertices[6] = -height / 2;
        vertices[10] = -width / 2;
        vertices[11] = -height / 2;
        vertices[15] = -width / 2;
        vertices[16] = height / 2;

        //top right
        vertices[3] = (float)(rect->x + rect->width) / mWidth;
        vertices[4] = (float)(rect->y + rect->height) / mHeight;       

        //bottom right
        vertices[8] = (float)(rect->x + rect->width) / mWidth;
        vertices[9] = (float)(rect->y) / mHeight;

        //bottom left
        vertices[13] = (float)(rect->x) / mWidth;
        vertices[14] = (float)(rect->y) / mHeight;

        //top left
        vertices[18] = (float)(rect->x) / mWidth;
        vertices[19] = (float)(rect->y + rect->height) / mHeight;

        if (subData) {
            //update to opengl
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        }
    }

	void Texture::clipAndScaleFreeTypeBitmap(Rect* rect) {
		
	}

    void Texture::clipReset(){
        Rect rect(0, 0, mWidth, mHeight);
        clip(&rect, true);
    }

    Texture::~Texture() {
        free();
    }
}
