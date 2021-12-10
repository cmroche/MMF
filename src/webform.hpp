#pragma once

#include <Arduino.h>

const char *PARAM_FEED_STEPS = "feedSteps";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  Current Time: %systemTime%<br/><br/>
  <form action="/get" target="hidden-form">
    feedSteps (current value %feedSteps%): <input type="number" name="feedSteps" value="%feedSteps%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/feed" target="hidden-form">
    <input type="submit" value="Feed" onclick="submitMessage()">
  </form><br>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";
