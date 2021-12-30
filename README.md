This project is a fork of the Hayesduino project by Payton Byrd.

What Is It?

Hayesduino is an Arduino sketch that provides a bridge between the world of the Internet and small devices that do not have built-in ethernet capabilities. Old computers, such as the Commodore 64, Apple II and Atari 800 have serial ports, but do not have readily available Internet solutions with wide software support. While specialized solutions do exist for these platforms, they all require specialized software to use them and do not lend themselves to more general usage such as simply opening a socket, sending some data, and/or receiving some data.

Hayesduino bridges this gap by emulating a Hayes compatible modem. This allows users to initiate Internet communications via sockets that are opened by "dialing" to a hostname and port. An example would be initiating a telnet session with a host by simply typing atdt hostname:23 and waiting for the host to respond. Using this technique, any online socket can be reached and communicated with.

Hayesduino could have accomplished this without emulating a modem, but there needed to be a good way to allow the small machine to receive incoming connections. The three platforms listed above were all very popular systems for hosting BBS (bulletin board systems) which would accept calls over a telephone line via modem. Hayesduino simulates the incoming phone call whenever the software receives an inbound connection on port 23 (this is changeable in the code). When an incoming connection is detected, the Hayesduino will toggle the DCE-DCD line to trigger the remote software to answer the incoming "call". In this way a classic BBS can be hooked up directly to the Internet.

History

Hayesduino started in early 2013 as the firmware for the Comet BBS product which is to be sold as a commercial product. The owner of the Comet BBS project and Payton Byrd agreed that the firmware would be made open-source as the Comet BBS product is based on the Arduino Uno and thus easily reproducible as a hobby project. Although the Comet BBS hardware project appears to be stalled, the development of the firmware is not stalled and I, Payton Byrd, have decided to move forward with open-sourcing the firmware as the Hayesduino project.
