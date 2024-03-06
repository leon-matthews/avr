
=====================
Building AVR Binaries
=====================

Notes on compiling completed programs and hex files for AVR targets.


Print annotated assembly listing
================================

Very useful to check that the compiler and I agree.

.. code-block:: console

    $ avr-objdump -S max7219.o > max7219.s


Reduce size of binary by omiting unused functions
=================================================

From: https://stackoverflow.com/a/11223700

    I get better results adding an additional step and building an .a archive:

    1. first, gcc and g++ are used with -ffunction-sections -fdata-sections flags
    2. then, all .o objects are put into an .a archive with ``ar rcs file.a *.o``
    3. finally, the linker is called with ``-Wl,-gc-sections,-u,main`` options
    4. for all, optimisation is set to -Os.
