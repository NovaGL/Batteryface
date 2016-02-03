Pebble.addEventListener("ready", function(e) {});

Pebble.addEventListener("showConfiguration",
  function(e) {
    Pebble.openURL("https://rawgit.com/NovaGL/Batteryface/master/config/index.html");
  }
);
Pebble.addEventListener("webviewclosed",
  function(e) {
    var configuration = JSON.parse(e.response);
    console.log(e.response);
    Pebble.sendAppMessage(configuration);
  }
);
