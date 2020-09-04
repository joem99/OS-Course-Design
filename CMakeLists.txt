cmake_minimum_required(VERSION 3.10)

project(OS-Course-Design)

# Add myw lib
include_directories("${PROJECT_SOURCE_DIR}/mywlib")
add_subdirectory(mywlib)


# Add GTK2
find_package(PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
set(CMAKE_C_STANDARD 11)
include_directories (${GTK2_INCLUDE_DIRS})
link_directories (${GTK2_LIBRARY_DIRS})
add_definitions (${GTK2_CFLAGS_OTHER})


# Build Exp 1
add_executable(copy ./EXP1-LINUX_UI/copy.c)
target_link_libraries(copy mywlib)
add_executable(procUI ./EXP1-LINUX_UI/procUI.c)
target_link_libraries(procUI mywlib ${GTK2_LIBRARIES})

# Build Exp 2
add_executable(testhello ./EXP2-SYSCALL/testhello.c)
add_executable(testcopy ./EXP2-SYSCALL/testcopy.c)

# Build Exp3
add_executable(testdev ./EXP3-Device_Driver/testDevDriver.c)

# Build Exp 4
aux_source_directory(./EXP4-System_monitor sysmonitor)
add_executable(myw_sysmonitor ${sysmonitor})
target_link_libraries(myw_sysmonitor mywlib ${GTK2_LIBRARIES})

# Build Exp 5
aux_source_directory(./EXP5-File_System filesystem)
add_executable(mywfs ${filesystem})
target_link_libraries(mywfs mywlib)