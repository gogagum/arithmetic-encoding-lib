include(FetchContent)

message(STATUS "FetchContent: dynamic-segment-tree")

FetchContent_Declare(
    dynamic-segment-tree
    GIT_REPOSITORY https://github.com/gogagum/dynamic-segment-tree.git
    GIT_TAG 0.2
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

SET(BOOST_FETCHCONTENT_TAG boost-1.84.0)

add_boost_package(Boost.Core core ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Config config ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Assert assert ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.ContainerHash container_hash ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.StaticAssert static_assert ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Mp11 mp11 ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.TypeTraits type_traits ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Move move ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Describe describe ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Intrusive intrusive ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Container container ${BOOST_FETCHCONTENT_TAG})
add_boost_package(Boost.Multiprecision multiprecision ${BOOST_FETCHCONTENT_TAG})
