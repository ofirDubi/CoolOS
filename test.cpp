/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   test.cpp
 * Author: ofir123dubi
 *
 * Created on December 16, 2017, 9:52 AM
 */

#include <cstdlib>
#include <iostream>
using namespace std;
void write(uint8_t size, uint8_t data, uint16_t port_number){
    char command[] = "out_ %0, %1\n";
    command[3] = size; //outb, outw or outl
    __asm__ volatile(command: : "a"(data), "Nd"(port_number) );
}
/*
 * 
 */
int main(int argc, char** argv) {
    char size = 'b';
    char data = 'a';
    short int port_number = 3;
    //const char str[] = { 'o','u','t', size, ' ','%','0',',',' ','%','1'};
    char command[] = "out_ %0, %1\n";
    command[3] = size; //outb, outw or outl
    __asm__ volatile(command : : "a"(data), "Nd"(port_number) );
    return 0;
}

