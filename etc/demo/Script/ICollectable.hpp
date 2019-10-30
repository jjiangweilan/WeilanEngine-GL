#pragma once
class ICollectable {

    /**
     * @brief called when other game objects try to collect this item
     * 
     * @return true if this item is collectable, false otherwise
     */
    virtual bool isCollected() = 0;
    /**
     * @brief get the fixture that is used to define the collect range
     */
    virtual void getCollectFixture() = 0;
};
