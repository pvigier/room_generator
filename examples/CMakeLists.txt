add_executable(example main.cpp)
target_link_libraries(example PRIVATE room_generator)
setWarnings(example)
setStandard(example)