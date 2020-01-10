target_include_directories(KuangyeEngine
    PUBLIC
    Public/
    PRIVATE
    Private/
)

add_subdirectory(Public/)
add_subdirectory(Private/)