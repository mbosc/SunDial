var keys = require('message_keys');

// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

var deltautc_h = 0;
var deltautc_m = 0;

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function (e) {
    console.log('AppMessage received!');
    if (e.payload.hasOwnProperty("DELTAUTC")) { //(e.payload["TIMEZONESET"]) {
      deltautc_h = parseInt(e.payload["DELTAUTC"].slice(start = 0, end = 3));
      deltautc_m = parseInt(e.payload["DELTAUTC"].slice(start = 0, end = 1) + e.payload["DELTAUTC"].slice(start = 3));
      console.log(e.payload["DELTAUTC"], e.payload["DELTAUTC"].slice(start = 0, end = 3), e.payload["DELTAUTC"].slice(start = 0, end = 1) + e.payload["DELTAUTC"].slice(start = 3));
      console.log('DELTAS', deltautc_h, deltautc_m);
    }
    getSuntimes();
  }
);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.setRequestHeader("Authorization", "Basic " + "Wm90aWNvOlpvdGljb0FNb2xsYTU=");
  xhr.send();
};



// var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
//   pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

var baseurl = 'http://boschini.casa:13922/';

var nodenames = [];

// -------------------------- SENDING SIMPLE LIST --------------------------------
// function sendNextItem(items, index) {
//   // Build message
//   var key = le_keys.NODECOM + index;
//   var dict = {};
//   dict[key] = items[index];

//   // Send the message
//   Pebble.sendAppMessage(dict, function() {
//     // Use success callback to increment index
//     index++;

//     if(index < items.length) {
//       // Send next item
//       sendNextItem(items, index);
//     } else {
//       console.log('Last item sent!');
//     }
//   }, function() {
//     console.log('Item transmission failed at index: ' + index);
//   });
// }
// function sendList(items) {
//   var index = 0;
//   sendNextItem(items, index);
// }
// -------------------------------------------------------------------------------


// -------------------------- SENDING DOUBLE LIST --------------------------------
// function sendDoubleList(items) {
//   // Transmit header

//   var dict = {};
//   // var index = 0;
//   var myslice = items.slice(start=0, end=2+items[1]);
//   dict[le_keys.HEADER] = myslice.toString();
//   // index++


//   Pebble.sendAppMessage(dict,
//     function(e) {
//       console.log('Big info sent to Pebble successfully!');
//       console.log(dict);
//     },
//     function(e) {
//       console.log('Error sending big info to Pebble!');
//     }
//   );
// }
// -------------------------------------------------------------------------------


function sendList(key, list, index) {
  if (index < list.length) {
    var dict2 = {};
    // console.log('sending ' +  (keys['NODENAMES'] + index));
    dict2[keys['NODENAMES'] + index] = list[index];
    Pebble.sendAppMessage(dict2,
      function (e) {
        // console.log('Body info ' + index + ' sent to Pebble successfully!');
        sendList(key, list, index + 1);
      },
      function (e) {
        console.log('Error sending Header info to Pebble!');
      }
    );
  } else {
    console.log('list ended');
    getStatus(nodenames);
  }
}

function getNames() {
  // Construct URL
  var url = baseurl + 'hello';

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET',
    function (responseText) {
      var ans = eval(responseText);
      nodenames = ans.slice(start = 1);
      console.log(ans);
      var dict = {
        'NODENUM': ans[0]
      }
      Pebble.sendAppMessage(dict,
        function (e) {
          // console.log('Header info sent to Pebble successfully!');
          sendList(keys.NODENAMES, ans.slice(start = 1), 0)

        },
        function (e) {
          console.log('Error sending Header info to Pebble!');
        }
      );
    }
  );
}


function getStatus(nodes) {
  if (nodes.length > 0) {
    // Construct URL
    var url = baseurl + 'node/' + nodes[0];

    // console.log('fetching details for ' + nodes[0]);

    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET',
      function (responseText) {
        var ans = eval(responseText);
        // console.log(ans);
        var dict = {
          'NODECOM': ans.toString()
        }
        Pebble.sendAppMessage(dict,
          function (e) {
            // console.log('Node info for ' + ans[1] + ' sent to Pebble successfully!');
            getStatus(nodes.slice(start = 1))
          },
          function (e) {
            console.log('Error sending Header info to Pebble!');
          }
        );
      }
    );
  } else {
    console.log('end of transmissions');
  }
}

function getServer(names) {
  // if (names)
  //   getNames();
  // else
  //   getStatus(nodenames);

}

// // Listen for when the watchface is opened
// Pebble.addEventListener('ready', 
//   function(e) {
//     console.log('PebbleKit JS ready!');

//     // Get the initial weather
//     getServer(true);
//   }
// );

var timeinfos;

function locationSuccess(pos) {
  // buildurl
  var url = "https://api.sunrise-sunset.org/json?lat=" + pos.coords.latitude + "&lng=" + pos.coords.longitude + "&date=today";
  // var url = "https://api.sunrise-sunset.org/json?lat=-37.8136&lng=144.9631&date=today";
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET',
    function (responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      if (json.status == 'OK') {
        var suntimes = json.results;
        timeinfos = new Object();
        var propValue;
        for (var propName in suntimes) {
          if (propName == "day_length")
            continue;
          propValue = suntimes[propName];
          var vec = propValue.split(':');
          var h = vec[0];
          var m = vec[1];
          var s = vec[2];
          h = parseInt(h);
          if (h == 12) {
            if (s[3] == 'A') {
              h -= 12;
            }
          } else {
            if (s[3] == 'P') {
              h += 12;
            }
          }
          h += deltautc_h;
          m = parseInt(m);
          m += deltautc_m;
          s = parseInt(s[0]);
          h = h % 24;
          if (s > 2)
            m += 1;
          if (m == 60) {
            h += 1;
            // overflow on hours until the next day is impossible enough
          }
          while (h >= 24)
            h -= 24;
          while (h < 0)
            h += 24;

          timeinfos[propName + '_h'] = h;
          timeinfos[propName + '_m'] = m;
          console.log(propValue, h, m, s);
        }

        for (var propName in timeinfos) {
          propValue = timeinfos[propName];
          console.log(propName, propValue);
        }

        var ikeys = [
          "sunrise_h",
          "sunrise_m",
          "sunset_h",
          "sunset_m",
          "civil_twilight_begin_h",
          "civil_twilight_begin_m",
          "civil_twilight_end_h",
          "civil_twilight_end_m",
          "nautical_twilight_begin_h",
          "nautical_twilight_begin_m",
          "nautical_twilight_end_h",
          "nautical_twilight_end_m",
          "astronomical_twilight_begin_h",
          "astronomical_twilight_begin_m",
          "astronomical_twilight_end_h",
          "astronomical_twilight_end_m",
          "solar_noon_h",
          "solar_noon_m"
        ]

        // Transmit this back to phone
        var dict2 = {};
        // var alldata = [];
        for (var k = 0; k < ikeys.length; k++) {
          // alldata.push(timeinfos[ikeys[k]]);
          dict2[keys['SUNTIMES'] + k] = timeinfos[ikeys[k]];
        }
        Pebble.sendAppMessage(dict2,
          function (e) {
            console.log('Sent successfully');
            // sendList(key, list, index+1);
          },
          function (e) {
            console.log('Error sending Header info to Pebble!');
          }
        );
      } else {
        console.log('Error in response: status ' + json.status);
      }

      // // Temperature in Kelvin requires adjustment
      // var temperature = Math.round(json.main.temp - 273.15);
      // console.log('Temperature is ' + temperature);

      // // Conditions
      // var conditions = json.weather[0].main;      
      // console.log('Conditions are ' + conditions);
    }
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getSuntimes() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    { timeout: 15000, maximumAge: 60000 }
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function (e) {
    console.log('PebbleKit JS ready!');
    Pebble.sendAppMessage({ 'JSREADY': 1 });
  }
);