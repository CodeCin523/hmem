add_executable(
    test_hmem_arena
    test_hmem_arena.c
)
target_link_libraries(
    test_hmem_arena PRIVATE
    hmem::hmem
)

add_executable(
    test_hmem_book
    test_hmem_book.c
)
target_link_libraries(
    test_hmem_book PRIVATE
    hmem::hmem
)

add_executable(
    test_hmem_os
    test_hmem_os.c
)
target_link_libraries(
    test_hmem_os PRIVATE
    hmem::hmem
)