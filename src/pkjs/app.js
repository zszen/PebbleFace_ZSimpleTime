console.log=function(strOutput){};

var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

// var config_isHighConstract = false;
// var config_duringLightTime = 0;
// var config_isColorful = true;

// Pebble.addEventListener('ready', 
// 	function(e) {
// 		console.log("PebbleKit JS ready!");
// 		var dataSend = {"TYPE":"ready"};
// 		// Send to Pebble
// 		Pebble.sendAppMessage(dataSend,
// 		function(e) {
// 			console.log("Weather info sent to Pebble successfully!");
//         },
//         function(e) {
// 			console.log("Error sending weather info to Pebble!");
//         });
// 	}
// );

// Pebble.addEventListener('appmessage',
// 	function(e)   {
// 		console.log("AppMessage received!");
// 		config_isHighConstract = e.payload.CONF_highConstract;//["TOJS_CITYNAME"];
// 		config_duringLightTime = e.payload.CONF_duringTime;//["TOJS_FREQUENCE"];
// 		config_isColorful = e.payload.CONF_colorful;//["TOJS_SMART"];
// 		var config_type = e.payload.TYPE;
// 		console.log("AppMessage init_list:"+config_type+config_isHighConstract+config_duringLightTime+config_isColorful);
// 	}                 
// );

// Pebble.addEventListener('showConfiguration', function() {
// 	var url = 'http://23.105.195.65/pebble/time_config.html?isHighConstract='+config_isHighConstract+"&duringLightTime="+config_duringLightTime+"&isColorful="+config_isColorful;
// 	console.log("config url: "+url);
// 	Pebble.openURL(url);
// });

// Pebble.addEventListener('webviewclosed', function(e) {
// 	// Decode the user's preferences
// 	if(!e.response)return;
// 	var configData = JSON.parse(decodeURIComponent(e.response));
// 	// Send to the watchapp via AppMessage
// 	config_isHighConstract = parseInt(configData.CONFIG_isHighConstract);
// 	config_duringLightTime = parseInt(configData.CONFIG_duringTime);
// 	config_isColorful = parseInt(configData.CONFIG_isColorful);
// 	console.log("AppMessage back_list:"+config_isHighConstract+config_duringLightTime+config_isColorful+configData.CONFIG_duringTime);
// 	var dict = {"TYPE":"config",
// 				"CONF_highConstract":config_isHighConstract,
// 				"CONF_duringTime":config_duringLightTime,
// 				"CONF_colorful":config_isColorful};
// 	// Send to the watchapp
// 	Pebble.sendAppMessage(dict, function() {
// 		console.log('Config data sent successfully!');
// 	}, function(e) {
// 		console.log('Error sending config data!');
// 	});
// });
												
// Pebble.addEventListener('webviewclosed', function(e) {
// 	if (e && !e.response) {
// 		return;
// 	}

// 	// Get the keys and values from each config item
// 	console.log(e.response);
// 	var dict = clay.getSettings(e.response);
// 	dict.TYPE = "config";
// 	// Send settings values to watch side
// 	Pebble.sendAppMessage(dict, function(e) {
// 		console.log('Sent config data to Pebble');
// 	}, function(e) {
// 		console.log('Failed to send config data!');
// 		console.log(JSON.stringify(e));
// 	});
// });										
												