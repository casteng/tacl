Log('Benchmarking ' + Title);
Sum := 0;

SaveCurrentMs();
for i := 0 to SIZE-1 do
  Data[Keys[i]] := i;
LogTime(Title + '. Put');

SaveCurrentMs();
for i := 0 to SIZE-1 do
  Data[Keys[i]];
LogTime(Title + '. Sequential get');

RandSeed := 123;
SaveCurrentMs();
for i := 0 to SIZE-1 do
  Sum := Sum + Data[Keys[Random(Size)]];
LogTime(Title + '. Random get');
Log(Title + '. Control sum: ' + FloatToStr(Sum));

