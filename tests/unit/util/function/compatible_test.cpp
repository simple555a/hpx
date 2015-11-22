//  Copyright (c) 2015-2016 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/include/util.hpp>
#include <hpx/util/lightweight_test.hpp>

void foo() {}

void test_construction()
{
    hpx::util::function_nonser<void()> f = foo;

    hpx::util::unique_function_nonser<void()> uf = std::move(f);
    HPX_TEST(uf.target<void(*)()>());
}

void test_assignment()
{
    hpx::util::function_nonser<void()> f = foo;

    hpx::util::unique_function_nonser<void()> uf;
    uf = std::move(f);
    HPX_TEST(uf.target<void(*)()>());
}

int main(int, char*[])
{
    test_construction();
    test_assignment();

    return hpx::util::report_errors();
}
