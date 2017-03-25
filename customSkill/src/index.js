var config = require('./config');

// var deviceId = "220033000b47343432313031";
// var accessToken = "da51fb9df5c254672f0476e51eb0efc12c9f1009";
var deviceId = config.deviceId;
var accessToken = config.accessToken;


var skillName = "The Haus"
var invocationName = "The house";
/**
 * Particle.io cloud function
 * The Arduino sketch specifies the REST URI when it makes
 * the particle.function()
 * For example, particle.function("myFunction", myFunction)
 * would be specified as: var cloudName = "myFunction"
 * You can leave this "myFunction", or change it later
 * if you change the sketch code.
 */
var cloudName = "myFunction";

var APP_ID = undefined; //replace with "amzn1.echo-sdk-ams.app.[your-unique-value-here]";

/**
 * The AlexaSkill prototype and helper functions
 * Particle is a child of AlexaSkill.
 */
var http = require('https');
var AlexaSkill = require('./AlexaSkill');
var Particle = function () {
  AlexaSkill.call(this, APP_ID);
};

// Extend AlexaSkill
Particle.prototype = Object.create(AlexaSkill.prototype);
Particle.prototype.constructor = Particle;

//----------------------------- Session Started --------------------------------
Particle.prototype.eventHandlers.onSessionStarted = function (sessionStartedRequest, session) {
  console.log(invocationName + "onSessionStarted requestId: " + sessionStartedRequest.requestId+ ", sessionId: " + session.sessionId);
     // any initialization logic goes here
};

//----------------------------- On Launch event (no arguments passed)---------------------------
Particle.prototype.eventHandlers.onLaunch = function (launchRequest, session, response) {
  console.log(invocationName + " onLaunch requestId: " + launchRequest.requestId + ", sessionId: " + session.sessionId);
  var speechOutput = "Welcome to " + skillName + ", what would you like me to do?";
  var repromptText = "Are you still there?";
  response.ask(speechOutput, repromptText);
};

//-------------------------- Session ended event -----------------------------
Particle.prototype.eventHandlers.onSessionEnded = function (sessionEndedRequest, session) {
  console.log(skillName + " onSessionEnded requestId: " + sessionEndedRequest.requestId + ", sessionId: " + session.sessionId);
  // any cleanup logic goes here
};

//----------------------------- Intent Handlers --------------------------------
Particle.prototype.intentHandlers = {
  
  "onOffIntent": function(intent, session, response){
    var deviceSlot      = intent.slots.deviceName;
    var onOrOffSlot     = intent.slots.onOrOff;
    var brightCmdSlot   = intent.slots.brightnessCmd;

    console.log(deviceSlot + ", " + onOrOffSlot + ", " + brightCmdSlot);

    // if slot exists, write value to var. ** could be 'undefined'
    var deviceName = deviceSlot   ? intent.slots.deviceName.value   : "" ;
    var onOrOff    = onOrOffSlot  ? intent.slots.onOrOff.value      : "on";    // on by default
    var brightCmd  = brightCmdSlot? intent.slots.brightnessCmd.value: "100";   // default to 100%

    console.log("cmd: " + deviceName + " ,  " + onOrOff + " , " + brightCmd);

    var arduinoPin = ""; 
    switch(deviceName){
      case "kitchen lights":
        arduinoPin = "1";   
        break;
      case "living room lights":
        arduinoPin = "2";
        break;
    }

    if (brightCmd === undefined){                                 // brightnessCmd.value is empty
          if      (onOrOff == "on")    dimValue = "100"
          else if (onOrOff == "off")   dimValue = "0";
    }else{
      dimValue = brightCmd.toString();
    }

    var requestURI  = "/v1/devices/" + deviceId + "/myFunction";  // calls turnPinOnOff() on photon
    var postData    = "args=" + arduinoPin + "," + dimValue;      // 'args=1,255'

    //------------ Command particle to do appropiate action. Turn on/off or dim.
    if (brightCmd === undefined){
      makeParticleRequest(requestURI, postData, function(resp){
          var json = JSON.parse(resp);
          console.log("Particle replied with:" + json.return_value);
          var speechOutput = "Ok, turning the " + deviceName  + " " + onOrOff ;
          response.tellWithCard(speechOutput, invocationName, "Turned "+ deviceName + " " + onOrOff );
        });
    }
    else{     
      makeParticleRequest(requestURI, postData, function(resp){
          var json = JSON.parse(resp);
          console.log("Particle replied with:" + json.return_value);
          var speechOutput = "Ok, dimming " + deviceName + " to " + brightCmd + " percent" ;
          response.tellWithCard(speechOutput, invocationName, deviceName+ "is dimmed" );
        });
    }

  },

  "turnUpIntent": function(intent, session, response){
    var maximum = 3;
    var minimum = 1;
    
    var requestURI  = "/v1/devices/" + deviceId + "/globalParty";  // calls '/vi/devices/devId/globalPwmA1'
    var postData    = "args= ";    
      makeParticleRequest(requestURI, postData, function(resp){
          var json = JSON.parse(resp);
          console.log("Particle replied with:" + json.return_value);
          var speechOutput = " ";
          
          var randomnumber = Math.floor(Math.random() * (maximum - minimum + 1)) + minimum;
          switch(randomnumber){
            case 1:
              response.tell("You got it dude");
              break;
            case 2:
              response.tell("Let me warn the neighbors first");
              break;
            case 3:
              response.tell("That's a horrible idea. Let's do it!");
              break;
            default:
              response.tell("Cheers");
              break;
          };

        });
  },


  "ProfanityIntent": function(intent, session, response){
    var maximum = 3;
    var minimum = 1;


    var randomNum = Math.floor(Math.random() * (maximum - minimum + 1)) + minimum;
    switch(randomNum){
      case 1:
        response.tell("I can't help you if you curse at me");
        break;
      case 2:
        response.tell("If I had feelings I would feel insulted");
        break;
      case 3:
        response.tell("Do you kiss your mother with that mouth?");
        break;
      default:
        response.tell("You're about as useful as an ashtray on a motorcycle");
        break;
    }
    
  },

    "peopleIntent": function(intent, session, response){

    var personName = intent.slots.peopleNames.value;
    var maximum = 2;
    var minimum = 1;


    var randomNum = Math.floor(Math.random() * (maximum - minimum + 1)) + minimum;
    switch(randomNum){
      case 1:
        response.tell("I don't know " + personName +", but sounds like a terrible person");
        break;
      case 2:
        response.tell( personName + "? Sounds boring");
        break;
      default:
        response.tell("You're about as useful as an ashtray on a motorcycle");
        break;
    }
    
  },


  "AMAZON.HelpIntent": function (intent, session, response) {
    response.tell("Still working on a help menu. Ask me again later");
  },

};



// Create the handler that responds to the Alexa Request.
exports.handler = function (event, context) {
  var particleSkill = new Particle();
  particleSkill.execute(event, context);
};


function makeParticleRequest(requestURI, postData, callback){
 	var options = {
 		hostname: "api.particle.io",
 		port: 443,
 		path: requestURI,
 		method: 'POST',
 		headers: {
 			'Content-Type': 'application/x-www-form-urlencoded',
      'Authorization': 'Bearer ' + accessToken,
 			'Accept': '*.*'
 		}
 	};

 	var req = http.request(options, function(res) {
 		console.log('STATUS: ' + res.statusCode);
 		console.log('HEADERS: ' + JSON.stringify(res.headers));

 		var body = "";

 		res.setEncoding('utf8');
 		res.on('data', function (chunk) {
 			console.log('BODY: ' + chunk);
 			body += chunk;
 		});

 		res.on('end', function () {
       callback(body);
    });
 	});

 	req.on('error', function(e) {
 		console.log('problem with request: ' + e.message);
 	});

 	// write data to request body
 	req.write(postData.toString());
 	req.end();
}
