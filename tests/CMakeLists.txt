# fetch googletest
fetchcontent_declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest    
    GIT_TAG b514bdc
)

fetchcontent_makeavailable(googletest)

# Tests need to be added as executables first
add_executable(bearpigtests tests.cpp)

target_compile_features(bearpigtests PRIVATE cxx_std_20)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(bearpigtests PRIVATE
    libbearpig
    GTest::gtest_main
)
# include(GoogleTest)
# gtest_discover_tests(tests)

add_test(NAME bearpigtests COMMAND bearpigtests) 

