cmake_minimum_required(VERSION 3.15.0)

set(SOURCES
    "Animation.cpp"
    "Audio.cpp"
    "Camera.cpp"
    "Component.cpp"
    "LuaScript.cpp"
    "Model.cpp"
    "NpcController.cpp"
    "RenderScript.cpp"
    "Script.cpp"
    "Sprite.cpp"
    "StateMachine.cpp"
    "Text.cpp"
    "Transform.cpp"
    "TRigidbody.cpp"
    "VolumetricLight.cpp"
    "CameraController.cpp"
)

target_sources(WeilanEngine
    PRIVATE 
    ${SOURCES}
)
