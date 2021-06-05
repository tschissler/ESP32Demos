# Azure Function to store sensor data in table storage
You can use the Azure Extension in VS Code to create a new Azure Function

In the IoT Hub go to "Build-in Endpoints" under "Settings" and copy the "Event Hub-compatible endpoint" to an application parameter named "IoTHubEndPoint" (Application Settings in the Azure Extension section for Functions). 

Provide a name for a table and the connection string for the storage account in your code (\<StorageEndpoint>).

Then right-click the folder your function is in (05-AzureFunction) in the VS Code Explorer and deploy it to a function app. AFter successful deployment you can open the stream log and you should see an event each time the device sends sensor data to the IoT Hub.

You can install the Azure Storage Explorer (https://aka.ms/portalfx/downloadstorageexplorer) to explore data stored in the table storage.


