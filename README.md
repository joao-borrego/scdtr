# SCDTR
Distributed Real Time Control Systems

This repository contains the course project and report.
It consists of an automated lighting control system, with a distributed approach to the global optimisation problem.
A brief video of the system response to an automated disturbance was [uploaded][video].

It was tested using 2 Arduino boards for system nodes, each with an LED and LDR as actuator and sensor respectively.
The system was enclosed in a shoe box, coated with reflective paper on the inside so as to reflect and accentuate inter-node coupling.

![System image][system_img]

A TCP/IP server was also developed in order to remotely control and monitor the system, as well as calculate performance heuristics.
The server itself was tested on a Raspberry Pi 3B.
In order for it to extract information from the system an I2C connection to the Arduinos is required.
However, one can be simulated by writing to the monitored FIFO.
Specifics are further discussed in the [report][report].

[report]: report/report.pdf
[system_img]: images/system.jpg "What amazing cable management!"
[video]: https://youtu.be/ojjWJ2vl8jE
