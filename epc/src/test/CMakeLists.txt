find_package(Check REQUIRED)
find_package(Threads REQUIRED)

include_directories(${CHECK_INCLUDE_DIRS})

set(MME_APP_UE_CONTEXT_IMSI_SRC
  test_mme_app_ue_context.c
)

add_executable(test_mme_app_ue_context_imsi ${MME_APP_UE_CONTEXT_IMSI_SRC})
target_link_libraries(test_mme_app_ue_context_imsi MME_APP ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})