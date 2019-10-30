#include <wlEngine/wlEngine.hpp>

using namespace wlEngine;

class Building : public Script{
    SCRIPT_DECLARATION(Script, Building, 32);
public:
    Building(Entity* entity);
	void update() override {};
	void postInit() override;
private:
    Entity* entrance;
    Entity* front;
    Entity* back;
	Entity* inside;
    Entity* outline;

    void entranceContactBegin(TRigidbody* other);
	void entranceContactEnd(TRigidbody* other);
};
