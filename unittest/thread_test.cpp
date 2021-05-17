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

void fun(int a) {
    std::cout << a << std::endl;
    sleep(1);
    std::cout << "asdf " << a << std::endl;
}

int main() {
    threadpool pool(5);
    int a = 5;
    auto func = [a]{fun(a);};
    auto func2 = std::bind(fun, 5);

    std::function<void(int)> func3 = std::bind(fun, 5);
    
    pool.commit(func2);
    pool.commit(func2);
    pool.commit(func2);
    pool.commit(func2);
    pool.commit(func2);
    
    pool.close();
    pool.join();
    return 0;
}