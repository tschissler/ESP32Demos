using System.Collections.Generic;
using System.Net;
using IoTHubTrigger = Microsoft.Azure.WebJobs.EventHubTriggerAttribute;
using System.Net.Http;
using System.Text;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Host;
using Microsoft.Azure.EventHubs;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Azure.Functions.Worker.Http;
using Microsoft.Extensions.Logging;
using Microsoft.WindowsAzure.Storage;
using Microsoft.WindowsAzure.Storage.Table;
using Newtonsoft.Json.Linq;

namespace tschissler.ESP32Demos
{
    public static class IoTFunctions
    {
        private static CloudTable _outputTable = CloudTableHelper.GetCloudTable("IoTData", "<connectionstring>");

        [FunctionName("ReceiveIotHubTelemetryTrigger")]
        public static void Run([IoTHubTrigger("messages/events", Connection = "IoTHubConnectionString")]EventData message, ILogger log)
        {
            var messageStr = Encoding.UTF8.GetString(message.Body.Array);
            messageStr = messageStr.Replace("nan", "\"nan\"");
            log.LogInformation($"C# IoT Hub trigger function processed a message: {messageStr}");
            var data = JObject.Parse(messageStr);

            var dynamicTableEntity = new DynamicTableEntity();

            foreach (KeyValuePair<string, JToken> keyValuePair in data)
            {
                if (keyValuePair.Key.Equals("deviceId"))
                {
                    dynamicTableEntity.PartitionKey = keyValuePair.Value.ToString();

                }
                else if (keyValuePair.Key.Equals("messageId"))
                {
                    dynamicTableEntity.RowKey = keyValuePair.Value.ToString();
                }
                else
                {
                    dynamicTableEntity.Properties.Add(keyValuePair.Key, EntityProperty.CreateEntityPropertyFromObject(keyValuePair.Value));
                }
            }

            if (dynamicTableEntity.PartitionKey == null)
            {
                dynamicTableEntity.PartitionKey = "KellerM1";
            }

            var tableOperation = TableOperation.InsertOrMerge(dynamicTableEntity);
            _outputTable.ExecuteAsync(tableOperation).Wait();
        }
    }

    public class CloudTableHelper
    {
        public static CloudTable GetCloudTable(string tableName, string storageConnectionString)
        {
            // Retrieve storage account from connection string.
            CloudStorageAccount storageAccount = CloudStorageAccount.Parse(storageConnectionString);

            // Create the table client.
            CloudTableClient tableClient = storageAccount.CreateCloudTableClient();

            // Retrieve a reference to a table.
            CloudTable table = tableClient.GetTableReference(tableName);

            // Create the table if it doesn't already exist
            table.CreateIfNotExistsAsync().Wait();

            return table;
        }
    }    
}
