
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ClockDSP.hpp"
#include "WavWriter.hpp"

#include "lufu\oscillator.hpp"
#include "lufu\continuous_iterator.hpp"
#include <iostream>


const auto M = 0.000001f;
const auto close_to_zero = Approx(0.0f).margin(M);
const auto close_to_one = Approx(1.0f).margin(M);

TEST_CASE("sin wave oscillator")
{
  
    // 10hz
    lufu::Oscillator s(1.0, 1.0);
    CHECK(s.sine() == 0);
    CHECK(s.sine() == Approx(0));
    s.tick(1.0); 
    CHECK(s.sine() == close_to_zero);

    s.tick(0.5); // half cycle.
    CHECK(s.sine() == close_to_zero);

    s.tick(0.5); // half cycle.
    CHECK(s.sine() == close_to_zero);

    s.tick(0.25);
    CHECK(s.sine() == close_to_one);
}

TEST_CASE("sin wave oscillator (10hz)")
{
    // 10hz
    lufu::Oscillator s(10.0, 1.0);
    CHECK(s.sine() == 0);
    CHECK(s.sine() == Approx(0));
    s.tick(1.0);
    CHECK(s.sine() == close_to_zero);

    constexpr auto half_cycle = (1.0f / 10.0f) / 2.0f;

    s.tick(half_cycle); // half cycle.
    CHECK(s.sine() == close_to_zero);

    s.tick(half_cycle); // half cycle.

    CHECK(s.sine() == close_to_zero);

    s.tick(half_cycle / 2.0f);
    CHECK(s.sine() == close_to_one);

}

    
TEST_CASE("continuous_iterator basics")
{

    std::vector<int> v{ 1,2,3,4,5,6,7,8,9,10 };
    lufu::continuous_iterator<std::vector<int>> it(v);

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
