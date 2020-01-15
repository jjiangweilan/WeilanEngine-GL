#pragma once
#include "Component.hpp"
#include "Graphics/Character.hpp"
#include "Graphics/Shader.hpp"
#include "Utility/Utility.hpp"

#include <iostream>
#include <fstream>

namespace WeilanEngine
{

enum class Language
{
	Chinese,
	Japanese,
	Korean,
	English
};

class Text : public Component
{
	COMPONENT_DECLARATION(Component, Text, 32);

public:
	Text(Entity *go);

	/* functions ************************************/
	void loadFromFile(const std::string &fileName);
	void loadText(const std::wstring &text);
	/**
         * @brief load text and calculate the Y offset using maxLineWidth and lineSpce
         *
         * @param textStr
         * @param maxLineWidth
         * @param lineSpace
         * @param charWidth
         * @param charHeight
         */
	void loadText(const std::wstring &textStr, const int &maxLineWidth, const int &lineSpace, const int &charWidth, const int &charHeight);
	void loadText(const std::string &textStr, const int &maxLineWidth, const int &lineSpace, const int &charWidth, const int &charHeight);

	/** Configure Text*/
	void setLanguage(const Language &choice);
	/* structs *************************************/
	struct CharacterETX
	{
		CharacterETX(Graphics::Character *c, const int &x, const int &rows);
		Graphics::Character *character;
		int x;
		int offsetY;
		glm::mat4 getTextTransform();
	};

	/* animated text ******************/
	bool animated;
	bool renderStarted;
	float animationSpeed;
	size_t renderUntilCharacter();

	const Graphics::Shader *getShader() const;

	/**
	 * @brief Get the Time Passed object
	 * 
	 * @return const float& the time passed since the text started render
	 */
	const float &getTimePassed() const;

private:
	Graphics::Shader *m_shader;
	float m_timePassed;

	std::vector<CharacterETX> text;
	int textRenderIndex;
	std::wstring textString;
	Language language;
	friend class RenderSystem;

	/* Functions ********************************/
	void loadEnglish(const std::wstring &textStr, const int &maxLineWidth, const int &lineSpace, const int &charWidth, const int &charHeight);
	void loadChinese(const std::wstring &textStr, const int &maxLineWidth, const int &lineSpace, const int &charWidth, const int &charHeight);
};
} // namespace WeilanEngine
