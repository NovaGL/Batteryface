Pebble.addEventListener("ready", function(e) {});

Pebble.addEventListener("showConfiguration",
  function(e) {
    Pebble.openURL("http://192.168.1.42/");
  }
);
Pebble.addEventListener("webviewclosed",
  function(e) {
    var configuration = JSON.parse(e.response);
    var backgroundColor = configuration.background_color;
    var dict = {};
        dict.KEY_COLOR_RED = parseInt(backgroundColor.substring(2, 4), 16);
        dict.KEY_COLOR_GREEN = parseInt(backgroundColor.substring(4, 6), 16);
        dict.KEY_COLOR_BLUE = parseInt(backgroundColor.substring(6), 16);
        dict.KEY_DATE_FORMAT = configuration.dateformat;
        dict.KEY_BT_FORMAT = configuration.bluetooth_toggle;
        dict.KEY_VIBE_ON_DISCONNECT = configuration.vibe_on_disconnect;
        dict.KEY_VIBE_ON_CONNECT = configuration.vibe_on_connect;
    
    console.log(e.response);
    //Pebble.sendAppMessage(configuration);
    
    // Send to watchapp
    Pebble.sendAppMessage(dict, function() {
        console.log('Send successful: ' + JSON.stringify(dict));
    }, function() {
        console.log('Send failed!');
    });
  }
);

