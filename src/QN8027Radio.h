/*
Written By ManojBhakarPCM with little help of other's code copy paste

This file has been modified by TEB for use with the PixelRadio Project.
The original QN8027 Arduino Library is incompatible, but can be downloaded from:
https://github.com/ManojBhakarPCM/Arduino-QN8027-with-Full-RDS-support
Edits by TEB, Feb-03-2022
*/

#include <Wire.h>
#include "PixelRadio.h"
#ifndef QN8027Radio_h
#define QN8027Radio_h

#define			QN8027_I2C_ADDR		  0x2C

#define         RADIOTEXT_SIZE        64
#define         PSN_SIZE              8

//QN8027 Register
#define     	SYSTEM_REG            0x00
#define     	CH1_REG               0x01
#define     	GPLT_REG              0x02
#define     	XTL_REG               0x03
#define     	VGA_REG               0x04
#define     	CID1_REG              0x05
#define     	CID2_REG              0x06
#define     	STATUS_REG            0x07
#define     	RDSD0_REG             0x08
#define     	RDSD1_REG             0x09
#define     	RDSD2_REG             0x0A
#define     	RDSD3_REG             0x0B
#define     	RDSD4_REG             0x0C
#define     	RDSD5_REG             0x0D
#define     	RDSD6_REG             0x0E
#define     	RDSD7_REG             0x0F
#define     	PAC_REG               0x10
#define     	FDEV_REG              0x11
#define     	RDS_REG               0x12
#define     	ANT_REG               0x1E



//indicate self definition
#define 		ON				  	  0x01
#define			OFF				  	  0x00
#define 		CH0_MASK			  0x03
#define 		POWER_MAX			  75
#define			POWER_MIN			  20


class QN8027Radio
{
private:
  uint8_t _address = QN8027_I2C_ADDR; // TEB, MAR-07-2022
  uint8_t freqH = 0x00;               // TEB, MAR-07-2022

public:
  //SYSTEM
  uint8_t radioStatus = 32;			//32==ON, 0==OFF
  uint8_t rdsReady = 0;				//Toggle between 4 And 0
  uint8_t monoAudio = 0;			//16==mono , 0=Stereo
  uint8_t muteAudio = 0; 			//8==mute ON, 0==mute OFF
  uint16_t piCode  = RDS_PI_CODE_DEF;
  uint8_t  ptyCode = 0xA;           // "Top 40"

  //XPLT
  uint8_t preEmphTime = 128; 		//128==75uS, 0==50uS
  uint8_t privateMode = 0;   		//0==disabled, 64==enabled
  uint8_t PAAutoOffTime = 60; 		//0==58s, 16==59s, 32==60s, 48==Never
  uint8_t TxPilotFreqDeviation = 9; //N == N% * 75KHz (N={7,8,9,10})

  //XTL
  uint8_t clockSource = 0; 			//0==Crystal at 1:2, 1== DigitalClockInject at 1, 2==SinWave at 1, 3==DiffSinWave at 1:2
  uint8_t CrystalCurrentuA = 16; 	// actual current = CrystalCurrentuA x 6.25uA

  //VGA
  uint8_t crystalFreqMHz = 128;		//0==12MHz, 128==24MHz
  uint8_t TxInputBufferGain = 48;   //(N << 4), Gain = [(N+1)*3] - [LRInputImpdKOhm*6]    N={0 to 5}
  uint8_t TxDigitalGain = 0;		// Gain = (N << 2) dB;  N={0,1,2}
  uint8_t LRInputImpdKOhm = 2;		// Impedence = 5*2^(N+1);   N={0,1,2,3}

  //FDEV
  uint8_t TxFreqDeviation = 129;	// TxFreqDeviation = N * 0.58 KHz ; N={0 to 255}

  //RDS
  uint8_t RDSEnable = 128;			//128 = enabled, 0= disabled
  uint8_t RDSFreqDeviationKHz = 6;  //dev = N * 0.35 KHz;  N={0 to 127}

  //PAC
  uint8_t AudioPeakClear = 0;		//Toggle between 1 and 0 to clear audioInputMaxPeak
  uint8_t PAOutputPower = 75;		//PowerAmp Output = 0.62*N + 71 dBu ; N={20 to 75}

  uint8_t rdsSentStatus = 0;		//Toggle between 8 and 0 when RDS is sent successfully.




  QN8027Radio();
  QN8027Radio(int address);
  void write1Byte(uint8_t regAddr,uint8_t comData);

  void setFrequency(float frequency);
  void reset();
  void reCalibrate();
  void mute(uint8_t onOffCtrl);
  void MonoAudio(uint8_t onOffCtrl);
  void scrambleAudio(uint8_t onOffCtrl);
  void setPreEmphTime50(uint8_t onOffCtrl);
  void Switch(uint8_t onOffCtrl); //radioPower
  void sendRDS(char By0,char By1,char By2,char By3,char By4,char By5,char By6,char By7);
  void sendStationName(String SN);
  void sendRadioText(String RT);
  void waitForRDSSend();


  float getFrequency();
  uint8_t read1Byte(uint8_t regAddr);
  uint8_t canRDSbeSent();
  uint8_t getFSMStatus();
  uint8_t getAudioInpPeak();
  uint8_t getStatus();
  uint16_t getPiCode(void);
  uint8_t getPTYCode();

  void updateSYSTEM_REG();
  void updateGPLT_REG();
  void updateXTL_REG();
  void updateVGA_REG();

  void setCrystalFreq(uint8_t Freq);
  void setTxInputBufferGain(uint8_t IBGain);
  void setTxDigitalGain(uint8_t DGain);
  void setAudioInpImp(uint8_t impdInKOhms);


  void setClockSource(uint8_t Type);
  void setCrystalCurrent(float percentOfMax);

  void radioNoAudioAutoOFF(uint8_t onOffCtrl);
  void setTxPower(uint8_t setX);
  void clearAudioPeak();

  void setTxPilotFreqDeviation(uint8_t PGain);
  void setTxFreqDeviation(uint8_t Fdev);
  void setPiCode(uint16_t piCodeVal);
  void setPTYCode(uint8_t ptyCodeVal);

  void RDS(uint8_t onOffCtrl);
  void setRDSFreqDeviation(uint8_t RDSFreqDev);

};


#endif