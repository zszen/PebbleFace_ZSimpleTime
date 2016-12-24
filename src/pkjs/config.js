//z simple time 
module.exports = 
[
	{//info
		"type": "heading", 
		"defaultValue": "Zszen Simple Weather Setting" 
	}, 
	{
		"type": "section",
		"items": [
			{//
				"type": "text",
				"defaultValue": "Donation zszen@hotmail.com(Alipay) or zszen.john@gmail.com(Paypal)"
			},
			{//Animation Turning 
				"type": "heading", 
				"defaultValue": "Animation Turning" 
			},
			{
				"type": "toggle",
				"messageKey": "CONFIG_isAnimationTurning",
				"label": "enable animation turning",
				"defaultValue": true
			},
			{
				"type": "text", 
				"defaultValue": "On with animation when start or hour change. Off just save power." 
			},
			{//High Constract 
				"type": "heading", 
				"defaultValue": "High Constract" 
			},
			{
				"type": "toggle",
				"messageKey": "CONFIG_isHighConstract",
				"label": "enable high constract",
				"defaultValue": false
			},
			{
				"type": "text", 
				"defaultValue": "Enable this will update watchface when you shack your watch. If you wanna to save your watch power. Turn it off." 
			},
			{//During Light Time 
				"type": "heading", 
				"defaultValue": "During Light Time" 
			},
			{
				"type": "select",
				"messageKey": "CONFIG_duringTime",
				"defaultValue": "0",
				"label": "During Light Time",
				"options": [
					{ 
						"label": "3 SEC",
						"value": "0" 
					},
					{ 
						"label": "5 SEC",
						"value": "1" 
					},
					{ 
						"label": "8 SEC",
						"value": "2" 
					},
					{ 
						"label": "15 SEC",
						"value": "3" 
					},
					{ 
						"label": "30 SEC",
						"value": "4" 
					},
					{ 
						"label": "1 MIN",
						"value": "5" 
					},
					{ 
						"label": "2 MIN",
						"value": "6" 
					},
					{ 
						"label": "5 MIN",
						"value": "7" 
					},
					{ 
						"label": "10 MIN",
						"value": "8" 
					},
					{ 
						"label": "30 MIN",
						"value": "9" 
					},
					{ 
						"label": "1 HOUR",
						"value": "10" 
					},
					{ 
						"label": "ALWAYS",
						"value": "11" 
					},
					{ 
						"label": "SHAKE TURN",
						"value": "12" 
					}
				]
			},
			{
				"type": "text", 
				"defaultValue": "AUTO: frequence changed with different weather: Just like 'sunny day' frequence is 2hour, cloudy frequence is 30min, rain/thunder is 15min. NEVER: don't frequence any time'" 
			},
			{//Colorful High Constract
				"type": "heading", 
				"defaultValue": "Colorful High Constract" 
			},
			{ 
				"type": "toggle",
				"messageKey": "CONFIG_isColorful",
				"label": "enable colorful",
				"defaultValue": true
			},
			{
				"type": "text", 
				"defaultValue": "If enable high constract. Enable this will show different color, disable will only show white background color." 
			},
			{//submit button
				"type": "submit",
				"defaultValue": "Save"
			}
		]
	}
];