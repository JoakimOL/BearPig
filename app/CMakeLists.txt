add_executable(bearpig main.cpp)
target_compile_features(bearpig PRIVATE cxx_std_20)

target_link_libraries(bearpig PRIVATE libbearpig spdlog::spdlog argparse)
