
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ClockDSP.hpp"

#include "Utils.hpp"

TEST_CASE("cyclic_iterator basics")
{

    std::vector<int> v{ 1,2,3,4,5,6,7,8,9,10 };
    cyclic_iterator<std::vector<int>> it(v);

    CHECK(*it == 1);

    it += 0.2;
    CHECK(*it == 1);

    it += 0.6;
    CHECK(*it == 1);

    it += 0.2;
    CHECK(*it == 2);

    it += 0.2;
    CHECK(*it == 2);

    it += -1.2;
    CHECK(*it == 1);

    it += -0.00001;
    CHECK(*it == 10);
}