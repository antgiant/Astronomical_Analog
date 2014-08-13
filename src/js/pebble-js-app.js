var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
//  Pebble.openURL('http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/configurable.html');
	Pebble.openURL('data:text/html,'+config_page()+'<!--.html');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  console.log("Response = "+dumpObj(e, "e", "    ", 9));
  //var options = JSON.parse(decodeURIComponent(e.response));
  //console.log("Options = " + JSON.stringify(options));
});

function config_page() {
	return((function () {/*
<!DOCTYPE html>
<html>
  <head>
    <title>Astronomical Analog Configuration</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
  <body>
    <div data-role="page" id="main">
      <div data-role="header" class="jqm-header">
        <h1>Astronomical Analog Configuration</h1>
      </div>

      <div data-role="content">
        <form name="config">
          <div data-role="fieldcontain">
            <label for="show-seconds">Show Second Hand:</label>
            <select name="show-seconds" id="show-seconds" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div data-role="fieldcontain">
            <label for="show-date">Show Date:</label>
            <select name="show-date" id="show-date" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div data-role="fieldcontain">
            <label for="show-ring">Show Outer Ring:</label>
            <select name="show-ring" id="show-ring" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div data-role="fieldcontain">
            <label for="low-res-time">Minimal Updates (Hour hand only moves once an hour, etc.):</label>
            <select name="low-res-time" id="low-res-time" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div data-role="fieldcontain">
            <label for="inverted">Invert Color Scheme:</label>
            <select name="inverted" id="inverted" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div data-role="fieldcontain">
            <label for="east-to-west-orb-rotation">Reverse Oribit Direction (i.e. make it East to West instead of Clockwise.):</label>
            <select name="east-to-west-orb-rotation" id="east-to-west-orb-rotation" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div class="ui-body ui-body-b">
            <fieldset class="ui-grid-a">
                <div class="ui-block-b"><button type="submit" data-theme="a" id="b-submit" onclick="submitit()">Submit</button></div>
                <a href='javascript:(document.write("Hi");)'>Cancel</a>
            </fieldset>
          </div>
        </form>
		<a href="javascript:(function(){var a=window.open('about:blank').document;a.write('<!DOCTYPE html><html><head></head><body></body></html>');a.close();var b=a.body.appendChild(a.createElement('pre'));b.style.overflow='auto';b.style.whiteSpace='pre-wrap';b.appendChild(a.createTextNode(document.documentElement.innerHTML))})();">Test Link</a>
      </div>
    </div>
    <script>
      function saveOptions() {
        var options = {
          'show-seconds': document.forms[0].elements["show-seconds"].value,
          'show-date': document.forms[0].elements["show-date"].value,
          'show-ring': document.forms[0].elements["show-ring"].value,
          'low-res-time': document.forms[0].elements["low-res-time"].value,
          'inverteds': document.forms[0].elements["inverted"].value,
          'east-to-west-orb-rotation': document.forms[0].elements["east-to-west-orb-rotation"].value,
        }
        return options;
      }

      function cancel() {
        //return(window.location.href = "pebblejs://close");
		document.write("Hi")
		document.write(dumpObj(document, "Document", "    ", 9));
      }
      function submitit() {
        var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
   document.write("<pre>pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()))+"</pre>");
        document.write(location);
//        window.location.replace(location);
      }
      document.write(dumpObj(window, "Window", "    ", 9));
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
    </script>
  </body>
</html>
*/}).toString().match(/[^]*\/\*([^]*)\*\/\}$/)[1]);
} 
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