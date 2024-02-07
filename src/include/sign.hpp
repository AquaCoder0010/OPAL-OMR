#pragma once

static int sign(int number)
{
    if(number < 0)
        return -1;
    if(number > 0)
        return 1;
    return 0;
}