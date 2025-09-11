#include "sparq.hpp"

int main()
{
    SPARQ sparq;

    int init_result = sparq.init();

    if (init_result != 0)
    {
        return init_result;
    }

    return sparq.run();
}