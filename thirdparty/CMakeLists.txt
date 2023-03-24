include(FetchContent)

message(STATUS "FetchContent: dynamic-segment-tree")

FetchContent_Declare(
    dynamic-segment-tree
    GIT_REPOSITORY https://github.com/gogagum/dynamic-segment-tree.git
    GIT_TAG 889ed54be0a092276586db43cb521ff503bd9140
)

FetchContent_MakeAvailable(dynamic-segment-tree)

function(add_boost_package PackageName package_repo_addr tag)
    message(STATUS "FetchContent: ${PackageName}, ${package_repo_addr}, ${tag}")

    FetchContent_Declare(
        ${PackageName}
        GIT_REPOSITORY https://github.com/boostorg/${package_repo_addr}.git
        GIT_TAG ${tag}
    )

    FetchContent_MakeAvailable(${PackageName})
endfunction()

SET(BOOST_FETCHCONTENT_TAG boost-1.81.0)

add_boost_package(Boost.Core core ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Config config ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Assert assert ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.ContainerHash container_hash ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.StaticAssert static_assert ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Mp11 mp11 ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.ThrowException throw_exception ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.TypeTraits type_traits ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Move move ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.WinAPI winapi ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Predef predef ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Describe describe ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Intrusive intrusive ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Container container ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Multiprecision multiprecision ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Iterator iterator ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.ConceptCheck concept_check ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Conversion conversion ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Preprocessor preprocessor ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.SmartPtr smart_ptr ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Typeof typeof ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Detail detail ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.FunctionTypes function_types ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Fusion fusion ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.MPL mpl ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Optional optional ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Utility utility ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Tuple tuple ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Functional functional ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.IO io ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Function function ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Bind bind ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Integer integer ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.TypeIndex type_index ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Range range ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Array array ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Regex regex ${BOOST_FETCHCONTENT_TAG})
