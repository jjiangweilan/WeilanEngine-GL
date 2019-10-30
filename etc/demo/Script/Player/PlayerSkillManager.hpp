#pragma once
#include <wlEngine/wlEngine.hpp>

using namespace wlEngine;

namespace PlayerSkill {
    struct SkillData {
        Entity* target;
        glm::vec2 targetLocation;
        int powerLevel;
    };

    struct Skill {
        virtual void useSkill() = 0;
        virtual Entity* castSpell(Entity* player) = 0;
		virtual void updateSkill() {};
        int useDifficulty;
        std::string spell;
    };

    struct FireBall : public Skill{
        FireBall();
        virtual void useSkill() override;
        virtual Entity* castSpell(Entity* player) override;
        private:
    };
}

class PlayerSkillManager {
  public:
    PlayerSkill::FireBall fireBall;
    PlayerSkillManager(Entity *player);
    void startSpellSkill();
    void updateSpellSkill();
    void endSpellSkill();
    Entity *player;
    Entity *buttonEffect;

    bool spellFailed;
    bool spellSucceed;
private:
    int currentSpellIndex;
    PlayerSkill::Skill* currentSpellSkill;

};