# Win32Api programs(6)
This repository contains 6 different programs on different topics.

## Bouncing Sprite (GDI, GRAPHIC, RESOURCES, MESSAGES, WINDOW)
Simple winapi program to move .bmp sprite with mouser and up/down/left/right keys.

## Table (GDI, GRAPHIC, RESOURCES, MESSAGES, WINDOW, FILES, FONTS)
Program that draws table with numbers fitting in it.

## DLL injection (DLL, REMOTE THREADS, PROCESSES, STATIC/DYNAMIC DLL IMPORTS)
Programs that executes static/dynamic dll imports, injecting dll in victim process with remote thread.

## MultiThreadingSort (THREADING, MERGE SORT, MUTEX, SYNCHRONIZATION, CONCURRENT QUEUES)
Program reads file content, splites it on several parts, threads dequeues each part, sorts it, then main calling thread sorts all parts with merge sort.

## Driver
Driver waites for process to be started. Then calc.exe started, when the first process terminated, driver terminates calc.exe.
