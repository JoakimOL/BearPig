# fetch googletest
fetchcontent_declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest    
    GIT_TAG b514bdc
)

fetchcontent_makeavailable(googletest)

add_executable(bearpigtests
    parsertests.cpp
    e2etest.cpp
)

target_compile_features(bearpigtests PRIVATE cxx_std_20)

target_link_libraries(bearpigtests PRIVATE
    libbearpig
    GTest::gtest_main
)

add_test(NAME bearpigtests COMMAND bearpigtests) 
