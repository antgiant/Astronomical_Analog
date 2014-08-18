var initialized = false;
var options = "";

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
//  Pebble.openURL('http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/configurable.html');
  console.log('http://htmlpreview.github.io/?https://github.com/antgiant/Astronomical_Analog/blob/SDK-2/src/configuration.html.c?'+encodeURIComponent(JSON.stringify(options)));
  Pebble.openURL('http://htmlpreview.github.io/?https://github.com/antgiant/Astronomical_Analog/blob/SDK-2/src/configuration.html.c?'+encodeURIComponent(JSON.stringify(options)));
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  //console.log("Response = "+dumpObj(e, "e", "    ", 9));
  options = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(options));
});

function dumpObj(obj, name, indent, depth) {
  var MAX_DUMP_DEPTH = 10;
  if (depth > MAX_DUMP_DEPTH) {
    return indent + name + ": <Maximum Depth Reached>\n";
  }
  if (typeof obj == "object") {
    var child = null;
    var output = indent + name + "\n";
    indent += "\t";
    for (var item in obj)
    {
      try {
        child = obj[item];
      } catch (e) {
        child = "<Unable to Evaluate>";
      }
      if (typeof child == "object") {
        output += dumpObj(child, item, indent, depth + 1);
      } else {
        output += indent + item + ": " + child + "\n";
      }
    }
    return output;
  } else {
    return obj;
  }
}