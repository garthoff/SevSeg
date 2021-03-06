/*
Written by Dean Reading, 2012
Version 2.0; Now works for any digital pin arrangement, common anode and common cathode displays.
 
 Direct any questions or suggestions to deanreading@hotmail.com
 
 This library allows an arduino to easily display numbers in decimal format on
 a 4 digit 7-segment display without a separate 7-segment display controller.
 
 Hardware:
 4 digit 7 segment displays use 12 digital pins.
 
 There are:
 4 common pins; 1 for each digit.  These will be cathodes (negative pins) for
 common cathode displays, or anodes (positive pins) for common anode displays.
 I refer to these as digit pins.
  8 pins for the individual segments (seven segments plus the decimal point).
 I refer to these as segment pins.
 
 Connect the four digit pins with four limiting resistors in series to any digital or analog pins.
 Connect the eight cathodes to any digital or analog pins.
 
 I have a cheap one from China, and the pins of the display are in the following order:
 Top Row
 1,a,f,2,3,b
 Bottom Row
 e,d,dp,c,g,4
 Where the digit pins are 1-4 and the segment pins are a-g + dp
 
 Software:
 Call SevSeg.Begin in setup.  
 The first argument (boolean) tells whether the display is common cathode (0) or common
 anode (1).
 The next four arguments (bytes) tell the library which arduino pins are connected to
 the digit pins of the seven segment display.  Put them in order from left to right.
 The next eight arguments (bytes) tell the library which arduino pins are connected to
 the segment pins of the seven segment display.  Put them in order a to g then the dp.
 
 In summary, Begin(type, digit pins 1-4, segment pins a-g,dp)
 
 The calling program must run the PrintOutput() function repeatedly to get
 the number displayed.
 To set the number displayed, use the NewNum function.  Any number between
 -999 and 9999 can be displayed. Out of range numbers show up as '----'.
 To move the decimal place one digit to the left, use '1' as the second
 argument in NewNum. For example, if you wanted to display '3.141' you would
 call NewNum(3141,3);
 
 */

#include "SevSeg.h"

SevSeg::SevSeg()
{
  //Initial values
  //number = 8888;
  DecPlace = 0;

}

//Begin
/*******************************************************************************************/
//Set pin modes and turns all displays off
void SevSeg::Begin(boolean mode_in, byte numOfDigits, byte dig1, byte dig2, byte dig3, byte dig4, 
	byte segA, byte segB, byte segC, byte segD, byte segE, byte segF, byte segG, 
	byte segDP){

  //Bring all the variables in from the caller
  numberOfDigits = numOfDigits;
  digit1 = dig1;
  digit2 = dig2;
  digit3 = dig3;
  digit4 = dig4;
  segmentA = segA;
  segmentB = segB;
  segmentC = segC;
  segmentD = segD;
  segmentE = segE;
  segmentF = segF;
  segmentG = segG;
  segmentDP = segDP;
  
  //Assign input values to variables
  //mode is what the digit pins must be set at for it to be turned on.  0 for common cathode, 1 for common anode
  mode = mode_in;
  if(mode == COMMON_ANODE){
    DigitOn = HIGH;
    DigitOff = LOW;
    SegOn = LOW;
    SegOff = HIGH;
  }
  else {
    DigitOn = LOW;
    DigitOff = HIGH;
    SegOn = HIGH;
    SegOff = LOW;
  }

  DigitPins[0] = digit1;
  DigitPins[1] = digit2;
  DigitPins[2] = digit3;
  DigitPins[3] = digit4;
  SegmentPins[0] = segmentA;
  SegmentPins[1] = segmentB;
  SegmentPins[2] = segmentC;
  SegmentPins[3] = segmentD;
  SegmentPins[4] = segmentE;
  SegmentPins[5] = segmentF;
  SegmentPins[6] = segmentG;
  SegmentPins[7] = segmentDP;


  //Set Pin Modes as outputs
  for (byte digit = 0 ; digit < numberOfDigits ; digit++) {
    pinMode(DigitPins[digit], OUTPUT);
  }
  for (byte seg = 0 ; seg < 8 ; seg++) {
    pinMode(SegmentPins[seg], OUTPUT);
  }

  //Turn Everything Off
  //Set all digit pins off. Low for common anode, high for common cathode
  for (byte digit = 0 ; digit < numberOfDigits ; digit++) {
    digitalWrite(DigitPins[digit], DigitOff);
  }
  //Set all segment pins off. High for common anode, low for common cathode
  for (byte seg = 0 ; seg < 8 ; seg++) {
    digitalWrite(SegmentPins[seg], SegOff);
  }
}

//Refresh Display
/*******************************************************************************************/
//Given 1022, we display "10:22"
//Each digit is displayed for ~2000us, and cycles through the 4 digits
//After running through the 4 numbers, the display is turned off
void SevSeg::DisplayNumber(int toDisplay, byte DecPlace){

	//For the purpose of this code, digit = 1 is the left most digit, digit = 4 is the right most digit

	//If the number received is negative, set the flag and make the number positive
	boolean negative = false;
	if (toDisplay < 0) {
		negative = true;
		toDisplay = toDisplay * -1;
	}


	for(byte digit = numberOfDigits ; digit > 0 ; digit--) {

		//Turn on a digit for a short amount of time
		switch(digit) {
			case 1:
			  digitalWrite(digit1, DigitOn);
			  break;
			case 2:
			  digitalWrite(digit2, DigitOn);
			  break;
			case 3:
			  digitalWrite(digit3, DigitOn);
			  break;
			case 4:
			  digitalWrite(digit4, DigitOn);
			  break;

			//This only currently works for 4 digits

		}

		if(toDisplay > 0) //The if(toDisplay>0) trick removes the leading zeros
			lightNumber(toDisplay % 10); //Now display this digit
		else if(negative == true) { //Special case of negative sign out in front
			lightNumber(DASH);
			negative = false; //Now mark negative sign as false so we don't display it multiple times
		}
		
		//Service the decimal point
		if(DecPlace == digit)
			lightNumber(DP);

		toDisplay /= 10;

		delayMicroseconds(2000); //Display this digit for a fraction of a second (between 1us and 5000us, 500-2000 is pretty good)
		//If you set this too long, the display will start to flicker. Set it to 25000 for some fun.

		//Turn off all segments
		lightNumber(BLANK);

		//Turn off all digits
		digitalWrite(digit1, DigitOff);
		digitalWrite(digit2, DigitOff);
		digitalWrite(digit3, DigitOff);
		digitalWrite(digit4, DigitOff);
	}
  
}

//Light up a given number
/*******************************************************************************************/
//Given a number, light up the right segments
//We assume the caller has the digit's Anode/Cathode turned on
void SevSeg::lightNumber(byte numberToDisplay) {

	//Segments in the display are named like this:
	//    -  A
    // F / / B
	//    -  G
	// E / / C
	//    -  D
  
    switch (numberToDisplay){

	  case 0:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentE, SegOn);
		digitalWrite(segmentF, SegOn);
		break;

	  case 1:
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		break;

	  case 2:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentE, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case 3:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case 4:
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentF, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case 5:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentF, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case 6:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentE, SegOn);
		digitalWrite(segmentF, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case 7:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		break;

	  case 8:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentE, SegOn);
		digitalWrite(segmentF, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case 9:
		digitalWrite(segmentA, SegOn);
		digitalWrite(segmentB, SegOn);
		digitalWrite(segmentC, SegOn);
		digitalWrite(segmentD, SegOn);
		digitalWrite(segmentF, SegOn);
		digitalWrite(segmentG, SegOn);
		break;

	  case BLANK:
		digitalWrite(segmentA, SegOff);
		digitalWrite(segmentB, SegOff);
		digitalWrite(segmentC, SegOff);
		digitalWrite(segmentD, SegOff);
		digitalWrite(segmentE, SegOff);
		digitalWrite(segmentF, SegOff);
		digitalWrite(segmentG, SegOff);
		digitalWrite(segmentDP, SegOff);
		break;

      case DASH:
		digitalWrite(segmentG, SegOn);
		break;
	  
	  case DP:
	    digitalWrite(segmentDP, SegOn);
		break;

/*	
		default:
		  lights[digit][0] = 0;
		  lights[digit][1] = 0;
		  lights[digit][2] = 1;
		  lights[digit][3] = 1;
		  lights[digit][4] = 1;
		  lights[digit][5] = 0;
		  lights[digit][6] = 1;
		  break;
		  */
    }

    //Set the decimal place lights
    /*if (numberOfDigits - 1 - digit == DecPlace){
      lights[digit][7] = 1;
    }
    else {
      lights[digit][7] = 0;
    }*/
}