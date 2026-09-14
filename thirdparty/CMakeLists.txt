include(FetchContent)

message(STATUS "FetchContent: dynamic-segment-tree")

FetchContent_Declare(
    dynamic-segment-tree
    GIT_REPOSITORY https://github.com/gogagum/dynamic-segment-tree.git
    GIT_TAG 0.2
)

FetchContent_MakeAvailable(dynamic-segment-tree)
