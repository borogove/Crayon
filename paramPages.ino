

int oscillatorShape = 0;
int vibratoDepth = 16;

char text1[64];
char text2[64];
void displayPage1()
{

	//sprintf( text1, "wav %3d vib %3dd", oscillatorShape, vibratoDepth);
	sprintf( text1, "drop %8ld   ", audioChannel.dropCount);
	sprintf( text1, "rd %4ld wr %4ld", audioChannel.readCount%10000,audioChannel.writeCount%10000);

	sprintf( text2, "midi %02X %02X %02x %02X", currentCommand, currentData[0],currentData[1],currentData[2] );
	displayAt(0,0, text1);
	display(text2);

}
