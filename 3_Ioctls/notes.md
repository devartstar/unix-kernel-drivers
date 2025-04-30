## IOCTL Commands in a Driver
-	Userspace Programs ---> REQUEST ---> Kernel Driver
-	Magic Numbers:
	-	Kernel can have many drivers loaded at the same time.
	-	Each driver can define their own ioctl commands.
	-	To prevent 2 drivers form using the same ioctl number
		Each driver chooses its own magic number.

- Every IOCTL Commands have 4 parts: (in a 32bit MACRO)
	1.	Direction
	2.	Magic Number
	3.	Command Number
	4.	DataType Size


