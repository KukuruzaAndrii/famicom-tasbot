TAS runner on STM32F411 black pill for FAMICOM

Communication:
Debug logs on uart1; RX - A9, TX - A10 (from uart-ttl perspective)

Connection:
A0 - Latch - on high - start reading bit sequence from "joystick"
A1 - Power - track console powering
A2 - Clock - on low - read bit from "joystick"
A4 - Data - send bit for corresponding button pressed

Maping bit to key:
B_A 		- 0
B_B 		- 1
B_SELECT 	- 2
B_START 	- 3
B_UP 		- 4
B_DOWN 		- 5
B_LEFT 		- 6
B_RIGHT 	- 7