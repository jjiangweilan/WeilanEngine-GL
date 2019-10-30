

transform = entity:getComponent("Transform")
stateMachine = entity:getComponent("StateMachine")
animation = entity:getComponent("Animation")
animation:addAnimationFromAseprite("../resource/enemy/enemy1.json", "../resource/enemy/enemy1.png")
animation:playAnimation("idle")

function idleUpdate()
    value = Input.getControllerButton(Input.ButtonLeft)
end

stateMachine:setAction("idle", "update", idleUpdate)
function update()
end