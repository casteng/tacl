{
@abstract(Template collections benchmark)

This is a benchmark for Template Collections and Algorithms Library

@author(George Bakhtadze (avagames@gmail.com))
}

program BenchList;
{$MODE Delphi}
{$APPTYPE CONSOLE}

uses
  SysUtils,
  Classes,
  DateUtils,
  {$IFDEF FPC}FGL{$ELSE}System.Generics.Collections{$ENDIF},
  Math;

const
  SIZE = 10000000;

type
  TValueType = Double;
  PValueType = ^TValueType;
  TTestGenericList =
    {$IFDEF FPC}
      TFPGList<PValueType>
    {$ELSE}
    TList<PTestedValueType>
    {$ENDIF};
  TStaticArray = array [0..SIZE - 1] of TValueType;
  TDynamicArray = array of TValueType;

  _VectorValueType = TValueType;
    {$MESSAGE 'Instantiating TTemplateVector interface'}
    {$I tpl_coll_vector.inc}
  TTemplateVector = _GenVector;

var
  LastTime: TDateTime;
  TestData: array [0..SIZE - 1] of integer;
  StaticArray: TStaticArray;
  ValuePtr: PValueType;
  Sum: TValueType;
  i: integer;

{$MESSAGE 'Instantiating TIntegerVector'}
{$I tpl_coll_vector.inc}

procedure Log(const msg: string);
begin
  WriteLn(msg);
  Flush(Output);
end;

procedure LogTime(const msg: String);
begin
  Log(Format(msg + ': %2.5f', [DateUtils.MilliSecondSpan(LastTime, Now())]));
end;

procedure BenchStaticArray(const Title: String; var Data: TStaticArray);
begin
  {$I benchList.inc}
end;

procedure BenchDynamicArray(const Title: String);
var
  Data: TDynamicArray;
begin
  SetLength(Data, SIZE);
  {$I benchList.inc}
  SetLength(Data, 0);
end;

procedure BenchTemplateVector(const Title: String);
var
  Data: TTemplateVector;
begin
  Data := TTemplateVector.Create();
  Data.Count := SIZE;
  {$I benchList.inc}
  FreeAndNil(Data);
end;

procedure BenchTList(const Title: String);
var
  TestList: TList;
begin
  TestList := TList.Create;
  TestList.Capacity := SIZE;
  for i := 0 to SIZE - 1 do
  begin
    new(ValuePtr);
    ValuePtr^ := 0;
    TestList.Add(ValuePtr);
  end;
  LastTime := Now;
  for i := 0 to SIZE - 1 do
    PValueType(TestList[TestData[i]])^ := TestData[i];
  LogTime(Title + '. Random write access');
  Sum := 0;
  LastTime := Now;
  for i := 0 to SIZE - 1 do
    Sum := Sum + PValueType(TestList[i])^;
  LogTime(Title + '. Sequential access');
  RandSeed := 111;
  LastTime := Now;
  for i := 0 to SIZE - 1 do
    Sum := Sum + PValueType(TestList[Random(Size)])^;
  LogTime(Title + '. Random read access');
  Log(Title + '. Control sum: ' + FloatToStr(Sum));
  for i := 0 to SIZE - 1 do
    dispose(PValueType(TestList[i]));
  TestList.Free;
end;

procedure BenchGenericList(const Title: String);
var
  GenericList: TTestGenericList;
begin
  GenericList := TTestGenericList.Create;
  GenericList.Capacity := SIZE;
  for i := 0 to SIZE - 1 do
  begin
    new(ValuePtr);
    ValuePtr^ := 0;
    GenericList.Add(ValuePtr);
  end;
  LastTime := Now;
  for i := 0 to SIZE - 1 do
    PValueType(GenericList[TestData[i]])^ := TestData[i];
  LogTime(Title + '. Random write access');
  Sum := 0;
  LastTime := Now;
  for i := 0 to SIZE - 1 do
    Sum := Sum + PValueType(GenericList[i])^;
  LogTime(Title + '. Sequential access');
  RandSeed := 111;
  LastTime := Now;
  for i := 0 to SIZE - 1 do
    Sum := Sum + PValueType(GenericList[Random(Size)])^;
  LogTime(Title + '. Random read access');
  Log(Title + '. Control sum: ' + FloatToStr(Sum));
  for i := 0 to SIZE - 1 do
    dispose(PValueType(GenericList[i]));
  GenericList.Free;
end;

begin
  Randomize;
  Log('Prepare test data...');
  for i := 0 to SIZE - 1 do
    TestData[i] := Math.RandomRange(0, SIZE);

  BenchStaticArray('Static array', StaticArray);
  BenchTemplateVector('Template vector');
  BenchDynamicArray('Dynamic array');
  BenchTList('TList');
  BenchGenericList('Generic List');
end.