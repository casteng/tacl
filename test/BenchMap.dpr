{
@abstract(Template collections benchmark)

This is a benchmark for Map in Template Collections and Algorithms Library

@author(George Bakhtadze (avagames@gmail.com))
}

program BenchMap;
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
    ghashmap,
  {$ELSE}
    Windows,
    {$IFDEF HAS_GENERICS}
      System.Generics.Collections,
    {$ENDIF}
  {$ENDIF}
  SysUtils;

const
  SIZE = 1000 * 1000;
  KEY_SIZE = 32;

type
  TKeyType = AnsiString;
  TValueType = Integer;
  PValueType = ^TValueType;

    _HashMapKeyType = TKeyType;
  _HashMapValueType = TValueType;
    {$MESSAGE 'Instantiating TTemplateMap interface'}
    {$I tpl_coll_hashmap.inc}
  TTemplateMap = _GenHashMap;

var
  LastTime: Int64;
  Keys: array [0..SIZE - 1] of TKeyType;
  Sum: TValueType;

{$MESSAGE 'Instantiating TTemplateMap'}
{$I tpl_coll_hashmap.inc}

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

function GenKey(): TKeyType;
var i: Integer;
begin
  SetLength(Result, KEY_SIZE);
  for i := 1 to KEY_SIZE do
    Result[i] := Chr(Ord('0') + Random(Ord('z') - Ord('0')));
end;

procedure BenchTemplateMap(const Title: String);
var
  Data: TTemplateMap;
  Iterator: _GenHashMapIterator;
  i: Integer;
begin
  Data := TTemplateMap.Create();

  {$I benchMap.inc}
  Sum := 0;
  SaveCurrentMs();
  Iterator := Data.GetIterator();
  while Iterator.GotoNext do
    Sum := Sum + Iterator.CurrentValue;
  LogTime(Title + '. Iteration');
  Log(Title + '. Control sum: ' + FloatToStr(Sum));

  FreeAndNil(Data);
end;

{$IFDEF HAS_GENERICS}
type
  TGHashMapHasher = class
  public
    class function hash(Key: TKeyType; Size: SizeUInt): SizeUInt;
  end;
  TTestGenericMap = THashMap<TKeyType, TValueType, TGHashMapHasher>;
  
procedure BenchGHashMap(const Title: String);
var
  Data: TTestGenericMap;
  Iterator: TTestGenericMap.TIterator;
  i: Integer;
begin
  Data := TTestGenericMap.Create();

  {$I benchMap.inc}
  Sum := 0;
  SaveCurrentMs();
  Iterator := data.Iterator;
  repeat
    Sum := Sum + Iterator.Value;
  until not iterator.Next;
  LogTime(Title + '. Iteration');
  Log(Title + '. Control sum: ' + FloatToStr(Sum));

  iterator.Destroy;
  FreeAndNil(Data);
end;

{ TGHashMapHasher }

class function TGHashMapHasher.hash(Key: TKeyType; Size: SizeUInt): SizeUInt;
var i: Integer;
begin
  {$Q-}
  Result := 5381;
  for i := 1 to Length(Key) do
    Result := 33 * Result + Ord(Key[i]);
  Result := Result mod Size;
end;
{$ENDIF HAS_GENERICS}

var
  i: Integer;

begin
  Randomize;
  Log('Prepare test data...');
  RandSeed := 111;
  for i := 0 to SIZE - 1 do
    Keys[i] := GenKey();

  BenchTemplateMap('Template map');
  {$IFDEF HAS_GENERICS}
  BenchGHashMap('GHashMap');
  {$ENDIF}
  Log('Press ENTER...');
  Readln;
end.
