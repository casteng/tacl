(*
  Template vector collection option constants
*)
  {$IF not Declared(NativeInt)}
    NativeInt = Integer;
  {$IFEND}
  {$IF (dsRangeCheck in _VectorOptions)}
    {$DEFINE _RANGECHECK}
  {$ELSE}
    {$UNDEF _RANGECHECK}
  {$IFEND}
