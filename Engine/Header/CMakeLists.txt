target_include_directories(wlEngine
    PUBLIC
    Public/
    PRIVATE
    Private/
)

add_subdirectory(Public/)
add_subdirectory(Private/)