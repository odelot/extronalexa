var AWS = require('aws-sdk');
AWS.config.update({region: 'us-west-2'});
exports.handler = async (event) => {
    console.log (event);
    if (event.directive.header.name == 'Discover') {
        const response = {
            "event": {
                "header": {
                    "namespace": "Alexa.Discovery",
                    "name": "Discover.Response",
                    "payloadVersion": "3",
                    "messageId": event.directive.header.messageId
                },
                "payload": {
                    "endpoints": [
                                 {
                                    "endpointId": 'fabio-casa-gameroom-snes',
                                    "manufacturerName": 'Nubix',
                                    "friendlyName": 'super nintendo',
                                    "description": 'super nintendo',
                                    "displayCategories": ['GAME_CONSOLE'],
                                    "cookie": {},
                                    "capabilities": [
                                        
                                                {
                                                    "type": "AlexaInterface",
                                                    "interface": "Alexa.PowerController",
                                                    "version": "3",
                                                    "properties": {
                                                        "supported": [
                                                            { "name": "powerState" }
                                                        ],
                                                        "proactivelyReported": true,
                                                        "retrievable": true
                                                    }
                                                }
                                        ]
                                },
                                {
                                    "endpointId": 'fabio-casa-gameroom-nes',
                                    "manufacturerName": 'Nubix',
                                    "friendlyName": 'nintendinho',
                                    "description": 'nintendinho',
                                    "displayCategories": ['GAME_CONSOLE'],
                                    "cookie": {},
                                    "capabilities": [
                                        
                                                {
                                                    "type": "AlexaInterface",
                                                    "interface": "Alexa.PowerController",
                                                    "version": "3",
                                                    "properties": {
                                                        "supported": [
                                                            { "name": "powerState" }
                                                        ],
                                                        "proactivelyReported": true,
                                                        "retrievable": true
                                                    }
                                                }
                                        ]
                                },
                                {
                                    "endpointId": 'fabio-casa-gameroom-nes2',
                                    "manufacturerName": 'Nubix',
                                    "friendlyName": 'NES',
                                    "description": 'NES',
                                    "displayCategories": ['GAME_CONSOLE'],
                                    "cookie": {},
                                    "capabilities": [
                                        
                                                {
                                                    "type": "AlexaInterface",
                                                    "interface": "Alexa.PowerController",
                                                    "version": "3",
                                                    "properties": {
                                                        "supported": [
                                                            { "name": "powerState" }
                                                        ],
                                                        "proactivelyReported": true,
                                                        "retrievable": true
                                                    }
                                                }
                                        ]
                                },
                                 {
                                    "endpointId": 'fabio-casa-gameroom-n64',
                                    "manufacturerName": 'Nubix',
                                    "friendlyName": 'nintendo 64',
                                    "description": 'nintendo 64',
                                    "displayCategories": ['GAME_CONSOLE'],
                                    "cookie": {},
                                    "capabilities": [
                                        
                                                {
                                                    "type": "AlexaInterface",
                                                    "interface": "Alexa.PowerController",
                                                    "version": "3",
                                                    "properties": {
                                                        "supported": [
                                                            { "name": "powerState" }
                                                        ],
                                                        "proactivelyReported": true,
                                                        "retrievable": true
                                                    }
                                                }
                                        ]
                                },
                                {
                                    "endpointId": 'fabio-casa-gameroom-gc',
                                    "manufacturerName": 'Nubix',
                                    "friendlyName": 'Gamecube',
                                    "description": 'Gamecube',
                                    "displayCategories": ['GAME_CONSOLE'],
                                    "cookie": {},
                                    "capabilities": [
                                        
                                                {
                                                    "type": "AlexaInterface",
                                                    "interface": "Alexa.PowerController",
                                                    "version": "3",
                                                    "properties": {
                                                        "supported": [
                                                            { "name": "powerState" }
                                                        ],
                                                        "proactivelyReported": true,
                                                        "retrievable": true
                                                    }
                                                }
                                        ]
                                },
                                 {
                                    "endpointId": 'fabio-casa-gameroom-wii',
                                    "manufacturerName": 'Nubix',
                                    "friendlyName": 'Wii',
                                    "description": 'Wii',
                                    "displayCategories": ['GAME_CONSOLE'],
                                    "cookie": {},
                                    "capabilities": [
                                        
                                                {
                                                    "type": "AlexaInterface",
                                                    "interface": "Alexa.PowerController",
                                                    "version": "3",
                                                    "properties": {
                                                        "supported": [
                                                            { "name": "powerState" }
                                                        ],
                                                        "proactivelyReported": true,
                                                        "retrievable": true
                                                    }
                                                }
                                        ]
                                },
                        ]
                }
            }
        }
        
        
        return response;
    }
    if (event.directive.header.name == 'ReportState') {
      //videogame
      const response = {
        "event": {
          "header": {
            "namespace": "Alexa",
            "name": "StateReport",
            "messageId": event.directive.header.messageId,
            "correlationToken": event.directive.header.correlationToken,
            "payloadVersion": "3"
          },
          "endpoint": {
            "endpointId": event.directive.endpoint.endpointId
          },
          "payload": {}
        },
        "context": {
          "properties": [
            {
              "namespace": "Alexa.PowerController",
              "name": "powerState",
              "value": 'ON',
              "timeOfSample": new Date().toISOString(),
              "uncertaintyInMilliseconds": 500
            },
            {
              "namespace": "Alexa.EndpointHealth",
              "name": "connectivity",
              "value": {
                "value": "OK"
              },
              "timeOfSample": new Date().toISOString(),
              "uncertaintyInMilliseconds": 0
            }
          ]
        }
      }
      return response;
        
    }
   
    if (event.directive.header.namespace == 'Alexa.PowerController') {
        try {
             
        var iotdata = new AWS.IotData({endpoint: 'yourEndpoint.iot.us-west-2.amazonaws.com'});
        
        //videogame
        const console = event.directive.endpoint.endpointId.replace('fabio-casa-gameroom-','');
        const consoleMap = { "nes": "1", "nes2": "1", "snes" : "2", "n64" : "3", "gc" : "4", "wii": "5" };
        const channel = consoleMap[console];
        params = {
            topic: 'fabio/casa/gameroom',
            payload: `{"command": "switch", "in" : "${channel}" }`,
            qos: 0
        };
        
        console.log (params);
        
        var result =await  iotdata.publish(params).promise();
        console.log (result);
        
        
        const response = {
            "context": {
                "properties": [
                    {
                        "namespace": "Alexa.PowerController",
                        "name": "powerState",
                        "value": 'ON',
                        "timeOfSample": new Date().toISOString(),
                        "uncertaintyInMilliseconds": 500
                    }
                ]
            },
            "event": {
                "header": {
                    "namespace": "Alexa",
                    "name": "Response",
                    "payloadVersion": "3",
                    "messageId": event.directive.header.messageId,
                    "correlationToken": event.directive.header.correlationToken
                },
                "endpoint": {
                    "endpointId": event.directive.endpoint.endpointId
                },
                "payload": {}
            }
        }
        console.log (response);
        return response;
        } catch (e) {
            console.error (e);
        }
        
        
    }
    const response = {
        statusCode: 200,
        body: JSON.stringify('Hello from Lambda!'),
    };
    
};
