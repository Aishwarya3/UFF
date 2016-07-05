# MTP
Sends or receives frames.


#How to compile

sh install



#How to run

1) To receive frames (blocks until a frame is received) 

	bash#	bin/mtpd 0

2) To send frame

	bash#	bin/mtpd 1 <msg_to_send> <Size_of_the_msg> <MAC_address_of_Destination>
