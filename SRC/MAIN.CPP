#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <sys/types.h>
#include <sys/movedata.h>
#include <go32.h>

#include "manager.h"

int main (int argc, char *argv[])
{
    Manager manager;
    manager.launch();

    return 0;
}

#endif
