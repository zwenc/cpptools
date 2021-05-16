/*
 * @copyright: zwenc
 * @email: zwence@163.com
 * @Date: 2021-05-15 17:44:56
 * @FilePath: \cpptools\unittest\thread_test.cpp
 */
#include <tools/threadpool.h>
#include <unistd.h>
#include <iostream>

std::mutex m_lock;

void fun() {
    std::cout << "asdf" << std::endl;
}

int main() {
    threadpool pool(5);
    
    pool.commit(fun);
    pool.commit(fun);
    pool.commit(fun);
    pool.commit(fun);

    return 0;
}