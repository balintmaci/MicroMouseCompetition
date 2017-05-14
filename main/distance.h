#define left 0
#define leftdi 1
#define front 2
#define rightdi 3
#define right 4
//Meredekség tömb
const int32_t m[5][16] = {
  {62500, 1592, 2283, 4032, 6410, 10000, 13888, 20000, 26315, 41666, 38461, 62500, 100000, 100000, 83333, 166666},
  {8771, 1602, 2590, 5208, 6329, 12195, 14285, 17857, 31250, 29411, 38461, 62500, 83333, 125000, 100000, 100000},
  {50000, 2127, 1893, 3546, 5747, 8196, 13157, 20833, 27777, 38461, 45454, 62500, 71428, 100000, 100000, 166666},
  {3571, 1706, 3125, 4807, 6410, 12820, 18518, 33333, 27777, 50000, 55555, 71428, 100000, 166666, 125000, 166666},
  {83333, 1666, 2127, 3875, 5263, 10638, 13157, 26315, 27777, 45454, 45454, 83333, 83333, 100000, 125000, 166666}
};

//Y offset tömb
const int32_t b[5][16] = {
  { -3437500, 399681, 139269, -903225, -2615384, -5480000, -8777777, -14180000, -19921052, -34166666, -31153846, -54062500, -90100000, -90100000, -73916666, -155333333},
  { -508771, 315705, -106217, -1729166, -2531645, -7195121, -8942857, -12053571, -24093750, -22411764, -30846153, -53562500, -73416666, -113375000, -89300000, -89300000},
  { -2900000, 355319, 426136, -510638, -2068965, -4016393, -8263157, -15125000, -21500000, -31500000, -38136363, -54500000, -63142857, -91000000, -91000000, -156666666},
  { -232142, 150170, -556250, -1663461, -2884615, -8269230, -13277777, -26700000, -21583333, -42450000, -47722222, -62928571, -90500000, -155166666, -114625000, -155333333},
  { -4916666, 391666, 223404, -825581, -1836842, -6265957, -8460526, -20421052, -21777777, -38500000, -38500000, -75166666, -75166666, -91500000, -116125000, -157333333}
};

//Threshold tömb
const int32_t thr[5][17] = {
  {55, 63, 377, 596, 720, 798, 848, 884, 909, 928, 940, 953, 961, 966, 971, 977, 980},
  {58, 115, 427, 620, 716, 795, 836, 871, 899, 915, 932, 945, 953, 959, 963, 968, 973},
  {58, 68, 303, 567, 708, 795, 856, 894, 918, 936, 949, 960, 968, 975, 980, 985, 988},
  {65, 205, 498, 658, 762, 840, 879, 906, 921, 939, 949, 958, 965, 970, 973, 977, 980},
  {59, 65, 365, 600, 729, 824, 871, 909, 928, 946, 957, 968, 974, 980, 985, 989, 992}
};

//Analog input pins
const int32_t inputs[5] = {A3, A6, A2, A1, A0};

//Read infra values, index: 0:left, 1:left-diagonal, 2:front, 3:right-diagonal, 4:right
void _readInfraPin(int8_t index)
{
  pastinfra[index] = infra[index];

  int16_t value = analogRead(inputs[index]);
  if (value > thr[index][16])
  {
    infra[index] = 999999;
  }
  else if (value < thr[index][0])
  {
    infra[index] = 0;
  }
  else
  {
    for (int8_t i = 15; i >= 0; i--)
    {
      if (value > thr[index][i])
      {
        infra[index] = (value * m[index][i] + b[index][i]) / 1000;
        break;
      }
    }
  }
  infra_deriv[index] = 1000 * (infra[index] - pastinfra[index]) / (int)micro;
}

void _readInfra(int8_t index = -1)
{
  if (index == -1)
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      _readInfraPin(i);
    }
  }
  else
  {
    _readInfraPin(index);
  }
}

uint8_t _infraCounter = 0;
void InfraISR()
{
  if (_infraCounter == 1)
  {
    digitalWrite(infraPin, 1);
  }
  if (_infraCounter == 2)
  {
    _readInfra();
    digitalWrite(infraPin, 0);
    micro = 0;
  }
  if (_infraCounter == 4)
  {
    _infraCounter = 0;
  }
  _infraCounter++;
}

#include <VL53L0X.h>
VL53L0X TOF;

//Function to setup TOF sensor for reading distance
void SetupTOF()
{
  TOF.init();
  TOF.setTimeout(50);
  TOF.setMeasurementTimingBudget(20000);
}

int32_t readTOF()
{
  int temp = TOF.readRangeSingleMillimeters();
  if (TOF.timeoutOccurred())
  {
    temp = 999999;
  }
  return temp;
}

