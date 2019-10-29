#include "Text.hpp"
#include "../Time.hpp"
#include "../Manager/ResourceManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
namespace wlEngine {
	COMPONENT_DEFINATION(Component, Text, 32);
	Text::Text(Entity* entity) : Component(entity), language(Language::English), animated(true), textRenderIndex(0), renderStarted(false), timePassed(0), animationSpeed(0.02){
		shader = Shader::collection["Text"];
	}

	/* Text Loading ***********************************************************************/
	void Text::loadFromFile(const std::string& fileName) {

	}

	void Text::loadText(const std::wstring& textStr) {
        auto resourceManager = ResourceManager::get();
        int advance = 0;
		int offsetY = 0;
        for (const wchar_t& character : textStr) {
            if (character == L'\n') {
				offsetY -= 40; // note: 40 is a magic number! becuase we use FT_Set_Pixel_Sizes(face, 32, 32); in ResourceManager! Be aware!
				advance = 0;
                continue;
            }
            auto textInfo = resourceManager->getTextTexture(character, 24,24);
      
			text.emplace_back(&textInfo->text, advance, offsetY);
			advance += textInfo->face->glyph->advance.x >> 6; // The advance vector is expressed in 1/64th of pixels
        }
	}

	void Text::loadText(const std::wstring& textStr, const int& maxLineWidth, const int& lineSpace, const int& charWidth, const int& charHeight) {
		switch (language)
		{
		case wlEngine::Language::Chinese:
			loadChinese(textStr, maxLineWidth, lineSpace, charWidth, charHeight);
			break;
		case wlEngine::Language::Japanese:
			break;
		case wlEngine::Language::Korean:
			break;
		case wlEngine::Language::English:
			loadEnglish(textStr, maxLineWidth, lineSpace, charWidth, charHeight);
			break;
		default:
			break;
		}
	}

	void Text::loadText(const std::string& textStr, const int& maxLineWidth, const int& lineSpace, const int& charWidth, const int& charHeight) {

        std::wstring str(textStr.length(), L' '); // Make room for characters
        // Copy string to wstring.
        std::copy(textStr.begin(), textStr.end(), str.begin());
		loadText(str, maxLineWidth, lineSpace, charWidth, charHeight);
    }

    void Text::loadChinese(const std::wstring &textStr, const int &maxLineWidth, const int &lineSpace, const int &charWidth, const int &charHeight) {
        auto resourceManager = ResourceManager::get();
        int totalAdvance = 0;
        int offsetY = 0;
        for (const wchar_t &character : textStr) {
            auto textInfo = resourceManager->getTextTexture(character, charWidth, charHeight);

            if (character == L'\n') {
                offsetY -= lineSpace;
                totalAdvance = 0;
                continue;
            }
            if (totalAdvance + (textInfo->face->glyph->advance.x >> 6) > maxLineWidth) {
                offsetY -= lineSpace;
                totalAdvance = 0;
            }
            if (character == L' ' && totalAdvance == 0)
                continue;

            text.emplace_back(&textInfo->text, totalAdvance, offsetY);
            totalAdvance += textInfo->face->glyph->advance.x >> 6; // The totalAdvance vector is expressed in 1/64th of pixels
        }
    }

    void Text::loadEnglish(const std::wstring &textStr, const int &maxLineWidth, const int &lineSpace, const int &charWidth, const int &charHeight) {
        auto resourceManager = ResourceManager::get();
        int totalAdvance = 0;
        int offsetY = 0;
        for (int i = 0; i < textStr.size(); i++) {
            const wchar_t &character = textStr[i];
            if (character != L' ') {
                // if the first char is line new, we directly offset Y
                if (character == L'\n') {
                    offsetY -= lineSpace;
                    totalAdvance = 0;
                    continue;
                }

                // build this word, used to test maximum line width
                std::vector<TextInfo *> word;
                int wordWidth = 0;
                while (textStr[i] != L'\n' && i < textStr.size()) {
                    auto textInfo = resourceManager->getTextTexture(textStr[i], charWidth, charHeight);
                    word.push_back(textInfo);
                    wordWidth += textInfo->face->glyph->advance.x >> 6;
                    if (textStr[i] == L' ')
                        break;
                    i++;
                }

                // the word is exceeding maximum line width
                if (totalAdvance + wordWidth > maxLineWidth) {
                    offsetY -= lineSpace;
                    totalAdvance = 0;
                }

                // add the word to our text
                for (auto textInfo : word) {
                    if (totalAdvance + (textInfo->face->glyph->advance.x >> 6) > maxLineWidth) {
                        offsetY -= lineSpace;
                        totalAdvance = 0;
                    }
                    if (character == L' ' && totalAdvance == 0)
                        continue;

                    text.emplace_back(&textInfo->text, totalAdvance, offsetY);
                    totalAdvance += textInfo->face->glyph->advance.x >> 6; // The totalAdvance vector is expressed in 1/64th of pixels
                }

                // the next char after the word is a new line, we offset Y
                if (textStr[i] == L'\n') {
                    offsetY -= lineSpace;
                    totalAdvance = 0;
                }
            }
        }
    }

	size_t Text::renderUntilCharacter() {
		if(!animated) return text.size();

        timePassed += Time::nonScaledDeltaTime;
        int rtn = timePassed / animationSpeed;
        if (rtn > text.size())
            rtn = text.size();
        return rtn;
    }

    glm::mat4 Text::Character::getTextTransform() { return glm::translate(glm::mat4(1.0), {x, offsetY, 0}); }

    void Text::setLanguage(const Language &choice) { language = choice; }

    Text::Character::Character(Texture *t, const int &x, const int &offsetY) : texture(t), x(x), offsetY(offsetY) {}
    } // namespace wlEngine
