add_executable(
    unit 
    unit.c
    test.h)

add_executable(
    integration
    integration.c
    test.h)
    

target_link_libraries(unit PRIVATE ${SIGFN_INCLUDE} pthreadpool)
target_link_libraries(integration PRIVATE ${SIGFN_INCLUDE} pthreadpool)

add_test(NAME test_create COMMAND unit create)
add_test(NAME test_destroy COMMAND unit destroy)
add_test(NAME test_submit COMMAND unit submit)
add_test(NAME test_shutdown COMMAND unit shutdown)

add_test(NAME test_pthreadpool COMMAND integration)
