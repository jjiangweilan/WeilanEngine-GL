cmake_minimum_required(VERSION 3.15.0)

set(SOURCES
    "Animation.cpp"
    "Audio.cpp"
    "Camera.cpp"
    "Camera2D.cpp"
    "Camera3D.cpp"
    "Component.cpp"
    "LuaScript.cpp"
    "Model.cpp"
    "NpcController.cpp"
    "RenderContext.cpp"
    "RenderScript.cpp"
    "Script.cpp"
    "Sprite.cpp"
    "StateMachine.cpp"
    "Text.cpp"
    "Transform.cpp"
    "TRigidbody.cpp"
    "VolumetricLight.cpp"
)

target_sources(wlEngine
    PRIVATE 
    ${SOURCES}
)