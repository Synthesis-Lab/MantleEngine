# CMake generated Testfile for 
# Source directory: /home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests
# Build directory: /home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cmake-build-debug/cpp/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[VulkanRendererTests]=] "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cmake-build-debug/cpp/tests/test_vulkan_renderer")
set_tests_properties([=[VulkanRendererTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests/CMakeLists.txt;27;add_test;/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests/CMakeLists.txt;0;")
add_test([=[RuntimeTests]=] "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cmake-build-debug/cpp/tests/test_runtime")
set_tests_properties([=[RuntimeTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests/CMakeLists.txt;62;add_test;/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests/CMakeLists.txt;0;")
add_test([=[AllEnginesIntegrationTests]=] "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cmake-build-debug/cpp/tests/test_all_engines_integration")
set_tests_properties([=[AllEnginesIntegrationTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests/CMakeLists.txt;76;add_test;/home/berkay/Belgeler/Geliştirme/CLionProjects/MantleEngine/cpp/tests/CMakeLists.txt;0;")
subdirs("../../_deps/googletest-build")
