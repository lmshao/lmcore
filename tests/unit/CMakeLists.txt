# Unit tests for individual components

# DataBuffer unit tests
add_executable(test_data_buffer test_data_buffer.cpp)
target_link_libraries(test_data_buffer lmcore)
add_test(NAME DataBufferTests COMMAND $<TARGET_FILE:test_data_buffer>)
set_tests_properties(DataBufferTests PROPERTIES LABELS "unit")

# TaskQueue unit tests
add_executable(test_task_queue test_task_queue.cpp)
target_link_libraries(test_task_queue lmcore)
add_test(NAME TaskQueueTests COMMAND $<TARGET_FILE:test_task_queue>)
set_tests_properties(TaskQueueTests PROPERTIES LABELS "unit")

# ThreadPool unit tests
add_executable(test_thread_pool test_thread_pool.cpp)
target_link_libraries(test_thread_pool lmcore ${PLATFORM_LIBS})
add_test(NAME ThreadPoolTests COMMAND $<TARGET_FILE:test_thread_pool>)
set_tests_properties(ThreadPoolTests PROPERTIES LABELS "unit")

# AsyncTimer unit tests
add_executable(test_async_timer test_async_timer.cpp)
target_link_libraries(test_async_timer lmcore ${PLATFORM_LIBS})
add_test(NAME AsyncTimerTests COMMAND $<TARGET_FILE:test_async_timer>)
set_tests_properties(AsyncTimerTests PROPERTIES LABELS "unit")

# ObjectPool unit tests
add_executable(test_object_pool test_object_pool.cpp)
target_link_libraries(test_object_pool lmcore ${PLATFORM_LIBS})
add_test(NAME ObjectPoolTests COMMAND $<TARGET_FILE:test_object_pool>)
set_tests_properties(ObjectPoolTests PROPERTIES LABELS "unit")

# ByteOrder unit tests
add_executable(test_byte_order test_byte_order.cpp)
target_link_libraries(test_byte_order lmcore)
add_test(NAME ByteOrderTests COMMAND $<TARGET_FILE:test_byte_order>)
set_tests_properties(ByteOrderTests PROPERTIES LABELS "unit")

# MappedFile unit tests
add_executable(test_mapped_file test_mapped_file.cpp)
target_link_libraries(test_mapped_file lmcore)
add_test(NAME MappedFileTests COMMAND $<TARGET_FILE:test_mapped_file>)
set_tests_properties(MappedFileTests PROPERTIES LABELS "unit")

# Base64 unit tests
add_executable(test_base64 test_base64.cpp)
target_link_libraries(test_base64 lmcore)
add_test(NAME Base64Tests COMMAND $<TARGET_FILE:test_base64>)
set_tests_properties(Base64Tests PROPERTIES LABELS "unit")

# CRC32 unit tests
add_executable(test_crc32 test_crc32.cpp)
target_link_libraries(test_crc32 lmcore)
add_test(NAME CRC32Tests COMMAND $<TARGET_FILE:test_crc32>)
set_tests_properties(CRC32Tests PROPERTIES LABELS "unit")

# MD5 unit tests
add_executable(test_md5 test_md5.cpp)
target_link_libraries(test_md5 lmcore)
add_test(NAME MD5Tests COMMAND $<TARGET_FILE:test_md5>)
set_tests_properties(MD5Tests PROPERTIES LABELS "unit")

# URL unit tests
add_executable(test_url test_url.cpp)
target_link_libraries(test_url lmcore)
add_test(NAME URLTests COMMAND $<TARGET_FILE:test_url>)
set_tests_properties(URLTests PROPERTIES LABELS "unit")

# Hex unit tests
add_executable(test_hex test_hex.cpp)
target_link_libraries(test_hex lmcore)
add_test(NAME HexTests COMMAND $<TARGET_FILE:test_hex>)
set_tests_properties(HexTests PROPERTIES LABELS "unit")

# StringUtils unit tests
add_executable(test_string_utils test_string_utils.cpp)
target_link_libraries(test_string_utils lmcore)
add_test(NAME StringUtilsTests COMMAND $<TARGET_FILE:test_string_utils>)
set_tests_properties(StringUtilsTests PROPERTIES LABELS "unit")

# TimeUtils unit tests
add_executable(test_time_utils test_time_utils.cpp)
target_link_libraries(test_time_utils lmcore)
add_test(NAME TimeUtilsTests COMMAND $<TARGET_FILE:test_time_utils>)
set_tests_properties(TimeUtilsTests PROPERTIES LABELS "unit")

# UUID unit tests
add_executable(test_uuid test_uuid.cpp)
target_link_libraries(test_uuid lmcore)
add_test(NAME UUIDTests COMMAND $<TARGET_FILE:test_uuid>)
set_tests_properties(UUIDTests PROPERTIES LABELS "unit")