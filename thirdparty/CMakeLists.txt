include(FetchContent)

message(STATUS "FetchContent: dynamic-segment-tree")

FetchContent_Declare(
    dynamic-segment-tree
    GIT_REPOSITORY https://github.com/gogagum/dynamic-segment-tree.git
    GIT_TAG 889ed54be0a092276586db43cb521ff503bd9140
)

FetchContent_MakeAvailable(dynamic-segment-tree)

