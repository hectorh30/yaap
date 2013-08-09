# yaap: Yet-another assembler paint

Academic project for the Compuer Graphics subject of [UVG](http://www.uvg.edu.gt) 2013.
The main goal is to implement a Paint program optimizing rendering work with native ASM
code.

## Set up

1. DOSBox

The project is designed to run over [DOSBox](http://www.dosbox.com/). Refer to it for installation.

2. DJPP

The project is to be compiled with [DJCPP](http://www.delorie.com/djgpp/). Please
add the path to DJCPP to the [dosbox.conf](http://www.dosbox.com/wiki/Dosbox.conf). Something like:

    set path=Z:;C:\DJGPP\bin
    set DJGPP=C:\DJGPP\DJGPP.ENV

3. 

## Compile
    cd SRC
    `gpp -o main.exe main.cpp`

## Run
    main.exe


## Usage
    Refer to the [usage](doc/usage.md) doc file.
