# Digital-Speedometer
Arduino code for the digital speedometer project

I saw a speedometer mod for the 67-72 trucks made popular by ls1nova71 and brad_man_72 on this forum. They were swapping in digital a speedometer from a newer S10 and doing some graphics work to make the range right. I decided that I’m bad at printing graphics and making the hardware side work so I wanted to just make the existing gauge function without a spinning cable. My first attempt was to try using a speed controlled electric motor to ‘spin’ the speedometer input shaft at the right RPM to pretend to be the transmission. That test ended mostly in tears when I couldn’t get the electric motor to produce a consistent speed and it was also extremely noisy. I was ready to give up and get some Dakota digital cluster, but I was dead-set on getting this to work for $40 instead of $1400. Plus I wanted it to look as original as possible.

The idea was born to do something similar to the S10 speedo, but be able to control the exact position of the needle. I did some digging around online and found some guys creating aviation simulators using the X27 stepper motors to drive analog gauges using an Arduino. I also found some code samples of people reading vehicle speed via PWM VSS output with. I picked up a small digital screen to act as the odometer display vs trying again to make a motor spin the physical odometer. I already own a 3D printer so I used that to quickly make some of the components for holding everything, it could be done another way if you didn’t have one. EEPROM on the Arduino chip is used to store the odometer reading and pull it up each time without having to consume constant power.
I have a video of it working https://youtu.be/emJhqhSkUgA and some pictures of the process. I’ll need to follow up on this post with more information as I get the idea fully functional, I still need to get it in the truck and do a road test. Ideally I’d like to make this easy enough for someone to get the parts, put them together, and just make it work without having to get into coding or 3D modeling.

Here’s the material list I ended up with. I ended up shelling out $67, but that left me with extra parts for later projects.
Elegoo Arduino Nano
eBoot Buck voltage converter
Uctronics .96” OLED I2C
Cciyu? X27.168 stepper motor
Delphi 4way connector

I started out taking apart the c10 speedometer to see how much room I would have, I also learned a trick right off the bat to use a dinner fork for removing the dial. These factory gauges are like clockwork inside so you want to be as gentle on them as you can if you’re going to reuse anything. I went ahead and carefully removed the dial, face, odometer, all the gears, and the needle armature. The magnet at the rear spun by the cable was not as easy to remove and I decided to cut the outside end off and then pressed out the brass bushing that was inside. With all that out of the way I made a 3D print that would replace the part holding the gauge face and in doing so would also hold my screen and stepper motor. If you use the same components I did (OLED and Stepper) they should fit right in the model I designed. You’ll need to de-solder the pins from the OLED and replace them with wires right off the board due to clearance. The speedometer needle didn’t quite fit back on the X27 stepper shaft so I did print a new part of that too, alternatively you could drill it out with a 1mm drill bit.
With that all assembled I ran the wiring through the speedometer cable hole to the back of the cluster so it could connect up with the controller. I’ll provide a picture of the wiring diagram and a link here https://crcit.net/c/905a34e381de435d8df352c393757068. I then printed a box to hold the Arduino and buck board while sealing up the wiring with a lid. The connections were soldered to the pins of the Arduino, I’m not very good at soldering that kind of stuff, but it works just fine for now. I did also throw on a Deplhi weather pack connector so that the cluster is easier to remove later. The IO to the vehicle are ground, +12v, sensor ground, and VSS. In my case I’ll be connecting to the IGN circuit for power and then VSS output on my Holley Terminator X. I’ll run my sensor ground to where the engine harness is grounded just to be sure I’m getting the least possible resistance.

For the Arduino coding I went used the x25 stepper library from Guy Carpenter, SSD1306Ascii library from Bill Greiman, and a blog post from Mathew MCMillan about reading the VSS. Later on I switch to using the multiMap function from Rob Tillaart to handle the non-linear printout of the speedometer face.