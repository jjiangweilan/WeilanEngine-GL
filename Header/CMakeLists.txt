target_include_directories(WeilanEngine
    PUBLIC
    Public/
    PRIVATE
    Private/
)

add_subdirectory(Public/)
add_subdirectory(Private/)