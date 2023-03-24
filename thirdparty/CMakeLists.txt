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

message(STATUS "FetchContent: Boost.ConceptCheck")

FetchContent_Declare(
    Boost.ConceptCheck
    GIT_REPOSITORY https://github.com/boostorg/concept_check.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.ConceptCheck)

message(STATUS "FetchContent: Boost.Conversion")

FetchContent_Declare(
    Boost.Conversion
    GIT_REPOSITORY https://github.com/boostorg/conversion.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Conversion)

message(STATUS "FetchContent: Boost.Preprocessor")

FetchContent_Declare(
    Boost.Preprocessor
    GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Preprocessor)

message(STATUS "FetchContent: Boost.SmartPtr")

FetchContent_Declare(
    Boost.SmartPtr
    GIT_REPOSITORY https://github.com/boostorg/smart_ptr.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.SmartPtr)

message(STATUS "FetchContent: Boost.Typeof")

FetchContent_Declare(
    Boost.Typeof
    GIT_REPOSITORY https://github.com/boostorg/typeof.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Typeof)

message(STATUS "FetchContent: Boost.Detail")

FetchContent_Declare(
    Boost.Detail
    GIT_REPOSITORY https://github.com/boostorg/detail.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Detail)

message(STATUS "FetchContent: Boost.FunctionTypes")

FetchContent_Declare(
    Boost.FunctionTypes
    GIT_REPOSITORY https://github.com/boostorg/function_types.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.FunctionTypes)

message(STATUS "FetchContent: Boost.Fusion")

FetchContent_Declare(
    Boost.Fusion
    GIT_REPOSITORY https://github.com/boostorg/fusion.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Fusion)

message(STATUS "FetchContent: Boost.MPL")

FetchContent_Declare(
    Boost.MPL
    GIT_REPOSITORY https://github.com/boostorg/mpl.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.MPL)

message(STATUS "FetchContent: Boost.Optional")

FetchContent_Declare(
    Boost.Optional
    GIT_REPOSITORY https://github.com/boostorg/optional.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Optional)

message(STATUS "FetchContent: Boost.Utility")

FetchContent_Declare(
    Boost.Utility
    GIT_REPOSITORY https://github.com/boostorg/utility.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Utility)

message(STATUS "FetchContent: Boost.Tuple")

FetchContent_Declare(
    Boost.Tuple
    GIT_REPOSITORY https://github.com/boostorg/tuple.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Tuple)

message(STATUS "FetchContent: Boost.Functional")

FetchContent_Declare(
    Boost.Functional
    GIT_REPOSITORY https://github.com/boostorg/functional.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Functional)

message(STATUS "FetchContent: Boost.IO")

FetchContent_Declare(
    Boost.IO
    GIT_REPOSITORY https://github.com/boostorg/io.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.IO)

message(STATUS "FetchContent: Boost.Function")

FetchContent_Declare(
    Boost.Function
    GIT_REPOSITORY https://github.com/boostorg/function.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Function)

message(STATUS "FetchContent: Boost.Bind")

FetchContent_Declare(
    Boost.Bind
    GIT_REPOSITORY https://github.com/boostorg/bind.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Bind)

message(STATUS "FetchContent: Boost.Integer")

FetchContent_Declare(
    Boost.Integer
    GIT_REPOSITORY https://github.com/boostorg/integer.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Integer)

message(STATUS "FetchContent: Boost.TypeIndex")

FetchContent_Declare(
    Boost.TypeIndex
    GIT_REPOSITORY https://github.com/boostorg/type_index.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.TypeIndex)

message(STATUS "FetchContent: Boost.Range")

FetchContent_Declare(
    Boost.Range
    GIT_REPOSITORY https://github.com/boostorg/range.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Range)

message(STATUS "FetchContent: Boost.Array")

FetchContent_Declare(
    Boost.Array
    GIT_REPOSITORY https://github.com/boostorg/array.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Array)

message(STATUS "FetchContent: Boost.Regex")

FetchContent_Declare(
    Boost.Regex
    GIT_REPOSITORY https://github.com/boostorg/regex.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost.Regex)
