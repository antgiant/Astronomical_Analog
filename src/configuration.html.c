/*
<!DOCTYPE html>
<html>
  <head>
    <title>Astronomical Analog Configuration</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
    <link rel="stylesheet" href="//ajax.googleapis.com/ajax/libs/jquerymobile/1.4.3/jquery.mobile.min.css" />
    <script src="//ajax.googleapis.com/ajax/libs/jquerymobile/1.4.3/jquery.mobile.min.js"></script>
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
              <option value="on">On</option>
              <option value="off">Off</option>
            </select>
          </div>

          <div data-role="fieldcontain">
            <label for="show-ring">Show Outer Ring:</label>
            <select name="show-ring" id="show-ring" data-role="slider">
              <option value="on">On</option>
              <option value="off">Off</option>
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
            <label for="east-to-west-orb-rotation">Proper Orbit Direction (i.e. make it East to West instead of Clockwise.):</label>
            <select name="east-to-west-orb-rotation" id="east-to-west-orb-rotation" data-role="slider">
              <option value="off">Off</option>
              <option value="on">On</option>
            </select>
          </div>

          <div class="ui-body ui-body-b">
            <fieldset class="ui-grid-a">
              <div class="ui-block-a"><button type="submit" data-theme="d" id="b-cancel">Cancel</button></div>
              <div class="ui-block-b"><button type="submit" data-theme="a" id="b-submit">Submit</button></div>
            </fieldset>
          </div>
        </form>
      </div>
    </div>
    <script>
      function saveOptions() {
        var options = {
          'show-seconds': $("#show-seconds").val(),
          'show-date': $("#show-date").val(),
          'show-ring': $("#show-ring").val(),
          'low-res-time': $("#low-res-time").val(),
          'inverteds': $("#inverted").val(),
          'east-to-west-orb-rotation': $("#east-to-west-orb-rotation").val(),
//          'checkbox-sunchips': $("#checkbox-sunchips").is(':checked')
        }
        return options;
      }

      $().ready(function() {
        $("#b-cancel").click(function() {
          console.log("Cancel");
          document.location = "pebblejs://close";
        });

        $("#b-submit").click(function() {
          console.log("Submit");

          var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
          console.log("Warping to: " + location);
          console.log(location);
          document.location = location;
        });

        //Set Options to whatever is passed in.
		var obj = jQuery.parseJSON(decodeURIComponent(window.location.search.substring(window.location.search.substring(1).indexOf("?") + 2)));
		for( key in obj ) {
          $("#"+[key]).val(obj[key]).attr('selected',true);
          $("#"+[key]).val(obj[key]).slider("refresh");       
      });
    </script>
  </body>
</html>
*/