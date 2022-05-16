# mSWAB

This repository contains the code of the mSWAB algorithm.

For the respective academic publication, please see:

"Enabling Efficient Time Series Analysis for Wearable Activity Data" <br>
Van Laerhoven, Kristof and Berlin, Eugen and Schiele, Bernt. <br>
In Proceedings of the 8th International Conference on Machine Learning and Applications (ICMLA 2009), IEEE Press, 2009.

https://www.eti.uni-siegen.de/ubicomp/papers/ess_icmla2009.pdf





## BUILD:

 To build the source, change to the directory with the Makefile and run:

    $ make

 This will create some additional folders, with ./bin/ containing the
 main_segs executable.


## SEGMENTATION:

 These commands approximate the raw data in 'example.bin':

    $ ./bin/main_segs mSWABsegs 30 100 ./data/example.bin ./data/segments.bin
    $ ./bin/main_segs  SWABsegs 15  80 ./data/example.bin ./data/segments.bin

 If you want to visualize the results, please see the demo.py or demo.ipynb


## DATA STRUCTURE:


 The data examples provided in the data folder have the following structure:
 The first byte gives the number of signal axes in the data. The sensor values
 are uint8 values (reaching from 0 to 255). As we are mostly working with
 3-D accelerometers, we have 3 signals stored as follows:

    <dim> <x0> <y0> <z0> <x1> <y1> <z1> <x2> <y2> <z3> ...

 The produced segments file has the following structure: a header holding
 the number of dimensions (1 byte) and the resulting array sizes per dimension
 (uint32 values), followed by index/value pairs:

    <dim> <size_x> <size_y> <size_z>
    <x_i0> <x_v0> <x_i1> <x_v1> <x_i2> <x_v2> ...
    <y_i0> <y_v0> <y_i1> <y_v1> <y_i2> <y_v2> ...
    <z_i0> <z_v0> <z_i1> <z_v1> <z_i2> <z_v2> ...

    <uint8> <uint32> <uint32> <uint32> <uint8> <uint8> <uint8> ....
