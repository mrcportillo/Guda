#include <hayai/hayai.hpp>
#include <totest.cpp>

BENCHMARK(totest, sumar, 20, 200) {
    totest().sumar(2,1);
}
