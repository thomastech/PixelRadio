#include <Wire.h>
#include <QN8027Radio.h>

QN8027Radio radio = QN8027Radio();
uint8_t st=0;
uint8_t fsmStatus;
String stats[] = {"Resetting","Recalibrating","Idle","TxReady","PACalib","Transmiting","PA Off"};
void setup() {
  Serial.begin(9600);
  Wire.begin();
  //-------Circuit Crystal--------------------------------------
  //radio.setClockSource(0);
  //radio.setCrystalCurrent(90);
  //radio.setCrystalFreq(12);

  //--------Reset And Antenna ReCalibration---------------------
  radio.reset();
  radio.reCalibrate();//importnt

  //--------Transmitter And RF Power Amplifier------------------
  //radio.setPreEmphTime50(ON);        //No effect.
  radio.setFrequency(88.5);
  radio.setTxFreqDeviation(150);   //Total Broadcast channel Bandwidth
  //radio.radioNoAudioAutoOFF(ON);
  //radio.setTxPower(75); //default is already maximum.
  radio.Switch(ON);

  //--------RDS--------------------------------------------------
  radio.RDS(ON);
  //radio.setRDSFreqDeviation(10); //RDS channel Bandwidth at 57KHz on MPX Spectrum

  //--------Audio------------------------------------------------
  //radio.MonoAudio(ON);
  //radio.mute(ON);
  //radio.scrambleAudio(ON);
  //radio.setTxPilotFreqDeviation(10); //Pilot tone Bandwidth at 19KHz in MPX Spectrum
  //radio.setAudioInpImp(20);      //Audio Input Impedence, lower value boosts volume, 5,10,20,40 KOhm
  //radio.setTxInputBufferGain(4); //volume boost. max 5
  //radio.setTxDigitalGain(2);     //volume boost  max 2

  delay(2000);
  Serial.println("Radio Started");

}

void loop() {
  if(radio.getFSMStatus()!=fsmStatus){
    fsmStatus = radio.getFSMStatus();
    Serial.print("FSM Mode Changed:");
    Serial.println(stats[fsmStatus]);
  }
  radio.sendStationName("QN8027FM");
  Serial.println("StationName Sent!");
  delay(5000);
  radio.sendRadioText("-:Word 'Teacher' is made from 'Teach', Not Knowledge");
  Serial.println("RadioText Sent!");
  delay(5000);
}
