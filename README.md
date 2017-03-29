# alexaParticleLights
----
My original intent was to integrate a Particle Photon board with an Amazon Echo device to control some devices using voice command. In the process, I found projects by people who had done similar things (see references at the end) and they helped lead the way. This is my attemt at bringing together a few of the things I learned on the way.

![flowDiagram](/img/alexaLightsFlow.png)
First, the user issues an utterance to the Amazon Echo device. The device sends the audio to Amazon's Alexa Voice service which 'tokenizes' the speech into a json format. It then gets routed to the specified endpoint (our AWS Lambda Function) and passes the tokenized values into the function call to Particle's API. This triggers our Particle Photon device to change state and return an *int* to the Lambda Function. The Lambda Function finishes the request and sends a voice response back for the Amazon Echo device along with an optional reference card for the companion app.

----
#### Hardware and embedded code
Let's start at the last step of the process; the Particle Photon. I wanted to control an LED strip which needed a voltage higher than what the Particle Photon could provide so I got a [power supply](https://www.amazon.com/gp/product/B005T7NIAI/ref=oh_aui_detailpage_o06_s00?ie=UTF8&psc=1) that could provide 24V for the LED strip and 5V for for the board. To switch this larger load I used a [TIP120](https://www.adafruit.com/product/976) Darlington transistor and wired it to the board according to the schematics.
![fritz](/img/alexaLights_Fritz.jpg)
----
## References:

##### Custom Skill:
* [alexaParticleBridge](https://github.com/rlisle/alexaParticleBridge)
* [Particle_Alexa](https://github.com/krvarma/Particle_Alexa)

##### Smart Home Skill:
* [Steps to Create a Smart Home Skill](https://developer.amazon.com/public/solutions/alexa/alexa-skills-kit/docs/steps-to-create-a-smart-home-skill)
* [particle-alexa-smart-home-skill](https://github.com/krvarma/particle-alexa-smart-home-skill) 

##### Todo:
* [Push Notifications](https://forums.developer.amazon.com/questions/40799/workaround-for-push-notifications.html)