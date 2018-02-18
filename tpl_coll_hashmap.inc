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
  _HashMapPairs = array[0..0] of _HashMapPair;
  _PHashMapPairs = ^_HashMapPairs;

  // Hash  map for each delegate
  _HashMapDelegate = procedure(const key: _HashMapKeyType; const value: _HashMapValueType; Data: Pointer) of object;

  // Data structure to store values of hash map
  _HashMapValueRec = record
    Count: __CollectionIndexType;
    Data: _PHashMapPairs;
    Pair: _HashMapPair;
  end;

  // Hash map value records collection type
  _HashMapValues = array[0..0] of _HashMapValueRec;
  _PHashMapValues = ^_HashMapValues;

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
    FValues: _PHashMapValues;
  {$IFDEF HAS_STRICT}strict{$ENDIF} protected
    // Capacity of the hash map. Should be a power of 2.
    FCapacity,
    // Capacity mask
    FMask: __CollectionIndexType;
    // Threshold of number of entries to capacity ratio after which capacity doubles. If zero automatic resizing doesn't occur.
    FMaxLoadFactor: Single;
    // Number of entries
    FCount: __CollectionIndexType;
    function GetIndexInBucket(const Key: _HashMapKeyType; out BucketIndex: __CollectionIndexType): __CollectionIndexType;
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

    // Sets value, returns True if there was no value in the map for the key
    function PutValue(const Key: _HashMapKeyType; const Value: _HashMapValueType): Boolean;
    // Returns True if the hash map contains the key
    function Contains(const Key: _HashMapKeyType): Boolean; {$I inline.inc}
    // Returns True if the hash map contains the value
    function ContainsValue(const Value: _HashMapValueType): Boolean; {$I inline.inc}
    // Removes value, returns True if there was value in the map for the key
    function Remove(const Key: _HashMapKeyType): Boolean; {$I inline.inc}

    // Calls a delegate for each value stored in the map
    procedure ForEach(Delegate: _HashMapDelegate; Data: Pointer);

    // Returns True if the collection contains no elements
    function IsEmpty(): Boolean; {$I inline.inc}
    // Removes all elements from the collection
    procedure Clear(); {$I inline.inc}

    // Values retrieved by pointer key
    property Values[const Key: _HashMapKeyType]: _HashMapValueType read GetValue write SetValue; default;
    // Determines hash function values range which is currently used.
    property Capacity: __CollectionIndexType read FCapacity write SetCapacity;
    // Threshold of number of entries to capacity ratio after which capacity doubles. If zero automatic resizing doesn't occur.
    property MaxLoadFactor: Single read FMaxLoadFactor write FMaxLoadFactor;
    // Current number of entries to capacity ratio
    property LoadFactor: Single read GetLoadFactor;
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

  {$Q-}
  {$R-}
  {$IF Declared(_HashMapOptions)}
    {$I tpl_coll_hashmap_opt.inc}
  {$IFEND}

  {$IF not Declared(_HashMapDefaultCapacity)}
    const _HashMapDefaultCapacity = 16;
  {$IFEND}

  {$IF not Declared(PtrUInt)}
    type
    {$IF Declared(NativeUInt)}
      PtrUInt = NativeUInt;
    {$ELSE}
      PtrUInt = Cardinal;
    {$IFEND}
      PPtrUInt = ^PtrUInt;
  {$IFEND}

  { _GenHashMapIterator }
  function _GenHashMapIterator.GoToNext(): Boolean;
  begin
    Result := False;
    Inc(DataI);
    while (BktI < Map.Capacity) and not Result do
      if DataI < Map.FValues^[BktI].Count then
        Result := True
      else begin
        DataI := 0;
        Inc(BktI);
      end;
  end;

  function _GenHashMapIterator.CurrentKey(): _HashMapKeyType; {$I inline.inc}
  begin
    if DataI = 0 then
      Result := Map.FValues^[BktI].Pair.Key
    else
      Result := Map.FValues^[BktI].Data^[DataI-1].Key;
  end;

  function _GenHashMapIterator.CurrentValue(): _HashMapValueType; {$I inline.inc}
  begin
    if DataI = 0 then
      Result := Map.FValues^[BktI].Pair.Value
    else
      Result := Map.FValues^[BktI].Data^[DataI-1].Value;
  end;

  function _GenHashMapIterator.HasNext(): Boolean;
  var lDataI, lBktI: __CollectionIndexType;
  begin
    Result := False;
    lDataI := DataI;
    lBktI  := BktI;
    Inc(lDataI);
    while (lBktI < Map.Capacity) and not Result do
      if lDataI < Map.FValues^[lBktI].Count then
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
    begin
      if DataI = 0 then
        Result := Map.FValues^[BktI].Pair
      else
        Result := Map.FValues^[BktI].Data^[DataI-1];
    end else
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
    if Result >= FValues^[BucketIndex].Count then
      Exit;
    {$IF Declared(_HashMapKeyEquals)}
    if _HashMapKeyEquals(FValues^[BucketIndex].Pair.Key, Key) then Exit;
    {$ELSE}
    if (FValues[BucketIndex].Pair.Key = Key) then Exit;
    {$IFEND}
    while (Result < FValues^[BucketIndex].Count-1) do
      {$IF Declared(_HashMapKeyEquals)}
        if _HashMapKeyEquals(FValues^[BucketIndex].Data^[Result].Key, Key) then Break else Inc(Result);
      {$ELSE}
        if (FValues[BucketIndex].Data[Result].Key = Key) then Break else Inc(Result);
      {$IFEND}
    Inc(Result);
  end;

  function _GenHashMap.GetValue(const Key: _HashMapKeyType): _HashMapValueType;
  var
    Index, i: __CollectionIndexType;
  begin
    i := GetIndexInBucket(Key, index);    //TODO: return -1 for single value case

    if i < FValues^[Index].Count then
    begin
      if i = 0 then
        Result := FValues^[Index].Pair.Value
      else
        Result := FValues^[Index].Data^[i-1].Value;
    end else begin
      {$IFDEF _HashMapTypeNullable}
        Result := nil;
      {$ELSE}
        Assert(False, 'Key not found');
      {$ENDIF}
    end;
  end;

  function _GenHashMap.PutValue(const Key: _HashMapKeyType; const Value: _HashMapValueType): Boolean;
  var
    Index, i: __CollectionIndexType;
    Pair: ^_HashMapPair;
  begin
    if (FMaxLoadFactor > 0) and (FCount + 1 > FMaxLoadFactor * FCapacity) then
      SetCapacity(FCapacity * 2);

    i := GetIndexInBucket(Key, Index);
    Result := i = FValues^[Index].Count;

    if i = 0 then
      Pair := @FValues^[Index].Pair
    else begin
      if Result then
        ReallocMem(FValues^[Index].Data, i * SizeOf(_HashMapPair));
      Pair := @FValues^[Index].Data^[i-1];
    end;

    if Result then
    begin
      FillChar(Pair^, SizeOf(_HashMapPair), 0);
      Pair^.Key := Key;
      Inc(FCount);
      FValues^[Index].Count := i + 1;
    end;

    Pair^.Value := Value;
  end;

  procedure _GenHashMap.SetValue(const Key: _HashMapKeyType; const Value: _HashMapValueType); {$I inline.inc}
  begin
    PutValue(Key, Value);
  end;

  procedure _GenHashMap.SetCapacity(ACapacity: __CollectionIndexType);
  var
    i, j, OldCapacity: __CollectionIndexType;
    OldValues, NewValues: _PHashMapValues;
    c: __CollectionIndexType;
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
    OldCapacity := FCapacity;

    FCapacity := ACapacity;

    // Allocate new values
    GetMem(NewValues, ACapacity * SizeOf(_HashMapValueRec));
    FillChar(NewValues^, ACapacity * SizeOf(_HashMapValueRec), 0);
    FValues := NewValues;
    FMask := ACapacity-1;
    FCount := 0;
    // Insert old values into clean map
    for i := 0 to OldCapacity-1 do
    begin
      c := OldValues^[i].Count;
      if c > 0 then
      begin
        SetValue(OldValues^[i].Pair.Key, OldValues^[i].Pair.Value);
        Finalize(OldValues^[i].Pair.Key);
        Finalize(OldValues^[i].Pair.Value);
      end;
      for j := 0 to c-2 do
      begin
        SetValue(OldValues^[i].Data^[j].Key, OldValues^[i].Data^[j].Value);
        Finalize(OldValues^[i].Data^[j].Key);
        Finalize(OldValues^[i].Data^[j].Value);
      end;
      Freemem(OldValues^[i].Data);
    end;
    Freemem(OldValues);
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
    FMaxLoadFactor := 0.75;
    SetCapacity(InitialCapacity);
  end;

  destructor _GenHashMap.Destroy();
  begin
    Clear;
    FreeMem(FValues, FCapacity * SizeOf(_HashMapValueRec));
    FValues := nil;
    inherited;
  end;

  function _GenHashMap.GetIterator(): _GenHashMapIterator;
  begin
    Result.BktI  := 0;
    Result.DataI := -1;
    Result.Map   := Self;
  end;

  function _GenHashMap.Contains(const Key: _HashMapKeyType): Boolean;
  var Index: __CollectionIndexType;
  begin
    Result := GetIndexInBucket(Key, index) < FValues^[Index].Count;
  end;

  function _GenHashMap.ContainsValue(const Value: _HashMapValueType): Boolean;  // TODO: fix
  var i, j: __CollectionIndexType;
  begin
    Result := True;
    for i := 0 to FCapacity-1 do
    begin
      {$IF Declared(_HashMapValueEquals)}
        if _HashMapValueEquals(FValues^[i].Pair.Value, Value) then Exit;
      {$ELSE}
        if (FValues^[i].Pair.Value = Value) then Exit;
      {$IFEND}
      for j := 0 to FValues^[i].Count-2 do
        {$IF Declared(_HashMapValueEquals)}
          if _HashMapValueEquals(FValues^[i].Data^[j].Value, Value) then Exit;
        {$ELSE}
          if (FValues^[i].Data^[j].Value = Value) then Exit;
        {$IFEND}
    end;
    Result := False;
  end;

  function _GenHashMap.Remove(const Key: _HashMapKeyType): Boolean;
  var
    Index, i: __CollectionIndexType;
    c: __CollectionIndexType;
  begin
    i := GetIndexInBucket(Key, Index);
    c := FValues^[Index].Count;
    Result := i < c;
    if Result then
    begin
      if i = 0 then
      begin
        Finalize(FValues^[Index].Pair.Key);
        Finalize(FValues^[Index].Pair.Value);
        if c > 1 then
        begin
          Move(FValues^[Index].Data^[c - 2], FValues^[Index].Pair, SizeOf(_HashMapPair));
        end;
      end else begin
        Finalize(FValues^[Index].Data^[i-1].Key);
        Finalize(FValues^[Index].Data^[i-1].Value);
        if i - 1 < c - 2 then
          Move(FValues^[Index].Data^[c - 2], FValues^[Index].Data^[i-1], SizeOf(_HashMapPair));
      end;
      Dec(FValues^[Index].Count);
      Dec(FCount);
    end;
  end;

  procedure _GenHashMap.ForEach(Delegate: _HashMapDelegate; Data: Pointer);
  var i, j, c: __CollectionIndexType;
  begin
    for i := 0 to FCapacity-1 do
    begin
      c := FValues^[i].Count;
      if c > 0 then
        Delegate(FValues^[i].Pair.Key, FValues^[i].Pair.Value, Data);
      for j := 0 to c - 2 do
        Delegate(FValues^[i].Data^[j].Key, FValues^[i].Data^[j].Value, Data);
    end;
  end;

  function _GenHashMap.IsEmpty(): Boolean;
  begin
    Result := FCount = 0;
  end;

  procedure _GenHashMap.Clear();
  var i, j: __CollectionIndexType;
  begin
    FCount := 0;
    for i := 0 to FCapacity-1 do begin
      Finalize(FValues^[i].Pair.Key);
      Finalize(FValues^[i].Pair.Value);
      for j := 0 to FValues^[i].Count-2 do
      begin
        Finalize(FValues^[i].Data^[j].Key);
        Finalize(FValues^[i].Data^[j].Value);
      end;
      FreeMem(FValues^[i].Data);
      FValues^[i].Data := nil;
      FValues^[i].Count := 0;
    end;
  end;

  {$UNDEF _HASHMAPIMPL}
{$ENDIF _HASHMAPIMPL}
