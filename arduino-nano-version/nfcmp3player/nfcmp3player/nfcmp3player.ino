#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

#include "PN532_SWHSU.h"

#include <PN532.h>
#include <NfcAdapter.h>

SoftwareSerial SWSerial1( 8, 9 ); // RX, TX
SoftwareSerial SWSerial2( 10, 11 ); // RX, TX

PN532_SWHSU pn532swhsu( SWSerial1 );

//PN532 nfc( pn532swhsu );
NfcAdapter nfc = NfcAdapter(pn532swhsu);


// forward declare the notify class, just the name
//
class Mp3Notify; 

// define a handy type using serial and our notify class
//
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3; 

// instance a DfMp3 object, 
//
DfMp3 dfmp3(SWSerial2);

void setup(void) {
  Serial.begin(115200);

  Serial.println("initializing...");
  
  dfmp3.begin();
  dfmp3.reset(); 
  
  // show some properties and set the volume
  uint16_t volume = dfmp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.setVolume(15);
  
  uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);

  uint16_t mode = dfmp3.getPlaybackMode();
  Serial.print("playback mode ");
  Serial.println(mode);
  
  Serial.println("starting...");  
  dfmp3.playRandomTrackFromAll();
 
  
  delay(20000);
  nfc.begin();

}

void loop(void) {
    Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();         
        if(tag.hasNdefMessage()) {            
            NdefMessage message = tag.getNdefMessage();
            for(int i=0;i<message.getRecordCount();i++) {
              NdefRecord record = message.getRecord(i);              
              int payloadLength = record.getPayloadLength();
              byte payload[payloadLength];
              record.getPayload(payload);
              String payloadAsString = "";
              for (int c = 1; c < payloadLength; c++) {
                payloadAsString += (char)payload[c];
              }
              //Serial.println(payloadAsString);

              if (payloadAsString.equals("en0")) { 
                  Serial.println(0);
                   dfmp3.playMp3FolderTrack(1);
              } else if(payloadAsString.equals("en1")) {                    
                  Serial.println(1);
                   dfmp3.playMp3FolderTrack(2);
              } else if (payloadAsString.equals("en2")) {
                  Serial.println(2);
                   dfmp3.playMp3FolderTrack(3);
              } else if (payloadAsString.equals("en3")) { 
                  dfmp3.setVolume(30);                                 
                  Serial.println(4);
              } else if (payloadAsString.equals("en4")) {
                  dfmp3.setVolume(15);             
                  Serial.println(5);
              } 
            }        
          }
    }    
    delay(1000);
}
