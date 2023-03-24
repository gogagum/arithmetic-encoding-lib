include(FetchContent)

message(STATUS "FetchContent: dynamic-segment-tree")

FetchContent_Declare(
    dynamic-segment-tree
    GIT_REPOSITORY https://github.com/gogagum/dynamic-segment-tree.git
    GIT_TAG 889ed54be0a092276586db43cb521ff503bd9140
)

FetchContent_MakeAvailable(dynamic-segment-tree)

message(STATUS "FetchContent: Boost.Core")

FetchContent_Declare(
    Boost.Core
    GIT_REPOSITORY https://github.com/boostorg/core.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Core)

message(STATUS "FetchContent: Boost.Config")

FetchContent_Declare(
    Boost.Config
    GIT_REPOSITORY https://github.com/boostorg/config.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Config)

message(STATUS "FetchContent: Boost.Assert")

FetchContent_Declare(
    Boost.Assert
    GIT_REPOSITORY https://github.com/boostorg/assert.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Assert)

message(STATUS "FetchContent: Boost.ContainerHash")

FetchContent_Declare(
    Boost.ContainerHash
    GIT_REPOSITORY https://github.com/boostorg/container_hash.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.ContainerHash)

message(STATUS "FetchContent: Boost.StaticAssert")

FetchContent_Declare(
    Boost.StaticAssert
    GIT_REPOSITORY https://github.com/boostorg/static_assert.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.StaticAssert)

message(STATUS "FetchContent: Boost.MP11")

FetchContent_Declare(
    Boost.MP11
    GIT_REPOSITORY https://github.com/boostorg/mp11.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.MP11)

message(STATUS "FetchContent: Boost.ThrowException")

FetchContent_Declare(
    Boost.ThrowException
    GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.ThrowException)

message(STATUS "FetchContent: Boost.TypeTraits")

FetchContent_Declare(
    Boost.TypeTraits
    GIT_REPOSITORY https://github.com/boostorg/type_traits.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.TypeTraits)

message(STATUS "FetchContent: Boost.Move")

FetchContent_Declare(
    Boost.Move
    GIT_REPOSITORY https://github.com/boostorg/move.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Move)

message(STATUS "FetchContent: Boost.WinAPI")

FetchContent_Declare(
    Boost.WinAPI
    GIT_REPOSITORY https://github.com/boostorg/winapi.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.WinAPI)

message(STATUS "FetchContent: Boost.Predef")

FetchContent_Declare(
    Boost.Predef
    GIT_REPOSITORY https://github.com/boostorg/predef.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Predef)

message(STATUS "FetchContent: Boost.Describe")

FetchContent_Declare(
    Boost.Describe
    GIT_REPOSITORY https://github.com/boostorg/describe.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Boost.Describe)

message(STATUS "FetchContent: Boost.Intrusive")

FetchContent_Declare(
    Boost.Intrusive
    GIT_REPOSITORY https://github.com/boostorg/intrusive.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Intrusive)

message(STATUS "FetchContent: Boost.Container")

FetchContent_Declare(
    Boost.Container
    GIT_REPOSITORY https://github.com/boostorg/container.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Container)

message(STATUS "FetchContent: Boost.Multiprecision")

FetchContent_Declare(
    Boost.Multiprecision
    GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Multiprecision)

message(STATUS "FetchContent: Boost.Iterator")

FetchContent_Declare(
    Boost.Iterator
    GIT_REPOSITORY https://github.com/boostorg/iterator.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Iterator)

