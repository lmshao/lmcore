# Examples for lmcore library

# AsyncTimer example
add_executable(async_timer_example async_timer_example.cpp)
target_link_libraries(async_timer_example lmcore ${PLATFORM_LIBS})

# ObjectPool example
add_executable(object_pool_example object_pool_example.cpp)
target_link_libraries(object_pool_example lmcore ${PLATFORM_LIBS})

# SpscChannel example
add_executable(spsc_channel_example spsc_channel_example.cpp)
target_link_libraries(spsc_channel_example lmcore ${PLATFORM_LIBS})

# Sync Channels comprehensive example
add_executable(sync_channels_example sync_channels_example.cpp)
target_link_libraries(sync_channels_example lmcore ${PLATFORM_LIBS})

# Set output directory for examples
set_target_properties(async_timer_example object_pool_example spsc_channel_example sync_channels_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)