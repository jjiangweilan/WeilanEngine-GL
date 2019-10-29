#pragma once
#include <glad/glad.h>
#include <stb_image.hpp>

#include <wlEngine/Graphics/Shader.hpp>
#include <wlEngine/Component/Struct/Clip.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace wlEngine {
    struct Texture {
        public:
        Texture() = default;
        Texture(const std::string& file);
        ~Texture();
        Texture* loadFromFile(const std::string& path);
        Texture* loadFromFTBitmap(const FT_GlyphSlot& bitmap_FT);
		void free();

        void clip(Rect*, bool subData=true);
        void clipReset();
		void clipAndScaleFreeTypeBitmap(Rect*);

        public:
        unsigned int mTexture = 0;
        std::string sourcePath;

        int mWidth;
        int mHeight;
        int nrChannel;
        Rect rect;

        float normalizationPara;

		//opengl
		GLuint VAO = 0;
		GLuint VBO = 0;
		GLuint EBO = 0;
        
        float vertices[20] = {
            0.0f,  0.0f, 0.0f,  1.0f, 1.0f,   // top right
            0.0f,  0.0f, 0.0f,  1.0f, 0.0f,   // bottom right
            0.0f,  0.0f, 0.0f,  0.0f, 0.0f,   // bottom left
            0.0f,  0.0f, 0.0f,  0.0f, 1.0f    // top left 
        };

    private:
        void load(unsigned char* data);
    };
}
