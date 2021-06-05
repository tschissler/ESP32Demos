# Over the Air Update Demo

this demo shows how we can update the firmware over the air (OTA) via WiFi-connection. The device connects to a WiFi network and then downloads the new firmware version after 20 seconds. In this demo you can see the new firmware as the LED is blinking blue instead of red.

To execute this demo there are several steps necessary:

1. Compile the project 07-OTAUpdateVersion
1. Upload the firmware.bin from the sub-folder \\.pio\build\esp32dev to a blob storage container in Azure
1. Copy the URL of the file from the blob storage into the parameter <blobstoragefileURL> in the main.cpp in the 06-OTAUpdate project
1. Flash the 06-OTAUpdate project onto your device 
1. The device tries to connect to your WiFi (see serial monitor). If it cannot connect, it will create a WiFi access point you can connect to via your phone or a different device. This will open a web page where you can setup the WiFi connection for your device
1. Now the LED will flash red twenty times before the firmware update starts
1. After a couple of seconds the LED starts flashing blue which indicates that the new firmware is now active

This demo allows only one update cycle. To improve this even further seet the 08-DeviceTwinOTAUpdate project