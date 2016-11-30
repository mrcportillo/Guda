#include <hayai/hayai.hpp>
#include <proceso.cpp>

BENCHMARK(proceso, principal, 2, 2) {
    proceso().principal();
}
