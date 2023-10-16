# синтаксис меняется от версии к версии, поэтому эта строчка обязательна
cmake_minimum_required(VERSION 3.15)

# название проекта, чтоб к нему подтягивать source-файлы
project(fuse-example)

# дополнительные флаги компиляции
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -g")

# автоматически находим fuse3
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)

# дополнительные папки для #include <abc.h> (где искать abc.h)
include_directories(${FUSE_INCLUDE_DIRS})
# говорим, что в этом файле реализована ф-ия main
add_executable(my_fake_fs_with_rabbit main.c)
# и линкуем библиотеки fuse
target_link_libraries(my_fake_fs_with_rabbit ${FUSE_LIBRARIES})
