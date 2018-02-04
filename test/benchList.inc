for i := 0 to SIZE - 1 do
  Data[i] := 0;
LastTime := Now;
for i := 0 to SIZE - 1 do
  Data[TestData[i]] := TestData[i];
LogTime(Title + '. Random write access');
Sum := 0;
LastTime := Now;
for i := 0 to SIZE - 1 do
  Sum := Sum + Data[i];
LogTime(Title + '. Sequential read access');
RandSeed := 111;
LastTime := Now;
for i := 0 to SIZE - 1 do
  Sum := Sum + Data[Random(Size)];
LogTime(Title + '. Random read access');
Log(Title + '. Control sum: ' + FloatToStr(Sum));
