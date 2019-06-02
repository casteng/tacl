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
  {$IF (dsDisableInit in _VectorOptions)}
    {$DEFINE _NO_INIT_NEEDED}
  {$ELSE}
    {$UNDEF _NO_INIT_NEEDED}
  {$IFEND}
