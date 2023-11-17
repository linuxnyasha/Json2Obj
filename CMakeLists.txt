cmake_minimum_required(VERSION 3.12)
find_package(Boost 1.83.0 CONFIG REQUIRED)
project(
  Json2Obj
  VERSION 1.0.1
    DESCRIPTION
    "A header-only c++ library that provides json serialization on reflection"
    HOMEPAGE_URL  "http://ivt5wiimhwpo56td6eodn2n3fduug3bglqvqewbk2jnyl4hcimea.b32.i2p/git/sha512sum/Json2Obj")

include(GNUInstallDirs)

add_library(Json2Obj INTERFACE)

# Adding the install interface generator expression makes sure that the include
# files are installed to the proper location (provided by GNUInstallDirs)
target_include_directories(
  Json2Obj
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(Json2Obj INTERFACE cxx_std_20)

#enable_testing()
if(BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

install(TARGETS Json2Obj
  EXPORT Json2ObjTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("Json2ObjConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/Json2ObjConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/Json2ObjConfig.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/Json2Obj/cmake)

install(EXPORT Json2ObjTargets
        FILE Json2ObjTargets.cmake
        NAMESPACE Json2Obj::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/Json2Obj/cmake)

install(FILES "${PROJECT_BINARY_DIR}/Json2ObjConfig.cmake"
              "${PROJECT_BINARY_DIR}/Json2ObjConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/Json2Obj/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/json2obj DESTINATION include)


