/*************************************************************
 CompactQik2s9v1 - basic class to control Pololu's Qik2s9v1
 motor controller (http://www.pololu.com/catalog/product/1110)
 
 This uses the default settings for the motor controller and the 
 Compact Protocol to communicate to it.

 This library also depends on the NewSoftSerial library which
 can be found at: http://arduiniana.org.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Author:  Eric Fialkowski (eric.fialkowski@gmail.com)
 History:
 Version 1.0	May 8, 2009		Initial code

************************************************************/

#include "CompactQik2s9v1.h"

CompactQik2s9v1::CompactQik2s9v1(NewSoftSerial *serial, uint8_t resetPin)
{   
	_serial = serial;
	_resetPin = resetPin;	
	_errByte = -1;
	pinMode(_resetPin, OUTPUT);
}

// needs to be called before using
void CompactQik2s9v1::begin()
{
  digitalWrite(_resetPin, LOW);
  delay(100);
  digitalWrite(_resetPin, HIGH);
  delay(10);

  sendByte(INITIALPACKET);
  
  delay(100);
}

void CompactQik2s9v1::motor0Forward(uint8_t speed)
{
	sendByte(MOTOR0FORWARDPACKET);
	sendByte(speed);
}

void CompactQik2s9v1::motor1Forward(uint8_t speed)
{
	sendByte(MOTOR1FORWARDPACKET);
	sendByte(speed);
}

void CompactQik2s9v1::motor0Reverse(uint8_t speed)
{
	sendByte(MOTOR0REVERSEPACKET);
	sendByte(speed);
}

void CompactQik2s9v1::motor1Reverse(uint8_t speed)
{
	sendByte(MOTOR1REVERSEPACKET);
	sendByte(speed);
}

void CompactQik2s9v1::motor0Coast()
{
	sendByte(MOTOR0COASTPACKET);
}

void CompactQik2s9v1::motor1Coast()
{
	sendByte(MOTOR1COASTPACKET);
}

/*
 *  Stopping Methods
 */
void CompactQik2s9v1::stopBothMotors()
{
	stopMotor0();
	stopMotor1();
}

void CompactQik2s9v1::stopMotor0()
{
	motor0Forward(0);
}

void CompactQik2s9v1::stopMotor1()
{
	motor1Forward(0);
}

uint8_t CompactQik2s9v1::getFirmwareVersion()
{
	if ( _fwVersion == 0)
	{
		sendByte(FWVERSIONPACKET);
		_fwVersion = readByte();
	}
	return _fwVersion;
}

uint8_t CompactQik2s9v1::getError()
{
	sendByte(ERRORPACKET);
	_errByte = readByte();
	return _errByte;
}


bool CompactQik2s9v1::hasDataOverrunError()
{
	return hasDataOverrunError(false);
}
bool CompactQik2s9v1::hasDataOverrunError(bool fetchError)
{
	return errorBitSet(DATAOVERRUNERRORBIT,fetchError);
}

bool CompactQik2s9v1::hasFrameError()
{
	return hasFrameError(false);
}
bool CompactQik2s9v1::hasFrameError(bool fetchError)
{
	return errorBitSet(FRAMEERRORBIT, fetchError);
}

bool CompactQik2s9v1::hasCRCError()
{
	return hasCRCError(false);
}
bool CompactQik2s9v1::hasCRCError(bool fetchError)
{
	return errorBitSet(CRCERRORBIT, fetchError);
}

bool CompactQik2s9v1::hasFormatError()
{
	return hasFormatError(false);
}
bool CompactQik2s9v1::hasFormatError(bool fetchError)
{
	return errorBitSet(FORMATERRORBIT, fetchError);
}

bool CompactQik2s9v1::hasTimeoutError()
{
	return hasTimeoutError(false);
}

bool CompactQik2s9v1::hasTimeoutError(bool fetchError = false)
{
	return errorBitSet(TIMEOUTERRORBIT, fetchError);
}

/*
 *	Private Helpers
 */
bool CompactQik2s9v1::errorBitSet(uint8_t bitToCheck, bool fetchError)
{
	if ( fetchError )
	{
		getError();
	}
	return (bitRead(_errByte, bitToCheck) == 1);
}

void CompactQik2s9v1::sendByte(uint8_t byteToSend)
{
	_serial->print(byteToSend,BYTE);
}

uint8_t CompactQik2s9v1::readByte()
{
	return _serial->read();
}