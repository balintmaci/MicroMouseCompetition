//Cascade Position
void stateC()
{
  CascadePos(param1, param2);
}
//Stop
void stateS()
{
  //SetMotorSpeed(0, 0);
  SetMotorPower(0, 0);
}
//Velocity control
void stateV()
{
  SetMotorSpeed(param1, param2);
}
//Testing program
void stateT()
{
  ResetAllStoredValues();
  state = 'W';
  param1 = 300;
  param2 = 2800;
}
//Rotating/Turning (pozitive means left)
void stateR()
{
  int leftS = -100;
  int rightS = 100;
  if (leftPos <= -param1)
    leftS = 0;
  if (rightPos >= param1)
    rightS = 0;
  SetMotorSpeed(leftS, rightS);
  if (leftPos <= -param1 && rightPos >= param1)
  {
    state = 'T';
  }
}
//delete values
void stateD()
{
  ResetAllStoredValues();
  state = 'S';
}
//Go until wall
void stateW()
{
  int de = (1650 - s4) >> 6;
  SetMotorSpeed(param1 - de, param1 + de);
  if (s2 < param2)
  {
    param1 = 30;
    ResetAllStoredValues();
    state = 'R';
  }
}

