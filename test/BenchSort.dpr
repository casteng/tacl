{
@abstract(Template collections benchmark)

This is a benchmark for Sort algorithm in Template Collections and Algorithms Library

@author(George Bakhtadze (avagames@gmail.com))
}

program BenchSort;
{$IFDEF FPC}
    {$DEFINE HAS_GENERICS}
    {$MODE Delphi}
{$ELSE}
    {$IFDEF VER170}
    {$DEFINE HAS_INLINE}
    {$DEFINE HAS_STRICT}
    {$ENDIF}
    {$IFDEF VER200}
    {$DEFINE HAS_GENERICS}
    {$ENDIF}
{$ENDIF}
{$APPTYPE CONSOLE}

uses
  {$IFDEF FPC}
    {$IFDEF UNIX}
      unix,
    {$ENDIF}
    garrayutils,
  {$ELSE}
    Windows,
    {$IFDEF HAS_GENERICS}
      System.Generics.Collections,
    {$ENDIF}
  {$ENDIF}
  template,
  SysUtils;

const
  SIZE = 1000*1000*10;
  KEY_SIZE = 32;

type
  TValueType = record
    Key: int32;
    Value: Single;
  end;
  TValueArray = array[0..SIZE - 1] of TValueType;

var
  LastTime: Int64;
  Data: ^TValueArray;

procedure Log(const msg: string);
begin
  WriteLn(msg);
  Flush(Output);
end;

{$IFDEF FPC}
function GetCurrentMs: Int64;
var tm: TimeVal;
begin
  fpGetTimeOfDay(@tm, nil);
  Result := tm.tv_sec * Int64(1000) + tm.tv_usec div 1000;
end;
{$ELSE}
function GetCurrentMs: Int64;
begin
    Result := GetTickCount();
end;
{$ENDIF}

procedure SaveCurrentMs();
begin
  LastTime := GetCurrentMs();
end;

function CalcMsPassed(): Int64;
begin
  Result := GetCurrentMs() - LastTime;
end;

procedure LogTime(const msg: String);
begin
  Log(Format(msg + ': %d ms', [CalcMsPassed()]));
end;

procedure GenData();
var i: Integer;
begin
  RandSeed := 213;
  Getmem(Data, SIZE * SizeOf(TValueType));
  for i := 0 to SIZE-1 do
  begin
    Data[i].Key := Random(SIZE);
    Data[i].Value := i;// * 222.0015;
  end;
end;

procedure CheckSorted(const Title: String);
var i: Integer;
begin
  i := SIZE - 2;
  while (i >= 0) and (Data[i].Key <= Data[i + 1].Key) do
    Dec(i);
  if i < 0 then
    Log(Title + ': sorted')
  else
    Log(Title + ': not sorted');
end;

function _SortGetValue(const V: TValueType): Integer; {$I inline.inc}
begin
  Result := V.Key;
end;

{function _SortCompare(const V1,V2: TValueType): Integer; (*$I inline.inc*)
begin
  Result := V1.Key - V2.Key;
end;}

procedure TemplateSort(_SortCount: Integer; var _SortData: TValueArray);
type
  _SortDataType = TValueType;
//  _SortValueType = Integer;
//const _SortOptions = [soBadData];
  {$I tpl_algo_quicksort.inc}
{$IFDEF _IDE_PARSER_}
begin
{$ENDIF}
end;

function BenchTemplateSort(const Title: String): TValueType;
begin
  GenData();
  CheckSorted(Title);
  SaveCurrentMs();
  TemplateSort(SIZE, Data^);
  LogTime(Title + '. Sort');
  CheckSorted(Title);
  FreeMem(Data);
end;

{$IFDEF HAS_GENERICS}
type
  TGHashMapHasher = class
  public
    class function c(const a, b: TValueType): Boolean; {$I inline.inc}
  end;
  TGArrayUtil = TOrderingArrayUtils<TValueArray, TValueType, TGHashMapHasher>;

function BenchGArraySort(const Title: String): TValueType;
var
  Util: TGArrayUtil;
  i: Integer;
begin
  GenData();
  CheckSorted(Title);
  SaveCurrentMs();
  Util := TGArrayUtil.Create();
  Util.Sort(Data^, SIZE);
  LogTime(Title + '. Sort');
  CheckSorted(Title);
  Util.Destroy;
  FreeMem(Data);
end;

{ TGHashMapHasher }

class function TGHashMapHasher.c(const a, b: TValueType): Boolean; {$I inline.inc}
begin
  Result := a.Key < b.Key;
end;
{$ENDIF HAS_GENERICS}

var
  i: Integer;

begin
  Randomize;
  Log('Prepare test data...');

  {$IFDEF HAS_GENERICS}
  BenchGArraySort('GArray sort');
  {$ENDIF}
  BenchTemplateSort('Template sort');
  Log('Press ENTER...');
  Readln;
end.
