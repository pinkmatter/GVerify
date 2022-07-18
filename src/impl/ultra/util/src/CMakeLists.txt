file(GLOB SOURCES "*.cpp")
file(GLOB SOURCES_concurrency "concurrency/*.cpp")
file(GLOB SOURCES_concurrency_lock "concurrency/lock/*.cpp")
file(GLOB SOURCES_concurrency_lock_pthreads "concurrency/lock/pthreads/*.cpp")
file(GLOB SOURCES_concurrency_thread "concurrency/thread/*.cpp")
file(GLOB SOURCES_concurrency_thread_pthreads "concurrency/thread/pthreads/*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-util ${SOURCES} ${SOURCES_concurrency} ${SOURCES_concurrency_lock} ${SOURCES_concurrency_lock_pthreads} ${SOURCES_concurrency_thread} ${SOURCES_concurrency_thread_pthreads})
