#!/bin/sh


insmod cmemk.ko phys_start=0x83200000 phys_end=0x88000000 allowOverlap=1 phys_start_1=0x00001000 phys_end_1=0x00008000 pools_1=1x28672

insmod edmak.ko
insmod irqk.ko
