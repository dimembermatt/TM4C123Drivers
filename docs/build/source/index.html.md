---
title: API Reference

toc_footers:
  - <a href='https://github.com/dimembermatt/TM4C123Drivers'>Base TM4C123Drivers
    Repo</a> 
  - <a href='https://github.com/slatedocs/slate'>Documentation Powered by
    Slate</a> 

includes:
  - setup
  - version_control
  - project0
  - project1
  - debugging
  - tuning
  - GPIO
  - ADC

search: true

code_clipboard: true

meta:
  - name: description
    content: RASware Documentation
---

# Introduction

```rust
 __   __    _______    ___        ___        _______  
|  | |  |  |       |  |   |      |   |      |       | 
|  |_|  |  |    ___|  |   |      |   |      |   _   | 
|       |  |   |___   |   |      |   |      |  | |  | 
|       |  |    ___|  |   |___   |   |___   |  |_|  | 
|   _   |  |   |___   |       |  |       |  |       | 
|__| |__|  |_______|  |_______|  |_______|  |_______| 
 _     _    _______    ______      ___        ______  
| | _ | |  |       |  |    _ |    |   |      |      | 
| || || |  |   _   |  |   | ||    |   |      |  _    |
|       |  |  | |  |  |   |_||_   |   |      | | |   |
|       |  |  |_|  |  |    __  |  |   |___   | |_|   |
|   _   |  |       |  |   |  | |  |       |  |       |
|__| |__|  |_______|  |___|  |_|  |_______|  |______| 

               ________
          _,.-Y  |  |  Y-._    -Row  (the Ascii-Wizard of Oz)
      .-~"   ||  |  |  |   "-.
      I" ""=="|" !""! "|"[]""|     _____
      L__  [] |..------|:   _[----I" .-{"-.
     I___|  ..| l______|l_ [__L]_[I_/r(=}=-P -------------------
    [L______L_[________]______j~  '-=c_]/=-^
     \_I_j.--.\==I|I==_/.--L_]
       [_((==)[`-----"](==)j
          I--I"~~"""~~"I--I     .---.     
          |[]|         |[]|    } 0 0 {    
          l__j         l__j    } 0 0 {    
          |!!|         |!!|     \_@_/     
          |..|         |..|    ,=|x|=.    
          ([])         ([])    'c/_\  'c  
          ]--[         ]--[     (   )     
          [_L]         [_L]    __) (__    
         /|..|\       /|..|\
        `=}--{='     `=}--{='
       .-^--r-^-.   .-^--r-^-.

     .---.          .===./`   
    } n n {        /.d b.\    
     \_-_/         "\_v_/"    
.'c ."|_|". n`.   .=(+++)=.   
'--'  /_\  `--'o="  (___)  "=o
     /| |\         . \_/  .   
    [_] [_]        . .:::.. .   
```

Welcome to the Robotathon UTRASware documentation!

Here, you'll find various programming and software tutorials, spanning from how
to use the integrated development environment (IDE) software to writing drivers
for the peripherals of the TM4C and for various devices that are used as sensors
and actuators for your robot.

The following pages on this site are recommended reads, preferably in order. If
you already know how to do the topic, it can be skipped.

1. [Setup](#setup). This page provides the list of prerequisites for Robotathon,
   and how to install them and set them up. Some overlap with the next tutorial,
   [Version Control](#version-control).
2. [Version Control](#version-control). This tutorial guides you through cloning
   the UTRASWare software library and how to maintain changes to the library for
   your own purposes.
3. [Running a Demo Project](#running-a-demo-project). This tutorial guides you
   into running the RGB LED example in UTRASWare. Prereqs include actually
   having CCS and UTRASWare installed. 
4. [Creating a New Project](#creating-your-first-project). This tutorial guides
   you on creating a new project from scratch. In particular, we'll manipulate
   some servos as a first step towards making a moving robot base. It references
   `rascar` from [Version Control](#version-control).
5. [Debugging](#debugging). TBD
6. [Tuning](#tuning). TBD

Past these pages are a list of all the drivers, both the low level drivers in
TM4C123Drivers and the high level drivers in UTRASWare. Each page will describe
their functionality, API, and usage.

# Contributors

- Matthew Yu (matthewjkyu@gmail.com)
- Dario Jimenez

# Copyright

Copyright 2021 Matthew Yu

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions: 

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
