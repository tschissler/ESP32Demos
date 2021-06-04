# Azure IoT Hub Demo
In this demo we connect our device to Azure IoT Hub via WiFi and push sensor data to the cloud. 
At the same time we allow to update values for the RGB LED on the digital twin which are then send from the cloud to the device.
For this demo it can be helpful to install the Azure IoT Hub extension in VS Code (https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.azure-iot-toolkit).

You need an active Azure Subscription. If you do not have one, you can create a new one which comes with plenty of free offers and a $200 budget (https://azure.microsoft.com/en-us/free/).

In the subscription you have to create a new IoT Hub. You can do so via the Azure portal (https://portal.azure.com) or the Azure IoT Hub Extension in VS Code.

For this demo the F1: Free tier should be sufficient.

In the IoT Hub create a new device. Use the connectionString in your code to define the device you want to connect to (line 27). Also set the WiFi parameters accordingly.

In the Azure Portal change the Device Twin to change the color of the LED

```json
...
    "properties": {
        "desired": {
            "red": 255,
            "green": 0,
            "blue": 255,
            "$metadata": {
...
```


