(*
  Generic hash table collection
  (C) George Bakhtadze
Usage - once per unit:
  type
    _HashMapKeyType = <some type>;
    _HashMapValueType = <some type>;
    {$I tpl_coll_hashmap.inc}

    T<xxx><yyy>HashMap = class(_GenHashMap)
    end;

  implementation

   [ function _HashMapHashFunc(const Key: _HashMapKeyType): __CollectionIndexType;
     begin
       Result := Ord(@Key);
     end;]

   [ function _HashMapKeyEquals(const Key1, Key2: _HashMapKeyType): Boolean;
     begin
       Result := Key1 = Key2;
     end;]

   [ function _HashMapValueEquals(const Value1, Value2: _HashMapValueType): Boolean;
     begin
       Result := Value1 = Value2;
     end;]

    // a compiler message with explanation which type is being instantiated would be very helpful:
    {$MESSAGE 'Instantiating TStringIntegerHashMap'}
    {$I tpl_coll_hashmap.inc}
*)

{$IFNDEF _HASHMAPIMPL}                 // Interface

{$IFDEF _IDE_PARSER_}                  // This block is for IDE parser only
unit tpl_coll_linkedlist;
interface
uses {!}template;
type
  _HashMapKeyType = Integer;
  _HashMapValueType = Integer;
{$ENDIF}

  {$IF not Declared(__CollectionIndexType)}
    __CollectionIndexType = Integer;
  {$IFEND}

  // Hash map key-value pair
  _HashMapPair = record Key: _HashMapKeyType; Value: _HashMapValueType; end;

  // Hash  map for each delegate
  _HashMapDelegate = procedure(const key: _HashMapKeyType; const value: _HashMapValueType; Data: Pointer) of object;

  // Data structure to store values of hash map
  _HashMapValueRec = record Count: __CollectionIndexType; Data: array of _HashMapPair; end;

  // Hash map keys collection type
  _HashMapKeys = array of _HashMapValueRec;

  _GenHashMap = class;

    (* Hash map forward iterator.
       No map modification protection or fail fast. *)
  _GenHashMapIterator = object
  private
    Map: _GenHashMap;
    BktI, DataI: __CollectionIndexType;
  public
    // Advances the iterator to next item and returns True on success or False if no items left
    function GoToNext(): Boolean;
    // Returns current key performing no iterator state changes
    function CurrentKey(): _HashMapKeyType; {$I inline.inc}
    // Returns current value performing no iterator state changes
    function CurrentValue(): _HashMapValueType; {$I inline.inc}
    // Returns True if there is more items
    function HasNext(): Boolean;
    // Advances the iterator to next item and returns it.
    // If no items left nil be returned for nullable collection (dsNullable option is defined) or error generated otherwise.
    function Next(): _HashMapPair; {$I inline.inc}
  end;

  _GenHashMap = class
  {$IFDEF HAS_STRICT}strict{$ENDIF} private
    function GetLoadFactor(): Single; {$I inline.inc}

  protected
    FValues: _HashMapKeys;
  {$IFDEF HAS_STRICT}strict{$ENDIF} protected
    // Capacity of the hash map. Should be a power of 2.
    FCapacity,
    // Capacity mask
    FMask: __CollectionIndexType;
    // Threshold of number of entries to capacity ratio after which capacity doubles. If zero automatic resizing doesn't occur.
    FMaxLoadFactor: Single;
    // Grow step of bucket array
    FBucketGrowStep: __CollectionIndexType;
    // Number of entries
    FCount: __CollectionIndexType;
    function GetIndexInBucket(const Key: _HashMapKeyType; out BucketIndex: __CollectionIndexType): __CollectionIndexType; {$I inline.inc}
    function GetValue(const Key: _HashMapKeyType): _HashMapValueType; {$I inline.inc}
    procedure SetValue(const Key: _HashMapKeyType; const Value: _HashMapValueType); {$I inline.inc}
    procedure SetCapacity(ACapacity: __CollectionIndexType);
  public
    constructor Create(); overload;
    // Create a hash map instance with the specified initial capacity which is rounded up to power of 2
    constructor Create(InitialCapacity: __CollectionIndexType); overload;
    destructor Destroy(); override;
    // Returns a forward iterator over map values
    function GetIterator(): _GenHashMapIterator;

    // Sets value, returns True if there was value in the map for the key
    function PutValue(const Key: _HashMapKeyType; const Value: _HashMapValueType): Boolean; {$I inline.inc}
    // Returns True if the hash map contains the key
    function ContainsKey(const Key: _HashMapKeyType): Boolean; {$I inline.inc}
    // Returns True if the hash map contains the value
    function ContainsValue(const Value: _HashMapValueType): Boolean; {$I inline.inc}
    // Removes value, returns True if there was value in the map for the key
    function RemoveValue(const Key: _HashMapKeyType): Boolean; {$I inline.inc}

    // Calls a delegate for each value stored in the map
    procedure ForEach(Delegate: _HashMapDelegate; Data: Pointer);

    // Returns True if the collection contains no elements
    function IsEmpty(): Boolean; {$I inline.inc}
    // Removes all elements from the collection
    procedure Clear(); {$I inline.inc}

    // Values retrieved by pointer key
    property Values[const Key: _HashMapKeyType]: _HashMapValueType read GetValue write SetValue; default;
    // Determines hash function values range which is currently used.
    property Capacity: __CollectionIndexType read FCapacity;
    // Threshold of number of entries to capacity ratio after which capacity doubles. If zero automatic resizing doesn't occur.
    property MaxLoadFactor: Single read FMaxLoadFactor write FMaxLoadFactor;
    // Current number of entries to capacity ratio
    property LoadFactor: Single read GetLoadFactor;
    // Grow step of bucket array
    property BucketGrowStep: __CollectionIndexType read FBucketGrowStep write FBucketGrowStep;
    // Number of entries
    property Count: __CollectionIndexType read FCount;
  end;

  {$DEFINE _HASHMAPIMPL}


{$IFDEF _IDE_PARSER_}{$DEFINE _IDE_DISABLE_CONDITIONALS_}{$ENDIF}
{$ELSE _HASHMAPIMPL}
{$IFDEF _IDE_PARSER_}{$UNDEF _IDE_DISABLE_CONDITIONALS_}{$ENDIF}

{$IFDEF _IDE_PARSER_}
implementation
{$ENDIF _IDE_PARSER_}

  {$IF Declared(_HashMapOptions)}
    {$I tpl_coll_hashmap_opt.inc}
  {$IFEND}

  {$IF not Declared(_HashMapDefaultCapacity)}
    const _HashMapDefaultCapacity = 16;
  {$IFEND}

  { _GenHashMapIterator }
  function _GenHashMapIterator.GoToNext(): Boolean;
  begin
    Result := False;
    Inc(DataI);
    while (BktI < Map.Capacity) and not Result do
      if DataI < Map.FValues[BktI].Count then
        Result := True
      else begin
        DataI := 0;
        Inc(BktI);
      end;
  end;

  function _GenHashMapIterator.CurrentKey(): _HashMapKeyType; {$I inline.inc}
  begin
    Result := Map.FValues[BktI].Data[DataI].Key;
  end;

  function _GenHashMapIterator.CurrentValue(): _HashMapValueType; {$I inline.inc}
  begin
    Result := Map.FValues[BktI].Data[DataI].Value;
  end;

  function _GenHashMapIterator.HasNext(): Boolean;
  var lDataI, lBktI: __CollectionIndexType;
  begin
    Result := False;
    lDataI := DataI;
    lBktI  := BktI;
    Inc(lDataI);
    while (lBktI < Map.Capacity) and not Result do
      if lDataI < Map.FValues[lBktI].Count then
        Result := True
      else begin
        lDataI := 0;
        Inc(lBktI);
      end;
  end;

  {$WARN NO_RETVAL OFF}
  function _GenHashMapIterator.Next(): _HashMapPair; {$I inline.inc}
  begin
    if GoToNext then
      Result := Map.FValues[BktI].Data[DataI]
    else
      Assert(False, 'No more elements');
  end;
  {$WARN NO_RETVAL ON}

  { _GenHashMap }

  {$IF not Declared(_HashMapHashFunc)}
    function _HashMapHashFunc(const Key: Integer): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}

    function _HashMapHashFunc(const Key: Pointer): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}

    {$WARN UNSAFE_CAST OFF}
    function _HashMapHashFunc(const Key: TObject): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    {$WARN UNSAFE_CAST ON}

    function _HashMapHashFunc(const Key: TClass): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}

    {$DEFINE Hash_Key_Float}
    function _HashMapHashFunc(const Key: Single): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    function _HashMapHashFunc(const Key: Double): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    {$IFDEF HAS_TYPE_EXTENDED}
    function _HashMapHashFunc(const Key: Extended): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    {$ENDIF}

    {$DEFINE Hash_Key_String}
    function _HashMapHashFunc(const Key: ShortString): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    function _HashMapHashFunc(const Key: AnsiString): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    function _HashMapHashFunc(const Key: WideString): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
    {$IFDEF UNICODE}
      function _HashMapHashFunc(const Key: UnicodeString): NativeUInt; overload; {$I inline.inc}
      {$I tpl_hash_funcs.inc}
    {$ENDIF}

    {$DEFINE Hash_Key_PChar}
    function _HashMapHashFunc(Key: PAnsiChar): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}

    function _HashMapHashFunc(Key: PWideChar): NativeUInt; overload; {$I inline.inc}
    {$I tpl_hash_funcs.inc}
  {$IFEND}


  function _GenHashMap.GetIndexInBucket(const Key: _HashMapKeyType; out BucketIndex: __CollectionIndexType): __CollectionIndexType;
  begin
    BucketIndex := _HashMapHashFunc(Key) and FMask;
    Result := 0;
    while (Result < FValues[BucketIndex].Count) do
      {$IF Declared(_HashMapKeyEquals)}
        if _HashMapKeyEquals(FValues[BucketIndex].Data[Result].Key, Key) then Break else Inc(Result);
      {$ELSE}
        if (FValues[BucketIndex].Data[Result].Key = Key) then Break else Inc(Result);
      {$IFEND}
  end;

  function _GenHashMap.GetValue(const Key: _HashMapKeyType): _HashMapValueType;
  var
    Index, i: __CollectionIndexType;
  begin
    i := GetIndexInBucket(Key, index);

    if i < FValues[Index].Count then
      Result := FValues[Index].Data[i].Value
    else
    begin
      {$IFDEF _HashMapTypeNullable}
        Result := nil;
      {$ELSE}
        Assert(False, 'Key not found');
      {$ENDIF}
    end;
  end;

  function _GenHashMap.PutValue(const Key: _HashMapKeyType; const Value: _HashMapValueType): Boolean;
  var Index, i: __CollectionIndexType;
  begin
    i := GetIndexInBucket(Key, Index);

    if i = FValues[Index].Count then begin
      if Length(FValues[Index].Data) <= i then
        SetLength(FValues[Index].Data, Length(FValues[Index].Data) + FBucketGrowStep);
      FValues[Index].Data[i].Key := Key;
      FValues[Index].Count := i+1;
      Inc(FCount);
      Result := False;
    end else
      Result := True;

    FValues[Index].Data[i].Value := Value;

    if (FMaxLoadFactor > 0) and (FCount > FMaxLoadFactor * FCapacity) then
      SetCapacity(FCapacity * 2);
  end;

  procedure _GenHashMap.SetValue(const Key: _HashMapKeyType; const Value: _HashMapValueType);
  begin
    PutValue(Key, Value);
  end;

  function _GenHashMap.RemoveValue(const Key: _HashMapKeyType): Boolean;
  var Index, i: __CollectionIndexType;
  begin
    i := GetIndexInBucket(Key, Index);
    Result := i < FValues[Index].Count;
    if Result then begin
      FValues[Index].Data[i] := FValues[Index].Data[FValues[Index].Count - 1];
      Dec(FValues[Index].Count);
      Dec(FCount);
    end;
  end;

  procedure _GenHashMap.SetCapacity(ACapacity: __CollectionIndexType);
  var
    i, j: __CollectionIndexType;
    OldValues, NewValues: _HashMapKeys;
  begin
    Assert((ACapacity > 0), 'Capacity should be greater then zero');
    if (ACapacity and (ACapacity-1) <> 0) then
    begin
      i := 1;
      while i < ACapacity do
        i := i*2;
      ACapacity := i;
    end;

    // Save values
    OldValues := FValues;

    // Allocate new values
    FCapacity := ACapacity;
    SetLength(NewValues, FCapacity);
    FValues := NewValues;
    FMask := FCapacity-1;
    FCount := 0;

    // Insert old values into clean map
    for i := 0 to High(OldValues) do
      for j := 0 to OldValues[i].Count-1 do
        SetValue(OldValues[i].Data[j].Key, OldValues[i].Data[j].Value);
  end;

  function _GenHashMap.GetLoadFactor(): Single; {$I inline.inc}
  begin
    Result := FCount / FCapacity;
  end;

  constructor _GenHashMap.Create();
  begin
    Create(_HashMapDefaultCapacity);
  end;

  constructor _GenHashMap.Create(InitialCapacity: __CollectionIndexType);
  begin
    inherited Create();
    FBucketGrowStep := 1;
    FMaxLoadFactor := 0.75;
    SetCapacity(InitialCapacity);
  end;

  destructor _GenHashMap.Destroy();
  begin
    Clear;
    SetLength(FValues, 0);
    inherited;
  end;

  function _GenHashMap.GetIterator(): _GenHashMapIterator;
  begin
    Result.BktI  := 0;
    Result.DataI := -1;
    Result.Map   := Self;
  end;

  function _GenHashMap.ContainsKey(const Key: _HashMapKeyType): Boolean;
  var Index: __CollectionIndexType;
  begin
    Result := GetIndexInBucket(Key, index) < FValues[Index].Count;
  end;

  function _GenHashMap.ContainsValue(const Value: _HashMapValueType): Boolean;
  var i, j: __CollectionIndexType;
  begin
    Result := True;
    for i := 0 to High(FValues) do
      for j := 0 to FValues[i].Count-1 do
        {$IF Declared(_HashMapValueEquals)}
          if _HashMapValueEquals(FValues[i].Data[j].Value, Value) then Exit;
        {$ELSE}
          if (FValues[i].Data[j].Value = Value) then Exit;
        {$IFEND}
    Result := False;
  end;

  procedure _GenHashMap.ForEach(Delegate: _HashMapDelegate; Data: Pointer);
  var i, j: __CollectionIndexType;
  begin
    for i := 0 to High(FValues) do
      for j := 0 to FValues[i].Count-1 do
        Delegate(FValues[i].Data[j].Key, FValues[i].Data[j].Value, Data);
  end;

  function _GenHashMap.IsEmpty(): Boolean;
  begin
    Result := FCount = 0;
  end;

  procedure _GenHashMap.Clear();
  var i: __CollectionIndexType;
  begin
    FCount := 0;
    for i := 0 to High(FValues) do begin
      SetLength(FValues[i].Data, 0);
      FValues[i].Count := 0;
    end;
  end;

  {$UNDEF _HASHMAPIMPL}
{$ENDIF _HASHMAPIMPL}
