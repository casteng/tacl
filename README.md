# Template Algorithms and Collections Library

The library provides Object Pascal implementations of generic containers and algorithms
based on "pseudo templates" approach.

The approach is based on type renaming and incudes and does not require use of generics.

Supported compilers: **FPC 2.6.4+**, **Delphi 7+**.

Current library version: **1.0**

## Why not use generics?

### Advantages of the approach

* Performance: Generics without aggressive compiler optimization has performance problems (see benchmark)
* Flexibility: Containers and algorithms may be parametrized with code as well (comparators etc)
* Stability: Generics in FPC and Delphi are still unstable
* Compatibility: Old compilers such as Delphi 7 can be supported

### Disadvantages

Pseudo templates has limitations.

* One container specialization per unit. Possible workaround are nested class types.
* Error reports may be unclear. Compiler hints and source documentation is a workaround.
* Weird usage syntax

As basic collections and algorithms should be as fast as possible it makes sense to use pseudo templates for them.

## Currently supported

* List
  * Linked list
  * Vector
* Map
  * Hash map
* Sorting
  * Quick sort

## Usage

### Vector collection

In its simplest form the usage is:

```pascal
...
  type
    _VectorValueType = Integer;                           // Assign desired type to _VectorValueType. This will be the type of the vector elements.
    {$MESSAGE 'Instantiating TIntegerVector interface'}   // Optional compiler message may be helpful for debugging
    {$I tpl_coll_vector.inc}                              // Include interface part of the template
    TIntegerVector = _GenVector;                          // Rename declared in template class to something reasonable
...
  implementation

    {$MESSAGE 'Instantiating TIntegerVector'}             // Optional compiler message may be helpful for debugging
    {$I tpl_coll_vector.inc}                              // Include implementation part of the template
```

There are optional type parameters:

    _VectorSearchType = <some type>;                      // Type parameter to search in vector by some pattern with Find() method. If the type is specified _VectorFound() function should also be declared.
    __CollectionIndexType = Integer;                     // Specify which type should be used to indexing the vector if Integer is not suitable

Before including implementation part the following parameters may be specified:
```pascal
    {$DEFINE _RANGECHECK}                                 // With this define a range check will be performed for each access to a vector element
    const _VectorDefaultCapacity = 16;                    // Default capacity of the vector
    const _VectorCapacityStep = 16;                       // Capacity growing step
    
    // Optional equals function can be declared as _VectorEquals()
    function _VectorEquals(const v1, v2: _VectorValueType): Boolean;
    begin
      Result := v1 = v2;
    end;
    
    // If _VectorSearchType was declared the _VectorFound should be declared and return True if v matches Pattern
    function _VectorFound(const v: _VectorValueType; const Pattern: _VectorSearchType): Boolean;
    begin
      Result := Matches(v, Pattern);
    end;
```

After this TIntegerVector instances can be created and used:
```pascal
  Data := TIntegerVector.Create();
  Data.Count := 100;                    // Set number of elements
  Data[0] := 10;                        // Access element as with usual array
```

### Linked list collection

In its simplest form the usage is:

```pascal
  type
    _LinkedListValueType = Integer;                           // Assign desired type to _LinkedListValueType. This will be the type of the linked list nodes.
    {$MESSAGE 'Instantiating TIntegerLinkedList interface'}   // Optional compiler message may be helpful for debugging 
    {$I tpl_coll_linkedlist.inc}                              // Include interface part of the template
    TIntegerLinkedList = _GenLinkedList                       // Rename declared in template class  to something reasonable
...
  implementation

     {$MESSAGE 'Instantiating TIntegerLinkedList'}            // Optional compiler message may be helpful for debugging
     {$I tpl_coll_linkedlist.inc}                             // Include implementation part of the template
```

There are optional type parameters:

    _LinkedListNode = <some type>;                            // Type used as node record can be overridden
    __CollectionIndexType = Integer;                          // Specify which type should be used to indexing the list if Integer is not suitable

Before including implementation part the following parameters may be specified:
```pascal
    {$DEFINE _RANGECHECK}                                     // With this define a range check will be performed for each indexed access to a list element
    
    // Optional equals function can be declared as _LinkedListEquals() to be used in methods like IndexOf()
    function _LinkedListEquals(const v1, v2: _LinkedListValueType): Boolean;
    begin
      Result := v1 = v2;                                      // actual comparing code
    end;
```

### Hash map collection

```pascal
  type
    _HashMapKeyType = string;                                  // Type used as map key
    _HashMapValueType = string;                                // Type used as map value
    {$MESSAGE 'Instantiating TStringStringHashMap interface'}  // Optional compiler message may be helpful for debugging 
    {$I tpl_coll_hashmap.inc}                                  // Include interface part of the template
    TStringStringHashMap = _GenHashMap                         // Rename declared in template class to something reasonable
...
  implementation

  {$MESSAGE 'Instantiating TStringIntegerHashMap'}             // Optional compiler message may be helpful for debugging
  {$I tpl_coll_hashmap.inc}                                    // Include implementation part of the template
```

There are optional type parameters:

    __CollectionIndexType = Integer;                          // Specify which type should be used to indexing the list if Integer is not suitable
    ...

Before including implementation part the following parameters may be specified:
```pascal
    const _HashMapDefaultCapacity = 16;                       // Default hash map capacity

    // Hash function can be specified explicitly
    function _HashMapHashFunc(const Key: _HashMapKeyType): __CollectionIndexType;
    begin
      Result := Ord(@Key);
    end;

    // Optional key equals function can be declared as _HashMapKeyEquals() to be used instead of "="
    function _HashMapKeyEquals(const Key1, Key2: _HashMapKeyType): Boolean;
    begin
      Result := Key1 = Key2;
    end;

    // Optional value equals function can be declared as _HashMapValueEquals() to be used instead of "="
    function _HashMapValueEquals(const Value1, Value2: _HashMapValueType): Boolean;
    begin
      Result := Value1 = Value2;
    end;
```

## Remarks

Tests require a tester.pas from here: https://github.com/casteng/g3commons

## Benchmark

Results of the included benchmark on **FPC 3.0.4** with **-O2** option:

### List (10 000 000 items)

    Static array. Random write access: 150 ms
    Static array. Sequential read access: 27 ms
    Static array. Random read access: 238 ms
    Static array. Control sum: 63193567528462
    Template vector. Random write access: 149 ms
    Template vector. Sequential read access: 28 ms
    Template vector. Random read access: 245 ms
    Template vector. Control sum: 63193567528462
    Dynamic array. Random write access: 149 ms
    Dynamic array. Sequential read access: 28 ms
    Dynamic array. Random read access: 245 ms
    Dynamic array. Control sum: 63193567528462
    TList. Random write access: 447 ms
    TList. Sequential read access: 120 ms
    TList. Random read access: 837 ms
    TList. Control sum: 63193567528462
    Generic List. Random write access: 196 ms
    Generic List. Sequential read access: 49 ms
    Generic List. Random read access: 346 ms
    Generic List. Control sum: 63193567528462

### Map (1 000 000 items)

    Benchmarking Template map
    Template map. Put: 607 ms
    Template map. Sequential get: 145 ms
    Template map. Random get: 252 ms
    Template map. Control sum: -1923252871
    Template map. Deletion: 77 ms
    Template map. Iteration: 15 ms
    Template map. Control sum: 1338095247
    Benchmarking GHashMap
    GHashMap. Put: 1229 ms
    GHashMap. Sequential get: 663 ms
    GHashMap. Random get: 838 ms
    GHashMap. Control sum: -1923252871
    GHashMap. Deletion: 339 ms
    GHashMap. Iteration: 101 ms
    GHashMap. Control sum: 1338095247


According to the results template vector has the same speed as static array and generic list from FGL library is ~1.5 times slower.
Template map is faster than GHashMap from 2 to 6.5 times.
