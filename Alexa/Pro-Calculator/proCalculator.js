var https = require('https');
var Alexa = require('alexa-sdk');

exports.handler = (event, context) => {

  try {

    if (event.session.new) {
      // New Session
      console.log("NEW SESSION");
    }

    switch (event.request.type) {

      case "LaunchRequest":
        // Launch Request
        console.log(`LAUNCH REQUEST`);
        context.succeed(
          generateResponse(
            buildSpeechletResponse("Launch Request", "Welcome to Pro Calculator", "", true),
            {}
          )
        )
        break;

      case "IntentRequest":
        // Intent Request
        console.log(`INTENT REQUEST`);
				onIntent(event.request,
					event.session,
				  function callback(sessionAttributes, speechletResponse){
						context.succeed(generateResponse(sessionAttributes, speechletResponse));
					});
        break;

      case "SessionEndedRequest":
        // Session Ended Request
        console.log(`SESSION ENDED REQUEST`);
        break;

      default:
        context.fail(`INVALID REQUEST TYPE: ${event.request.type}`)

    }

  } catch(error) { context.fail(`Exception: ${error}`) }

}


  // Called when the user specifies an intent for this skill.
function onIntent(intentRequest, session, callback) {
    console.log("onIntent requestId=" + intentRequest.requestId
        + ", sessionId=" + session.sessionId);

		var cardTitle = "Addition";

    var intent = intentRequest.intent,
        intentName = intentRequest.intent.name;

    // dispatch custom intents to handlers here
		switch(intentName){

			//addition
			case "addIntent":

			  var valA = intent.getSlots().get(valA).getValue();
				var valB = intent.getSlots().get(valB).getValue();
				var ans = valA + valB;
			  callback(session.attributes,
				  buildSpeechletResponse(cardTitle, `The answer is ${ans}`, "", "true"));
				break;

			default:
			  throw "Invalid intent";
		}

}

// Helpers
buildSpeechletResponse = (title, outputText, repromptText, shouldEndSession) => {

  return {
    outputSpeech: {
      type: "PlainText",
      text: outputText
    },
		card: {
			type: "Simple",
			title: title,
			content: outputText
		},
		reprompt: {
				outputSpeech: {
						type: "PlainText",
						text: repromptText
				}
		},
    shouldEndSession: shouldEndSession
  }

}

generateResponse = (speechletResponse, sessionAttributes) => {

  return {
    version: "1.0",
    sessionAttributes: sessionAttributes,
    response: speechletResponse
  }

}
