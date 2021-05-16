#ifndef IO_H
#define IO_H

unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);
unsigned int insl(unsigned int port);

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);
void outl(unsigned short port, unsigned int val);

#endif