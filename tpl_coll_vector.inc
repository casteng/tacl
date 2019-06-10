(*
  Template vector collection
  (C) George Bakhtadze
Usage:
  uses template;
  type
    _VectorValueType = <some type>;
    // Optional search type if Find() method is needed
    [_VectorSearchType = <some type>;]
    {$MESSAGE 'Instantiating TIntegerVector interface'}
    {$I tpl_coll_vector.inc}

    T<xxx>Vector = _GenVector;

  implementation

    // Optional equals function:
    [function _VectorEquals(const v1, v2: _VectorValueType): Boolean;
    begin
      Result := v1 = v2;
    end;]

    // Search function if search type was specified:
    [function _VectorFound(const v: _VectorValueType; const Pattern: _VectorSearchType): Boolean;
    begin
      Result := ...
    end;]

    // a compiler message with explanation which type is being instantiated would be very helpful:
    {$MESSAGE 'Instantiating TIntegerVector'}
    {$I tpl_coll_vector.inc}

    Some options can be specified in _VectorOptions. See template.TDataStructureOption documentaion.
*)

{$IFNDEF _VECTORIMPL}                  // Interface

{$IFDEF _IDE_PARSER_}
unit tpl_coll_vector;
interface
uses template;
type
{$ENDIF}

  {$IF Declared(_VectorOptions)}
      {$I tpl_coll_vector_opt.inc}
  {$IFEND}

  {$IF not Declared(__CollectionIndexType)}
  __CollectionIndexType = Integer;
  {$IFEND}

  {$I _T.inc} = _VectorValueType;
  {$IF not Declared(_PVectorValueType)}
    _PVectorValueType = ^_VectorValueType;
  {$IFEND}
  _VectorValueArray = array[0..0] of _VectorValueType;
  _PVectorValuesType = ^_VectorValueArray;
  // Vector for each delegate
  _VectorDelegate = function(const e: _VectorValueType; Data: Pointer): Boolean of object;
  {$I _Dg.inc} = _VectorDelegate;
  // Vector for each delegate
  _VectorCallback = function(const e: _VectorValueType; Data: Pointer): Boolean;
  {$I _Cb.inc} = _VectorCallback;
  _IGenVector = interface
    {$IFDEF TPL_HINTS}{$MESSAGE 'Instantiating _IGenVector interface for a template vector type'}{$ENDIF}
    {$I tpl_coll_list.inc}
  end;
  // Advance template type instance counter to reuse it in other template collection classes
  {$DEFINE _GEN_CNT_ADV}
  {$I _T.inc}

  // Generic vector template. Unit which use this inclde should use template unit also.
  _GenVector = class {$IF Declared(TTemplateInterface)}(TTemplateInterface, _IGenVector){$IFEND}
  {$IFDEF HAS_STRICT}strict{$ENDIF} protected
    // Values container
    FValues: _PVectorValuesType;
    // Current capacity of the vector
    FCapacity,
    // Number of entries
    FSize: __CollectionIndexType;
    procedure SetValue(const Index: __CollectionIndexType; const e: _VectorValueType); {$I inline.inc}
    procedure SetCapacity(const ACapacity: __CollectionIndexType);
  public
    constructor Create(); overload;
    constructor Create(const Capacity: __CollectionIndexType); overload;
    constructor Create(const arr: array of _VectorValueType); overload;
    destructor Destroy(); override;

    // Increases the capacity of the list to ensure that it can hold at least the number of elements specified
    procedure EnsureCapacity(const ASize: __CollectionIndexType); {$I inline.inc}

    { Collection interface }

    // Returns the number of elements in the collection
    function GetSize(): __CollectionIndexType; {$I inline.inc}
    // Sets the number of elements in the collection
    procedure SetSize(const ASize: __CollectionIndexType);
    // Returns True if the collection contains no elements
    function IsEmpty(): Boolean; {$I inline.inc}
    // Returns True if the collection contains the specified element
    function Contains(const e: _VectorValueType): Boolean;
    // Calls the delegate for each element in the collection
    procedure ForEach(Delegate: _VectorDelegate; Data: Pointer); overload;
    // Calls the delegate for each element in the collection
    procedure ForEach(Callback: _VectorCallback; Data: Pointer); overload;
    {$IF Declared(_VectorSearchType)}
    // Searches for element which satisfies the condition _VectorFound(element, Pattern) and returns its index or -1 if no such element.
    function Find(const Pattern: _VectorSearchType): __CollectionIndexType;
    // Searches for element which satisfies the condition _VectorFound(element, Pattern) starting from last one and returns its index or -1 if no such element.
    function FindLast(const Pattern: _VectorSearchType): __CollectionIndexType;
    {$IFEND}
    // Appends the element as the last element of the vector and returns True
    function Add(const e: _VectorValueType): Boolean; {$I inline.inc}
    {/ Removes the specified element from the collection.
       Returns True if the collection contained the element./}
    function Remove(const e: _VectorValueType): Boolean;
    // Removes all elements from the collection
    procedure Clear(); {$I inline.inc}
    // Number of elements
    property Size: __CollectionIndexType read FSize write SetSize;

    { List interface }

    {/ Returns the element at the specified position in the list.
       Throws an error on invalid index if dsRangeCheck was included in the list options before instantiation. }
    function Get(const Index: __CollectionIndexType): _VectorValueType; {$I inline.inc}
    {/ Returns the address of the element at the specified position in the list.
       Throws an error on invalid index if dsRangeCheck was included in the list options before instantiation. }
    function GetPtr(const Index: __CollectionIndexType): _PVectorValueType; {$I inline.inc}
    {/ Replaces the element at the specified position in the list with the specified element.
       Returns the element previously at the specified position.
       Throws an error on invalid index if dsRangeCheck was included in the list options when instantiation. }
    function Put(const Index: __CollectionIndexType; const e: _VectorValueType): _VectorValueType; {$I inline.inc}
    {/ Inserts the element at the specified position in the list shifting the element currently at that
       position (if any) and any subsequent elements to the right.
       Throws an error on invalid index if dsRangeCheck was included in the list options when instantiation. }
    procedure Insert(const Index: __CollectionIndexType; const e: _VectorValueType);
    {/ Removes the element at the specified position in the list shifting any subsequent elements
       to the left.
       Returns the element that was removed from the list. }
    function RemoveBy(const Index: __CollectionIndexType): _VectorValueType; {$I inline.inc}
    {/ Returns the index of the first occurrence of the specified element in the list,
       or -1 if the list does not contain the element. }
    function IndexOf(const e: _VectorValueType): __CollectionIndexType; {$I inline.inc}
    {/ Returns the index of the last occurrence of the specified element in the list,
       or -1 if the list does not contain the element. }
    function LastIndexOf(const e: _VectorValueType): __CollectionIndexType; {$I inline.inc}
    // Values retrieved by index
    property Values[const Index: __CollectionIndexType]: _VectorValueType read Get write SetValue; default;

    // Pointer to values retrieved by index
    property ValuesPtr[const Index: __CollectionIndexType]: _PVectorValueType read GetPtr;

    // Number of elements which the collection able to hold without memory allocations
    property Capacity: __CollectionIndexType read FCapacity write SetCapacity;

  end;

  {$DEFINE _VECTORIMPL}

{$IFDEF _IDE_PARSER_}{$DEFINE _IDE_DISABLE_CONDITIONALS_}{$ENDIF}
{$ELSE _VECTORIMPL}
{$IFDEF _IDE_PARSER_}{$UNDEF _IDE_DISABLE_CONDITIONALS_}{$ENDIF}

{$IFDEF _IDE_PARSER_}
implementation
{$ENDIF _IDE_PARSER_}

{$Q-}
{$R-}
  {$IF not Declared(_VectorDefaultCapacity)}
  const _VectorDefaultCapacity = 16;
  {$IFEND}
  {$IF not Declared(_VectorCapacityStep)}
  const _VectorCapacityStep = 16;
  {$IFEND}

  procedure _GenVector.SetValue(const Index: __CollectionIndexType; const e: _VectorValueType);
  begin
    {$IFDEF _RANGECHECK}
      if (Index < 0) or (Index >= FSize) then Assert(False, 'Range check error');
    {$ENDIF}
    FValues^[Index] := e;
  end;

  procedure _GenVector.SetCapacity(const ACapacity: __CollectionIndexType);
  {$IFNDEF _NO_INIT_NEEDED}
  var initStart: __CollectionIndexType;
  {$ENDIF}
  begin
    {$IFNDEF _NO_INIT_NEEDED}
    if ACapacity > FCapacity then
      initStart := FCapacity
    else
      initStart := -1;
    {$ENDIF}
    FCapacity := ACapacity;
    ReallocMem(FValues, Ord(FCapacity) * SizeOf(_VectorValueType));
    {$IFNDEF _NO_INIT_NEEDED}
    if initStart >= 0 then
      FillChar(FValues^[initStart], (FCapacity - initStart) * SizeOf(_VectorValueType), 0);
    initStart := FCapacity * SizeOf(_VectorValueType);
    {$ENDIF}
    if FSize > FCapacity then FSize := FCapacity;
  end;

  constructor _GenVector.Create();
  begin
    Create(_VectorDefaultCapacity);
  end;

  constructor _GenVector.Create(const Capacity: __CollectionIndexType);
  begin
    inherited Create();
    SetCapacity(Capacity);
  end;

  constructor _GenVector.Create(const arr: array of _VectorValueType);
  var i: Integer;
  begin
    FSize := Length(arr);
    SetCapacity(FSize);
    for i := 0 to FCapacity-1 do FValues^[i] := arr[i];
  end;

  destructor _GenVector.Destroy();
  begin
    SetCapacity(0);
    FValues := nil;
    inherited;
  end;

  function __MaxI(const V1, V2: __CollectionIndexType): __CollectionIndexType; {$I inline.inc}
  begin
    Result := V1 * Ord(V1 >= V2) + V2 * Ord(V1 < V2);
  end;

  procedure _GenVector.EnsureCapacity(const ASize: __CollectionIndexType);
  begin
    if ASize > FCapacity then SetCapacity(__MaxI(ASize, FCapacity + _VectorCapacityStep));
    Assert(FCapacity >= ASize, 'Capacity low');
  end;

  function _GenVector.GetSize(): __CollectionIndexType;
  begin
    Result := FSize;
  end;

  procedure _GenVector.SetSize(const ASize: __CollectionIndexType);
  begin
    EnsureCapacity(ASize);
    FSize := ASize;
  end;

  function _GenVector.IsEmpty(): Boolean;
  begin
    Result := FSize = 0;
  end;

  function _GenVector.Contains(const e: _VectorValueType): Boolean;
  begin
    Result := IndexOf(e) >= 0;
  end;

  procedure _GenVector.ForEach(Delegate: _VectorDelegate; Data: Pointer);
  var i: Integer;
  begin
    for i := 0 to FSize-1 do Delegate(FValues^[i], Data);
  end;

  procedure _GenVector.ForEach(Callback: _VectorCallback; Data: Pointer);
  var i: Integer;
  begin
    for i := 0 to FSize-1 do Callback(FValues^[i], Data);
  end;

  {$IF Declared(_VectorSearchType)}
  function _GenVector.Find(const Pattern: _VectorSearchType): __CollectionIndexType;
  begin
    Result := 0;
    while (Result < FSize) do
      if _VectorFound(FValues^[Result], Pattern) then Break else Inc(Result);
    if Result >= FSize then Result := -1;
  end;

  function _GenVector.FindLast(const Pattern: _VectorSearchType): __CollectionIndexType;
  begin
    Result := FSize-1;
    while (Result >= 0) do
      if _VectorFound(FValues^[Result], Pattern) then Break else Dec(Result);
  end;
  {$IFEND}

  function _GenVector.Add(const e: _VectorValueType): Boolean;
  begin
    EnsureCapacity(FSize+1);
    FValues^[FSize] := e;
    Result := True;
    Inc(FSize);
  end;

  function _GenVector.Remove(const e: _VectorValueType): Boolean;
  var ind: __CollectionIndexType;
  begin
    ind := IndexOf(e);
    Result := ind >= 0;
    if Result then RemoveBy(ind);
  end;

  // Removes all elements from the collection
  procedure _GenVector.Clear();
  begin
    FSize := 0;
  end;

  function _GenVector.Get(const Index: __CollectionIndexType): _VectorValueType;
  begin
    {$IFDEF _RANGECHECK}
    if (Index < 0) or (Index >= FSize) then Assert(False, 'Range check error');
    {$ENDIF}
    Result := FValues^[Index];
  end;

  function _GenVector.GetPtr(const Index: __CollectionIndexType): _PVectorValueType;
  begin
    {$IFDEF _RANGECHECK}
      if (Index < 0) or (Index >= FSize) then Assert(False, 'Range check error');
    {$ENDIF}
    Result := @FValues^[Index];
  end;

  function _GenVector.Put(const Index: __CollectionIndexType; const e: _VectorValueType): _VectorValueType;
  begin
    Result := Get(Index);
    SetValue(index, e);
  end;

  procedure _GenVector.Insert(const Index: __CollectionIndexType; const e: _VectorValueType);
  var i: __CollectionIndexType;
  begin
    {$IFDEF _RANGECHECK}
      if (Index < 0) or (Index >= FSize) then Assert(False, 'Range check error');
    {$ENDIF}
    EnsureCapacity(FSize+1);
    for i := FSize-1 downto Index+1 do
      FValues^[i] := FValues^[i-1];
    FValues[Index] := e;
    Inc(FSize);
  end;

  function _GenVector.RemoveBy(const Index: __CollectionIndexType): _VectorValueType;
  var i: __CollectionIndexType;
  begin
    {$IFDEF _RANGECHECK}
      if (Index < 0) or (Index >= FSize) then Assert(False, 'Range check error');
    {$ENDIF}
    Result := FValues^[Index];
    for i := Index to FSize-2 do
      FValues^[i] := FValues^[i+1];
    Dec(FSize);
  end;

  function _GenVector.IndexOf(const e: _VectorValueType): __CollectionIndexType;
  begin
    Result := 0;
    while (Result < FSize) do
      {$IF Declared(_VectorEquals)}
        if _VectorEquals(FValues^[Result], e) then Break else Inc(Result);
      {$ELSE}
        if (FValues^[Result] = e) then Break else Inc(Result);
      {$IFEND}
    if Result >= FSize then Result := -1;
  end;

  function _GenVector.LastIndexOf(const e: _VectorValueType): __CollectionIndexType;
  begin
    Result := FSize-1;
    while (Result >= 0) do
      {$IF Declared(_VectorEquals)}
        if _VectorEquals(FValues^[Result], e) then Break else Dec(Result);
      {$ELSE}
        if (FValues^[Result] = e) then Break else Dec(Result);
      {$IFEND}
  end;

  {$UNDEF _VECTORIMPL}
{$ENDIF _VECTORIMPL}
