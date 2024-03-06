
===========================
Programming AVR and Arduino
===========================

Notes on flashing AVR chips, both bare chips and contained within an Arduino board.

Check Communication
===================

Run ``avrdude`` without a ``-U`` argument to test communication.


Serial over USB
---------------

Direct USB connection for devices **Arduino UNO** or **Nano**, where the serial port is
something like ``tty.usbmodem411`` in Mac OS X and ``ttyACM0`` under Ubuntu - but only
if the chip has a boot loader installed:

.. code-block:: console

    $ avrdude -c arduino -P /dev/XXX -p m328p


Serial over FTDI Friend
-----------------------

The *Arduino Pro Mini* does not have a USB port, so requires at FTDI USB to serial
adapter. Serial programming requires that a bootloader already be installed:

.. code-block:: console

    $ avrdude -c avrisp -b 57600 -P /dev/ttyUSB0 -p m328p


AVR ICSP
--------

Bare AVR chip, or using ICSP header to burn new bootloader on Arduino boards.

**USBasp Programmer**

.. code-block:: console

    $ avrdude -c usbasp -p m328p

**USBtinyISP**

Does not seem to play well with USB3. Try another cable, or use an
intermediate USB2 hub.

.. code-block:: console

    $ avrdude -c usbtiny -p m328p


Flash Firmware
==============

Add ``-U flash:w:filename:a`` to any of the above commands, eg.:

.. code-block:: console

    $ avrdude -c usbtiny -p m328p -U flash:w:main.hex:a
