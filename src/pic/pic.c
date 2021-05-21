#include "pic.h"
#include "io/io.h"
#include "kernel.h"

void init_pic()
{
    /* set up cascading mode */
	outb(PIC_MASTER_CMD, 0x10 + 0x01);
	outb(PIC_SLAVE_CMD,  0x10 + 0x01);
	/* Setup master's vector offset */
	outb(PIC_MASTER_DATA, 0x20);
	/* Tell the slave its vector offset */
	outb(PIC_SLAVE_DATA, 0x28);
	/* Tell the master that he has a slave */
	outb(PIC_MASTER_DATA, 4);
	outb(PIC_SLAVE_DATA, 2);
	/* Enabled 8086 mode */
	outb(PIC_MASTER_DATA, 0x01);
	outb(PIC_SLAVE_DATA, 0x01);

	outb(PIC_MASTER_DATA, 0);
	outb(PIC_SLAVE_DATA, 0);
}
