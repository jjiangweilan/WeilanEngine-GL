#pragma once

#include <iostream>
#include <fstream>
#include "Component.hpp"
#include "../Graphics/Texture.hpp"
#include "../Graphics/Shader.hpp"
#include "../Utility/Utility.hpp"

namespace wlEngine {
	
	enum class Language {
		Chinese, Japanese, Korean, English
	};
	class Text : public Component{
		COMPONENT_DECLARATION(Component, Text, 32);
	public:
		Text(Entity* go);

		/* functions ************************************/
		void loadFromFile(const std::string& fileName);
		void loadText(const std::wstring& text);
        /**
         * @brief load text and calculate the Y offset using maxLineWidth and lineSpce
         *
         * @param textStr
         * @param maxLineWidth
         * @param lineSpace
         * @param charWidth
         * @param charHeight
         */
		void loadText(const std::wstring& textStr, const int& maxLineWidth, const int& lineSpace, const int& charWidth, const int& charHeight);
		void loadText(const std::string& textStr, const int& maxLineWidth, const int& lineSpace, const int& charWidth, const int& charHeight);
		
		/** Configure Text*/
		void setLanguage(const Language& choice);
		/* structs *************************************/
		struct Character {
            Character(Texture* t, const int& x, const int& rows);
			Texture* texture;
			int x;
			int offsetY;
            glm::mat4 getTextTransform();
		};

		/* animated text ******************/
		bool animated;
		bool renderStarted;
		float animationSpeed;
		float timePassed;
		size_t renderUntilCharacter();
	private:
        Shader* shader;
		
		std::vector<Character> text;
		int textRenderIndex;
		std::wstring textString;
		Language language;
        friend class RenderSystem;

		/* Functions ********************************/
		void loadEnglish(const std::wstring& textStr, const int& maxLineWidth, const int& lineSpace, const int& charWidth, const int& charHeight);
		void loadChinese(const std::wstring& textStr, const int& maxLineWidth, const int& lineSpace, const int& charWidth, const int& charHeight);
	};
}
