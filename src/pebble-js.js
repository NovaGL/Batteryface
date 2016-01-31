Pebble.addEventListener("ready", function(e) {});

Pebble.addEventListener("showConfiguration",
  function(e) {
    Pebble.openURL("http://192.168.1.39");
  }
);
Pebble.addEventListener("webviewclosed",
  function(e) {
    var configuration = JSON.parse(e.response);
    console.log(e.response);
    Pebble.sendAppMessage(configuration);
  }
);
