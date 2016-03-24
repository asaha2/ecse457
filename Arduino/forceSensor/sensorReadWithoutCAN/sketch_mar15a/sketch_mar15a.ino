int analogPin = 3;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
int val = 0;
int minimal;
int maximal;
void setup()
{
  Serial.begin(9600);          //  setup serial
}

void loop()
{
  delay(1000);
  val = analogRead(analogPin);    // read the input pin
  Serial.println(val);             // debug value
}
